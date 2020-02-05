function SocketCallbacks(io, spawn, path, performance, fs){
  var self = this;
  this.io = io;
  this.spawn = spawn;
  this.path = path;
  this.performance = performance;
  this.fs = fs;
  this.target_dir = self.path.join(__dirname, "/uploads/");
  this.send_data = false;
  this.uart = null;
  this.programmer = null;
    
  this.ask = function(question){
    switch(question.id){
      case "file_exist":
        var file = question.data;
        self.io.emit("answer", {
          id: "file_exist",
          data: fs.existsSync(self.path.join(self.target_dir, file))
        });
        break;
    }
  };

  this.command = function(params){
    var command = {
      cmd: '',
      args: [],
      options: {} 
    };
    var date = new Date();
    var read_file = self.path.join(__dirname, "downloads", "read_files", date.toISOString());

    //build command
    switch(params.firmware){
      case "picberry":
        var gpios = "";
        var action = "";
        var option = "";

        //prepare programming connector
        switch (params.connector) {
          case 'rj11':
              gpios = '19,20,21';
              break;
          case 'row':
              gpios = '5,6,12';
              break;
        }

        
        // prepare programming action
        switch (params.action) {
            case 'write':
                action = 'w ' + params.file;
                break;
            case 'read':
                action = 'r '+ read_file + '.hex';
                break;
            default:
                action = '-' + params.action;
                break;
        }

        // prepare additional option
        if(params.option == 'none'){
            option = '';
        }else{
            option = '--' + params.option;
        }

        // assemble command
        //command = 'picberry -' + action + ' --gpio=' + gpios + ' --family='  + params.family + ' ' + option;
        command.cmd = "picberry";
        command.args = ['-' + action, '--gpio=' + gpios, '--family='  + params.family, option];
        
        break;

      case "openocd":
        var driver_specific = "";
        var action = "";
        // prepare driver specific part
        switch(params.family){
          case "atsame5x.cfg":
            driver_specific = '-c "atsame5 chip-erase" -c "atsame5 bootloader 0';
            break;
          case "at91samdXX.cfg":
            driver_specific = '-c "atsame5 chip-erase" -c "atsame5 bootloader 0';
            break;
          case "atsamv.cfg":
            driver_specific = '-c "flash erase_sector 0 0 0"';
            break;
        }

        // prepare programming action
        switch (params.action) {
            case 'write': 
                action = driver_specific + ' -c "flash write_image ' + params.file + '" '; //-c "verify_image ' + params.file + ' 0x00000000" 
                break;
            case 'read':
                action = '-c "flash read_bank 0 ' + read_file + '.bin" ';
                break;
            case 'erase':
                action = driver_specific + ' -c "flash erase_check 0" ';
                break;
            case 'probe':
                action = '-c "flash probe 0" ';
                break;
            case 'info':
                action = '-c " flash info 0" ';
                break;
            case 'list':
                action = '-c "flash list" ';
                break;
            case 'banks':
                action = '-c "flash banks" ';
                break;  
        }

        // assemble command
        command.cmd = "openocd";
        command.args = ['-f', 'rpi.cfg', '-f', self.path.join('targets', params.family),  '-f', 'openocd.cfg', action, '-c', '"reset run"', '-c', '"shutdown"'];
        command.options = {
          cwd: self.path.join(__dirname, "OpenocdCustomConfigFiles")
        }
        break;
      case "pymcuprog":
        var action = "";

        switch(params.action){
          case "write":
              action = params.action + ' -f ' + params.file + ' --erase --verify';
            break;
          case "read":
              action = params.action + ' -f ' + read_file + '.hex';
            break;
          default:
            action = params.action;
        }
        
        command.cmd = "pymcuprog";
        command.args = [action, '-d ', params.family, '-t ', params.connector, '--' + params.option];

        break;
    }

    console.log(command);

    //spawn command
    self.programmer = self.spawn(command.cmd, command.args, command.options);
    self.programmer.stdout.setEncoding('utf-8');
    self.programmer.stderr.setEncoding('utf-8');

    // write log and emmit output
    var logger = self.fs.createWriteStream(self.path.join(__dirname, "downloads", "logs", 'log' + date.toISOString() + '.txt'), {
      flags: 'a'
    })
    
    var output_command_data = function(data){
      var dataparts = data.split("\n");
      dataparts.forEach(function(val){
        self.io.emit("command_output", val);
        logger.write(val + "\n");
      });
    };

    self.programmer.stdout.on("data", output_command_data);
    self.programmer.stderr.on("data", output_command_data);
    self.programmer.on("error", output_command_data);
  };

  this.kill_command = function(){
    if(self.programmer !== null){
      self.programmer.kill('SIGKILL');
    }
  };

  // on start event spawn uart.py and add event handlers 
  this.start = function(speed){
    console.log('starting uart');
    console.log("Speed: " + speed);
    self.send_data = true;
    self.uart = spawn("python3", ["-u", path.join(__dirname, 'uart.py'), "-v " + speed]);
    self.uart.stdout.setEncoding('utf-8');
    self.uart.stderr.setEncoding('utf-8');

    var n = self.performance.now();
    // handle data input from stdout of uart script
    self.uart.stdout.on("data", (data) => {
      console.log(self.performance.now() - n);
      n = self.performance.now();
      console.log(data);
      if(self.send_data){
        var parts = data.split(',');
        // check if data was send or a message
        if(parts.length > 1){
          // push data to server
          self.io.emit('data', {time: parts[0], yval0: parts[1], yval1: parts[2], yval2: parts[3]});
        }
      }
    });
    // handle data input from stderr of uart script
    self.uart.stderr.on("data", (data) => {
      console.log(data);
    });
    // handle error event from uart script
    self.uart.on("error", (msg) => {
      console.log(msg);
    });
  };
  
  // tell uart script to stop logging
  this.stop = function(){
    if(self.uart !== null){
      console.log('stopping uart');
      self.uart.stdin.write("stop", "utf-8");
      self.uart.stdin.end();
    }
  };

  // toggle pushing data to browser
  this.pause = function(){
    if(self.send_data === true){
      self.send_data = false;
    }else{
      self.send_data = true;
    }
  };
}

module.exports = SocketCallbacks;
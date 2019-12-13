/**
 * NodeJs/Socket server to feed the log graph of RaspiPgmLog
 */

 // require neccessary modules
var express = require('express');
var app = express();
var http = require('http').createServer(app);
var io = require('socket.io')(http);
var { spawn } = require('child_process');
const { PerformanceObserver, performance } = require('perf_hooks');
const path = require('path');
var send_data = false;
var n = performance.now();
var download_file = "";

// send website on request
app.get('/', function(req, res){
  res.sendFile(__dirname + '/graph_template.html');
  app.use(express.static(__dirname + '/public'));
});

// handle download request for logfile
app.get('/downloadfile/', (req, res) => {
  res.download(download_file);
})

// when someone enters the website, add event handlers
io.on('connection', function(socket){
  // on start event spawn uart.py and add event hadlers 
  socket.on('start', function(speed){
    console.log('starting uart');
    console.log("Speed: " + speed);
    send_data = true;
    uart = spawn("python3", ["-u", path.join(__dirname, 'uart.py'), "-v " + speed]);
    uart.stdout.setEncoding('utf-8');
    uart.stderr.setEncoding('utf-8');
    // handle data input from stdout of uart script
    uart.stdout.on("data", (data) => {
      console.log(performance.now() - n);
      n = performance.now();
      console.log(data);
      if(send_data){
        var parts = data.split(',');
        // check if data was send or a message
        if(parts.length > 1){
          // push data to server
          io.emit('data', {time: parts[0], yval0: parts[1], yval1: parts[2], yval2: parts[3]});
        }else{
          // handle message
          parts = data.split('\n');
          for(var i = 0; i < parts.length; i++){
            // check if the logfile name was send and save it if yes
            if(parts[i].indexOf("timeplot") > -1){
              download_file = parts[i];
            }
          }
        }
      }
    });
    // handle data input from stderr of uart script
    uart.stderr.on("data", (data) => {
      console.log(data);
    });
    // handle error event from uart script
    uart.on("error", (msg) => {
      console.log(msg);
    });
  });
  // tell uart script to stop  logging
  socket.on('stop', function(){
    console.log('stopping uart');
    uart.stdin.write("stop", "utf-8");
    uart.stdin.end();
  });
  // toggle pushing data to browser
  socket.on('pause', function(){
    if(send_data === true){
      send_data = false;
    }else{
      send_data = true;
    }
  });
});

// listen for incomming connections
http.listen(3000, function(){
  console.log('listening on *:3000');
});
var express = require('express');
var session = require('express-session');
var bodyParser = require('body-parser');
var path = require('path');
var app = express();
var http = require('http').createServer(app);
var io = require('socket.io')(http);
var fs = require("fs");
var formidable = require('formidable');
var { spawn } = require('child_process');
const { PerformanceObserver, performance } = require('perf_hooks');
var SocketCallbacks = require("./socket_callbacks");

var sc = new SocketCallbacks(io, spawn, path, performance, fs);

var datastorage = JSON.parse(fs.readFileSync("datastorage.json"));

// login mit vordefiniertem calback check
// datastorage mit json
app.set('view engine', 'ejs');

app.use(session({
	secret: 'secret',
	resave: true,
	saveUninitialized: true
}));
app.use(bodyParser.urlencoded({extended : true}));
app.use(bodyParser.json());
app.use(express.static(__dirname + '/public'));

app.get('/login', function(request, response){
    response.render("pages/login");
});

// send website on request
app.get('/', check_login, function(request, response){
    response.render("pages/frontpage", {
        picberry_families: datastorage.picberry_families,
        picberry_connectors: datastorage.picberry_connectors,
        picberry_actions: datastorage.picberry_actions,
        picberry_options: datastorage.picberry_options,
        openocd_families: datastorage.openocd_families,
        openocd_actions: datastorage.openocd_actions,
        openocd_options: datastorage.openocd_options,
        pymcuprog_chips: datastorage.pymcuprog_chips,
        pymcuprog_programmers: datastorage.pymcuprog_programmers,
        pymcuprog_actions: datastorage.pymcuprog_actions,
        pymcuprog_options: datastorage.pymcuprog_options
    });
});

app.get('/downloads', check_login, function(request, response){
    var logs = fs.readdirSync(path.join(__dirname, "downloads", "logs"));
    var read_files = fs.readdirSync(path.join(__dirname, "downloads", "read_files"));
    var timeplots = fs.readdirSync(path.join(__dirname, "downloads", "timeplots"));
    response.render("pages/downloads",{
        logs: logs,
        read_files: read_files,
        timeplots: timeplots
    });
});

app.get('/graph', check_login, function(request, response){
    response.render("pages/graph");
});

app.get('/help', check_login, function(request, response){
    response.render("pages/help");
});

app.post('/fileupload', function(request, response){
    var form = new formidable.IncomingForm();
    form.parse(request, function (err, fields, files) {
        var oldpath = files.upload.path;
        var newpath = path.join(sc.target_dir, files.upload.name);
        fs.rename(oldpath, newpath, function (err) {
            if (err) {
                response.send('File upload failed!');
                console.log(err);
            }else{
                response.send('File upload successfull!');
            }
            response.end();
        });
    });
});

// // handle download request for logfile
app.get('/downloadfile/', (request, response) => {
    file = request.query.file;
    console.log(file);
    response.download(file);
});

app.post('/auth', function(request, response) {
	var password = request.body.password;
	if (password === datastorage.password) {
        request.session.loggedin = true;
        response.redirect('/');
    } else {
        response.send('Incorrect Password!');
    }
		
});

io.on('connection', function(socket){
    socket.on('ask', sc.ask);
    socket.on('command', sc.command);
    socket.on('kill_command', sc.kill_command);
    socket.on("start", sc.start);
    socket.on("stop", sc.stop);
    socket.on("pause", sc.pause);
});

// listen for incomming connections
http.listen(3000, function(){
    console.log('listening on *:3000');
});

function check_login(request, response, next){
    if (!request.session.loggedin) {
        response.redirect('/login');
    }else{
        next();
    }
}
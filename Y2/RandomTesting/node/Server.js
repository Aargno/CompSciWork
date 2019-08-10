var http = require("http");
var fs = require("fs"); 
var port = 3000;
var serverUrl = "localhost";

console.log("Listening at " + serverUrl + ":" + port);

http.createServer(function(req, res) {

  fs.readFile('h/test.html',function (err, data){
    res.writeHead(200, {'Content-Type': 'text/html','Content-Length':data.length});
    res.write(data);
    res.end();
  });

}).listen(port, serverUrl);
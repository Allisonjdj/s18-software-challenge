var WebSocketServer = require('websocket').server;
var http = require('http');

var server = http.createServer(function(request, response) {
  // process HTTP request. Since we're writing just WebSockets
  // server we don't have to implement anything.
});
server.listen(3333, function() { console.log("Server running on port 3333") });

// create the server
wsServer = new WebSocketServer({ httpServer: server });

// WebSocket server
wsServer.on('request', function(request) {
    var connection = request.accept(null, request.origin);

    // This is the most important callback for us, we'll handle all messages from users here.
    connection.on('message', function(message) {
        // Read message
        if (message.type === 'utf8') {
            console.log("message:", message.utf8Data);
        }
        // Send message (just an example)
        connection.send(JSON.stringify([{"teamID": "1", "status": "Start"}, {"teamID": "2", "status": "Stop"}, {"teamID": "3", "status": "Slow Down"}]));
        setTimeout(function(){
            connection.send(JSON.stringify([{"teamID": "4", "status": "Start"}, {"teamID": "5", "status": "Stop"}, {"teamID": "6", "status": "Slow Down"}]));
            setTimeout(function(){
                connection.send(JSON.stringify([{"teamID": "1", "status": "Slow Down"}, {"teamID": "2", "status": "Start"}, {"teamID": "3", "status": "Crashed"}]));
                setTimeout(function(){
                    connection.send(JSON.stringify([{"teamID": "2", "status": "Slow Down"}, {"teamID": "4", "status": "Crashed"}, {"teamID": "6", "status": "Start"}]));
                }, 3000);
            }, 3000);
        }, 3000);
    });

    connection.on('close', function(connection) {
        // close user connection
    });
});

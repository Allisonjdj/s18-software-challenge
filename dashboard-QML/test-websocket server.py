#sudo pip install git+https://github.com/dpallot/simple-websocket-server.git
from SimpleWebSocketServer import SimpleWebSocketServer, WebSocket
import json
import time

class SimpleEcho(WebSocket):

    def handleMessage(self):
        # echo message back to client
        data = {
            ID : 1,
            Error : False,
            Type : "Command",
            Message : "IDLE command executed"
        }
        json_data = json.dumps(data, seperators=(':'))    
        #while True:
        time.sleep(0.1)
        self.sendMessage(json_data)
#        self.sendMessage(self.data)

    def handleConnected(self):
        print(self.address, 'connected')
        #time.sleep(0.15);
        
    def handleClose(self):
        print(self.address, 'closed')

server = SimpleWebSocketServer('', 3333, SimpleEcho)
server.serveforever()

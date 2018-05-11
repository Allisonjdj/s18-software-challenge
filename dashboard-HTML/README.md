# Arduino Challenge Dashboard (HTML Dashboard)

require node modules (just added websocket)
``` 
npm install 
```
## Front End Rendering
Current version is black text on white background but to add the Mars background image and make the text white uncomment lines 7 and 10 and comment lines 8 and 11.

When the entries are updated the most recent entry/entries are red font color. To change this color change the color in lines 34 and 36.


## Server and Web Socket
Connects to a server on localhost:3333

app.js contains an example web server with web socket that was used for testing

Data: Server sends an array of objects to the client (objects are of form {"teamID": 1, "status": "Start"} as an example) 

## Run
Open the HTML file in a browser

Run app.js or a server with a web socket on localhost:3333

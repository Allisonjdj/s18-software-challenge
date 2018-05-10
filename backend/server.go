package main

import (
    "./serial"
    "log"
    "time"
    "net/http"
    "github.com/gorilla/websocket"
    "encoding/json"
)

var upgrader = websocket.Upgrader{
    CheckOrigin: func(r *http.Request) bool {
        return true
    },
}

var lastValue uint8

func main() {
    wsconns := make(map[*websocket.Conn]bool, 0)

    http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
        // Upgrade the http connection to a WebSocket connetion
        c, err := upgrader.Upgrade(w, r, nil)
        if err != nil {
            log.Println("err: upgrade:", err);
            return
        }
        defer c.Close()

        // Add it to the set of connections
        wsconns[c] = true
        defer delete(wsconns, c)

        // Wait for messages and deal with them
        for {
            _, message, err := c.ReadMessage()
            if err != nil {
                log.Println("err: read:", err);
                break
            }
            log.Printf("ws-recv: path{%s} message{%s}", r.URL.Path, string(message))
        }
    })

    log.Println("About to listen on :3333")
    go func() { log.Fatal(http.ListenAndServe(":3333", nil)) }()

    // Open the UART connection.
    if err := serial.Setup(9600, "/dev/cu.usbmodem14131"); err != nil {
        log.Fatal(err)
    }

    defer serial.Close()

    time.Sleep(1 * time.Second) // Sometimes Arduinos need a bit of time to reset after connecting.

    for {
        // Read exact number of bytes, waiting for more if necessary.
        packet := serial.Read()

        if packet != nil && packet.Id != lastValue {
            id := packet.Id
            errorOccurred := false
            messageType := ""
            message := ""

            if packet.Id == 0x15 {
                errorOccurred = true
                messageType = "Packet"
                message = "Illegal Packet received"
            } else if packet.Id == 0x16 {
                errorOccurred = true
                messageType = "Pod"
                message = "Pod Crashed Boom!!"
            } else {
                errorOccurred = false
                messageType = "Command"

                switch packet.Id {
                case 0x01:
                    message = "IDLE command executed"
                    break
                case 0x02:
                    message = "READY command executed"
                    break
                case 0x03:
                    message = "ACCEL command executed"
                    break
                case 0x04:
                    message = "COAST command executed"
                    break
                case 0x05:
                    message = "BRAKE command executed"
                    break
                case 0x06:
                    message = "STOP command executed"
                    break
                case 0x07:
                    errorOccurred = true
                    message = "Command rejected by State machine"
                    break
                default:
                    errorOccurred = true
                    message = "Illegal Command received"
                }
            }

            lastValue = packet.Id
            // Marshal the packet data into a JSON string
            stringified, err := json.Marshal(struct {
                ID      uint8
                Error   bool
                Type    string
                Message string
            }{
                ID:      id,
                Error:   errorOccurred,
                Type:    messageType,
                Message: message,
            })
            if err != nil {
                log.Fatal(err)
            }
            log.Printf("uart-recv: message{%s}", string(stringified))

            // Send the string to each websocket connection
            for c := range wsconns {
                c.WriteMessage(websocket.TextMessage, stringified)
            }
        }
    }
}

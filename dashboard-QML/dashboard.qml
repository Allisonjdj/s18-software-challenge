import QtQuick 2.10
import QtQuick.Window 2.10
import QtWebSockets 1.0


Window {
    visible: true
    width: 1280
    height: 800
    title: qsTr("Software Challenge Dashboard")

    property string cmd;
    property string stat;

    function updateCommand(com) {
        cmd = com;
    }

    function updatePodState(sta) {
        stat = sta;
    }

    Rectangle {
        width: 845; height: 800
        color: "#c6c6c6"

        Component {
            id: podDelegate
            Item {
                width: 845; height: 200
                Column {
                    Text { id: teamID; font.family: "Arial Narrow"; horizontalAlignment: Text.AlignHCenter; font.pointSize: 20;
                           text: '<b>Team ID:</b> ' + team }
                    Text { id: commandID; font.family: "Arial Narrow"; horizontalAlignment: Text.AlignHCenter; font.pointSize: 20;
                           text: '<b>Command:</b> ' + commands }
                    Text { id: stateID; font.family: "Arial Narrow"; horizontalAlignment: Text.AlignHCenter; font.pointSize: 19;
                           text: '<b>Pod State:</b> ' + status }
                }
            }
        }

        ListView {
            anchors.fill: parent
            model: PodModel{}
            delegate: podDelegate
            //highlight: Rectangle { color: "gold"; radius: 10 }
            focus: true
        }
    }
    Rectangle {
        x: 845
        y: 0
        width: 435
        height: 800
        color: "#848484"
        WebSocket {
                id: socket
                url: "ws://localhost:3333"
                onTextMessageReceived: {
                    messageBox.text = messageBox.text + "\nReceived message: " + message

                    // TID - team ID
                    // Num - Pod State
                    var JsonString = message;
                    var JsonObject = parse(JsonString);
                    updatePodState(JsonObject.Num);

                }
                onStatusChanged: if (socket.status == WebSocket.Error) {
                                     console.log("Error: " + socket.errorString)
                                 } else if (socket.status == WebSocket.Open) {
                                     socket.sendTextMessage("Socket Open")
                                     messageBox.text = "Message Sent"
                                 } else if (socket.status == WebSocket.Closed) {
                                     messageBox.text += "\nSocket closed"
                                 }
                active: true
        }
       Text {
                id: messageBox
                text: socket.status == WebSocket.Open ? qsTr("Sending...") : qsTr("Welcome!")
                anchors.centerIn: parent
            }

       MouseArea {
            anchors.fill: parent
            onClicked: {
                socket.active = !socket.active
                //Qt.quit();
            }
        }
    }
}

import QtQuick 2.10
import QtQuick.Window 2.10
import QtWebSockets 1.0


Window {
    visible: true
    width: 1280
    height: 800
    title: qsTr("Software Challenge Dashboard")

    property int tid;
    property string err;
    property string typ;
    property string messag;

//    function updateError(com) {
//        err = com;
//    }

//    function updateType(sta) {
//        typ = sta;
//    }

//    function updateMessage(msg) {
//        messag = msg;
//    }

    function getColour(errorString) {
        if (errorString == "True")
            return "red"
        else
            return "gold"
    }

    Rectangle {
        width: 845; height: 800
        color: "#c6c6c6"

        ListView {

            id: podView
            // ID - team ID - uint8
            // Error - Bool
            // Type - String oneof("Packet", "Pod", "Command")
            // Message - String

            anchors.fill: parent
            model: PodModel{}
            delegate: podDelegate
            highlight: Rectangle {
                color: getColour(err);
                radius: 10 }
            highlightFollowsCurrentItem: true
            focus: true
        }

        Component {
            id: podDelegate
            Item {
                width: 845; height: 200
                Column {
                    Text { id: teamID; font.family: "Arial Narrow"; horizontalAlignment: Text.AlignHCenter; font.pointSize: 20;
                           text: '<b>Team ID:</b> ' + model.iD }
                    Text { id: commandTypeID; font.family: "Arial Narrow"; horizontalAlignment: Text.AlignHCenter; font.pointSize: 20;
                           text: '<b>Type:</b> ' + model.Type }
                    Text { id: messageID; font.family: "Arial Narrow"; horizontalAlignment: Text.AlignHCenter; font.pointSize: 19;
                           text: '<b>Pod State:</b> ' + model.Message }
                    Text { id: errorID; font.family: "Arial Narrow"; horizontalAlignment: Text.AlignHCenter; font.pointSize: 19;
                           text: '<b>Error:</b> ' + model.Errorbool }
                }
            }
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
                url: "wss://wloop2.localtunnel.me"
                onTextMessageReceived: {
                    messageBox.text = messageBox.text + "\nReceived message: " + message

                    // ID - team ID - uint8
                    // Error - Bool
                    // Type - String oneof("Packet", "Pod", "Command")
                    // Message - String

                    var JsonString = message;
                    var JsonObject = JSON.parse(JsonString);
//                    updateMessage(JsonObject.Message)
//                    updateType(JsonObject.Type);
//                    updateError(JsonObject.Error)
                    tid = parseInt(JSON.toString(JsonObject.ID));
                    err = JSON.toString((JsonObject.Error));
                    typ = JSON.toString(JsonObject.Type);
                    messag = JSON.toString(JsonObject.Message);

                    podView.currentIndex = tid - 1;
                    PodModel.setProperty(podView.currentIndex, "Errorbool", err);
                    PodModel.setProperty(podView.currentIndex, "Type", typ);
                    PodModel.setProperty(podView.currentIndex, "Message", messag);

//                   if(err == "True") {
//                       podView.highlightItem()
//                   }

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

import QtQuick 2.0

ListModel {
    id: podModel

    ListElement {
        iD: "1";
        Errorbool: "True";
        Type: "Pod";
        Message: "POD crashed boom";
    }
    ListElement {
        iD: "2";
        Errorbool: "False";
        Type: "Command";
        Message: "IDLE Ready";
    }
    ListElement {
        iD: "3";
        Errorbool: "False";
        Type: "Command";
        Message: "IDLE Ready";
    }
    ListElement {
        iD: "4";
        Errorbool: "True";
        Type: "Packet";
        Message: "Incorrect Packet Received";
    }
}

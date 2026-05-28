import QtQuick 2.15

Item {
    id: root
    property real percent: 0
    property color barColor: "#64b5f6"
    property color trackColor: "#eeeeee"

    implicitHeight: 8
    implicitWidth: 120

    Rectangle {
        anchors.fill: parent
        radius: 3
        color: root.trackColor
    }

    Rectangle {
        height: parent.height
        width: Math.max(2, parent.width * Math.min(1, percent))
        radius: 3
        color: root.barColor
    }
}

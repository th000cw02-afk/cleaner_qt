import QtQuick 2.15
import Analyzer 1.0

Rectangle {
    id: banner
    property string text: ""
    property bool isError: false
    property AnalyzerController controller

    implicitHeight: visible ? 40 : 0
    radius: controller && controller.themeManager ? controller.themeManager.radius : 8
    color: isError
          ? (controller ? Qt.rgba(0.9, 0.3, 0.3, 0.15) : "#ffebee")
          : (controller ? Qt.rgba(0.3, 0.7, 0.4, 0.15) : "#e8f5e9")
    border.color: isError
                ? (controller ? controller.themeManager.errorColor : "#ef9a9a")
                : (controller ? controller.themeManager.successColor : "#a5d6a7")
    border.width: 1

    Text {
        anchors.fill: parent
        anchors.margins: 10
        text: banner.text
        color: isError
              ? (controller ? controller.themeManager.errorColor : "#c62828")
              : (controller ? controller.themeManager.successColor : "#2e7d32")
        font.pixelSize: 13
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}

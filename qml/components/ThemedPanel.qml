import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15
import Analyzer 1.0

Rectangle {
    id: root
    property AnalyzerController controller
    property alias content: contentLayout.data
    default property alias children: contentLayout.data

    color: controller && controller.themeManager ? controller.themeManager.panelBackground : "#ffffff"
    radius: controller && controller.themeManager ? controller.themeManager.radius : 8
    border.color: controller && controller.themeManager ? controller.themeManager.borderColor : "#e0e0e0"
    border.width: 1

    ColumnLayout {
        id: contentLayout
        anchors.fill: parent
        anchors.margins: 12
        spacing: controller && controller.themeManager ? controller.themeManager.spacing : 8
    }
}

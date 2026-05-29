import QtQuick 2.15
import QtQuick.Controls 2.15
import Analyzer 1.0

Rectangle {
    id: root
    property AnalyzerController controller
    property bool selected: false
    property bool hovered: false
    property int rowIndex: 0
    default property alias content: rowLayout.data

    implicitHeight: controller && controller.themeManager ? controller.themeManager.rowHeight : 40
    color: selected
           ? (controller ? controller.themeManager.selectionBackground : "#e3f2fd")
           : (hovered
              ? (controller ? controller.themeManager.hoverBackground : "#f5f5f5")
              : (rowIndex % 2
                 ? (controller ? controller.themeManager.zebraBackground : "#fafafa")
                 : (controller ? controller.themeManager.panelBackground : "#ffffff")))

    RowLayout {
        id: rowLayout
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        spacing: 8
    }
}

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Analyzer 1.0

ProgressBar {
    property AnalyzerController controller

    Material.accent: controller && controller.themeManager ? controller.themeManager.accentColor : "#1976d2"
    background: Rectangle {
        implicitHeight: 6
        radius: 3
        color: controller && controller.themeManager ? controller.themeManager.trackBackground : "#eeeeee"
    }
    contentItem: Item {
        implicitHeight: 6
        Rectangle {
            width: parent.width * control.visualPosition
            height: parent.height
            radius: 3
            color: controller && controller.themeManager ? controller.themeManager.accentColor : "#1976d2"
        }
    }
}

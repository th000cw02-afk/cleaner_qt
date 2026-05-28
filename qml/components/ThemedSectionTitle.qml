import QtQuick 2.15
import Analyzer 1.0

Text {
    property AnalyzerController controller
    property string title: ""
    property string subtitle: ""

    font.pixelSize: controller && controller.themeManager ? controller.themeManager.titleSize : 15
    font.bold: true
    color: controller && controller.themeManager ? controller.themeManager.textColor : "#212121"
    text: subtitle.length > 0 ? title + "  ·  " + subtitle : title
}

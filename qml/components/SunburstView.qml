import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Analyzer 1.0

ThemedPanel {
    id: root
    property AnalyzerController controller

    ColumnLayout {
        anchors.fill: parent
        ThemedSectionTitle {
            controller: root.controller
            title: qsTr("Sunburst")
            subtitle: qsTr("请使用 Treemap 进行空间分析")
        }
        Text {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignCenter
            horizontalAlignment: Text.AlignHCenter
            text: qsTr("Sunburst 视图将在后续版本提供")
            color: root.controller ? root.controller.themeManager.secondaryTextColor : "#888"
            wrapMode: Text.WordWrap
        }
    }
}

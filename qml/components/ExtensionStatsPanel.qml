import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Analyzer 1.0

ThemedPanel {
    id: root
    property AnalyzerController controller

    ColumnLayout {
        anchors.fill: parent
        spacing: root.controller ? root.controller.themeManager.spacing : 8

        ThemedSectionTitle {
            controller: root.controller
            title: qsTr("扩展名统计")
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.controller ? root.controller.extensionStats : []

            delegate: Rectangle {
                width: ListView.view.width
                height: root.controller.themeManager.rowHeight
                color: mouseArea.containsMouse
                       ? root.controller.themeManager.hoverBackground
                       : root.controller.themeManager.panelBackground

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    Text {
                        Layout.preferredWidth: 80
                        text: modelData.extension
                        font.bold: true
                        color: root.controller.themeManager.textColor
                    }
                    SizeBarDelegate {
                        Layout.fillWidth: true
                        percent: (modelData.totalSize || 0) / Math.max(root.controller.totalSize, 1)
                        barColor: modelData.color || root.controller.themeManager.accentColor
                        trackColor: root.controller.themeManager.trackBackground
                    }
                    Text {
                        text: root.controller.formatFileSize(modelData.totalSize)
                        font.pixelSize: 12
                        color: root.controller.themeManager.textColor
                    }
                    Text {
                        text: modelData.fileCount
                        font.pixelSize: 12
                        color: root.controller.themeManager.secondaryTextColor
                    }
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if (!root.controller) return
                        root.controller.extensionFilter = modelData.extension
                        root.controller.searchFilter = modelData.extension
                    }
                }
            }
        }
    }
}

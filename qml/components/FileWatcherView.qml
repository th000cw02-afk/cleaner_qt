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
            title: qsTr("目录监控")
        }

        RowLayout {
            Layout.fillWidth: true
            TextField {
                id: watchPath
                Layout.fillWidth: true
                text: root.controller ? root.controller.scanPath : ""
                placeholderText: qsTr("监控路径")
            }
            Button {
                text: root.controller && root.controller.fileWatcher.watching ? qsTr("停止") : qsTr("监控")
                enabled: root.controller
                onClicked: {
                    if (!root.controller) return
                    if (root.controller.fileWatcher.watching)
                        root.controller.fileWatcher.stopWatch()
                    else
                        root.controller.fileWatcher.startWatch(watchPath.text)
                }
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.controller ? root.controller.fileWatcher.log : []

            delegate: Text {
                width: ListView.view.width
                text: modelData
                font.pixelSize: 11
                color: root.controller.themeManager.secondaryTextColor
                wrapMode: Text.Wrap
            }
        }
    }
}

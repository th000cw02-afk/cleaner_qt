import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Analyzer 1.0

Rectangle {
    id: root
    property AnalyzerController controller

    color: "#ffffff"
    radius: 8
    border.color: "#e0e0e0"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            Text {
                text: qsTr("清理扫描结果")
                font.pixelSize: 15
                font.bold: true
            }
            Item { Layout.fillWidth: true }
            Button {
                text: qsTr("空目录")
                enabled: controller && !controller.isScanning
                onClicked: controller.scanEmptyDirectories()
            }
            Button {
                text: qsTr("重复候选")
                enabled: controller && !controller.isScanning
                onClicked: controller.scanDuplicateCandidates()
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: controller ? controller.cleanupResults : []

            delegate: Rectangle {
                width: ListView.view.width
                height: content.height + 16
                color: "#fafafa"
                border.color: "#eee"

                ColumnLayout {
                    id: content
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    Text {
                        text: modelData.type === "duplicate_group"
                              ? qsTr("重复组 (%1 字节)").arg(modelData.size)
                              : modelData.path
                        font.pixelSize: 12
                        wrapMode: Text.WordWrap
                        Layout.fillWidth: true
                    }

                    Repeater {
                        model: modelData.paths || []
                        delegate: Text {
                            text: "  • " + modelData
                            font.pixelSize: 11
                            color: "#555"
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
    }
}

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

        RowLayout {
            Layout.fillWidth: true
            ThemedSectionTitle {
                controller: root.controller
                title: qsTr("重复文件组")
            }
            Item { Layout.fillWidth: true }
            Button {
                text: qsTr("扫描重复")
                enabled: root.controller
                onClicked: {
                    if (root.controller) root.controller.scanDuplicateCandidates()
                }
            }
            Button {
                text: qsTr("删除选中组")
                enabled: root.controller && dupList.currentIndex >= 0
                onClicked: {
                    if (!root.controller) return
                    var item = root.controller.duplicateGroups[dupList.currentIndex]
                    if (item && item.paths) {
                        var paths = []
                        for (var i = 1; i < item.paths.length; i++) {
                            paths.push(item.paths[i])
                        }
                        root.controller.deleteSelectedFiles(paths)
                    }
                }
            }
        }

        ListView {
            id: dupList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.controller ? root.controller.duplicateGroups : []

            delegate: Rectangle {
                width: dupList.width
                height: 72
                color: dupList.currentIndex === index
                       ? root.controller.themeManager.selectionBackground
                       : root.controller.themeManager.panelBackground

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    Text {
                        text: root.controller
                              ? qsTr("大小: ") + root.controller.formatFileSize(modelData.size || 0)
                                + " · " + (modelData.paths ? modelData.paths.length : 0) + qsTr(" 个文件")
                              : ""
                        font.bold: true
                        color: root.controller.themeManager.textColor
                    }
                    Text {
                        Layout.fillWidth: true
                        text: modelData.paths ? modelData.paths.join("\n") : ""
                        font.pixelSize: 11
                        color: root.controller.themeManager.secondaryTextColor
                        wrapMode: Text.Wrap
                        maximumLineCount: 2
                        elide: Text.ElideRight
                    }
                }
                TapHandler {
                    onTapped: dupList.currentIndex = index
                }
            }
        }
    }
}

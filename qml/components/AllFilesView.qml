import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Analyzer 1.0
import "qrc:/components"

ThemedPanel {
    id: root
    property AnalyzerController controller

    ColumnLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: root.controller && root.controller.themeManager ? root.controller.themeManager.spacing : 8

        RowLayout {
            Layout.fillWidth: true
            ThemedSectionTitle {
                controller: root.controller
                title: qsTr("全部文件")
                subtitle: root.controller ? String(root.controller.fileIndexModel.filteredRowCount) : "0"
            }
            Item { Layout.fillWidth: true }
            CheckBox {
                text: qsTr("已分配")
                enabled: root.controller
                checked: root.controller ? root.controller.useAllocatedSize : false
                onToggled: if (root.controller) root.controller.useAllocatedSize = checked
            }
            CheckBox {
                text: qsTr("正则")
                enabled: root.controller
                checked: root.controller ? root.controller.regexSearch : false
                onToggled: if (root.controller) root.controller.regexSearch = checked
            }
            Button {
                text: qsTr("删除选中")
                enabled: root.controller && fileList.selectedPaths.length > 0
                onClicked: {
                    if (root.controller)
                        root.controller.deleteSelectedFiles(fileList.selectedPaths)
                }
            }
        }

        Label {
            Layout.fillWidth: true
            visible: root.controller && root.controller.isScanning
            text: qsTr("扫描进行中，完整文件列表将在扫描完成后刷新")
            color: root.controller ? root.controller.themeManager.secondaryTextColor : "#666"
            font.pixelSize: 12
            wrapMode: Text.WordWrap
        }

        ListView {
            id: fileList
            property var selectedPaths: []
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            interactive: root.controller && !root.controller.isScanning
            model: root.controller ? root.controller.fileIndexModel : null

            delegate: Rectangle {
                width: fileList.width
                height: root.controller ? root.controller.themeManager.rowHeight : 40

                required property int index
                required property string path
                required property var displaySize
                required property var logicalSize
                required property string extension

                property bool rowSelected: fileList.selectedPaths.indexOf(path) >= 0
                color: rowSelected
                       ? root.controller.themeManager.selectionBackground
                       : (index % 2 ? root.controller.themeManager.zebraBackground
                                  : root.controller.themeManager.panelBackground)

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    Text {
                        Layout.fillWidth: true
                        text: path
                        elide: Text.ElideMiddle
                        font.pixelSize: 12
                        color: root.controller.themeManager.textColor
                    }
                    Text {
                        text: root.controller.formatFileSize(displaySize || logicalSize || 0)
                        font.pixelSize: 12
                        color: root.controller.themeManager.textColor
                    }
                    Text {
                        text: extension
                        font.pixelSize: 11
                        color: root.controller.themeManager.secondaryTextColor
                    }
                }

                TapHandler {
                    onTapped: function(eventPoint) {
                        if (!root.controller || !path) return
                        var paths = fileList.selectedPaths.slice()
                        var idx = paths.indexOf(path)
                        if (eventPoint.modifiers & Qt.ControlModifier) {
                            if (idx >= 0) paths.splice(idx, 1)
                            else paths.push(path)
                        } else {
                            paths = [path]
                        }
                        fileList.selectedPaths = paths
                    }
                    onDoubleTapped: {
                        if (root.controller && path)
                            root.controller.revealInExplorer(path)
                    }
                }
            }
        }
    }
}

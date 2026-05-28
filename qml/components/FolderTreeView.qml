import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Analyzer 1.0

ThemedPanel {
    id: root
    property AnalyzerController controller
    property string contextPath: ""

    ColumnLayout {
        anchors.fill: parent
        spacing: root.controller ? root.controller.themeManager.spacing : 8

        ThemedSectionTitle {
            controller: root.controller
            title: qsTr("目录树")
        }

        Label {
            Layout.fillWidth: true
            visible: root.controller && root.controller.isScanning
            text: qsTr("扫描进行中，目录树将在完成后刷新")
            color: root.controller.themeManager.secondaryTextColor
            font.pixelSize: 12
        }

        TreeView {
            id: tree
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            interactive: root.controller && !root.controller.isScanning
            model: root.controller ? root.controller.folderTreeModel : null

            delegate: TreeViewDelegate {
                id: del
                implicitHeight: root.controller ? root.controller.themeManager.rowHeight : 40
                indentation: 16

                contentItem: RowLayout {
                    spacing: 8
                    width: del.width

                    Text {
                        Layout.preferredWidth: Math.min(160, del.width * 0.35)
                        text: model.name ?? ""
                        elide: Text.ElideRight
                        font.pixelSize: 13
                        color: root.controller.themeManager.textColor
                    }

                    SizeBarDelegate {
                        Layout.fillWidth: true
                        percent: model.sizePercent ?? 0
                        barColor: root.controller.themeManager.accentColor
                        trackColor: root.controller.themeManager.trackBackground
                    }

                    Text {
                        Layout.preferredWidth: 72
                        horizontalAlignment: Text.AlignRight
                        text: root.controller ? root.controller.formatFileSize(model.size ?? 0) : ""
                        font.pixelSize: 12
                        color: root.controller.themeManager.secondaryTextColor
                    }
                }

                TapHandler {
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onSingleTapped: function(eventPoint) {
                        if (!root.controller || !model.path) return
                        if (eventPoint.button === Qt.RightButton) {
                            root.contextPath = model.path
                            treeMenu.popup()
                            return
                        }
                        root.controller.selectFolderInTree(model.path)
                        root.controller.treemapFocusPath = model.path
                    }
                    onDoubleTapped: {
                        if (root.controller && model.path)
                            root.controller.drillTreemap(model.path)
                    }
                }
            }
        }
    }

    Menu {
        id: treeMenu
        parent: Overlay.overlay
        MenuItem {
            text: qsTr("在资源管理器中显示")
            enabled: root.controller
            onTriggered: {
                if (root.controller) root.controller.revealInExplorer(root.contextPath)
            }
        }
        MenuItem {
            text: qsTr("复制路径")
            enabled: root.controller
            onTriggered: {
                if (root.controller) root.controller.copyPathToClipboard(root.contextPath)
            }
        }
        MenuItem {
            text: qsTr("钻取 Treemap")
            enabled: root.controller
            onTriggered: {
                if (root.controller) root.controller.drillTreemap(root.contextPath)
            }
        }
    }
}

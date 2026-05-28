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
            title: qsTr("Top 大文件")
        }

        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.controller ? root.controller.filteredTopFiles : []

            delegate: Rectangle {
                width: list.width
                height: root.controller.themeManager.rowHeight
                color: mouseArea.containsMouse
                       ? root.controller.themeManager.hoverBackground
                       : (index % 2 ? root.controller.themeManager.zebraBackground
                                  : root.controller.themeManager.panelBackground)

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    Text {
                        Layout.fillWidth: true
                        text: modelData.path
                        elide: Text.ElideMiddle
                        font.pixelSize: 12
                        color: root.controller.themeManager.textColor
                    }
                    Text {
                        text: root.controller ? root.controller.formatFileSize(modelData.size) : ""
                        font.pixelSize: 12
                        color: root.controller.themeManager.secondaryTextColor
                    }
                }

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: function(mouse) {
                        if (mouse.button === Qt.RightButton) {
                            fileMenu.popup()
                        } else if (root.controller) {
                            root.controller.revealInExplorer(modelData.path)
                        }
                    }
                }

                Menu {
                    id: fileMenu
                    parent: Overlay.overlay
                    MenuItem {
                        text: qsTr("复制路径")
                        enabled: root.controller
                        onTriggered: {
                            if (root.controller) root.controller.copyPathToClipboard(modelData.path)
                        }
                    }
                    MenuItem {
                        text: qsTr("在资源管理器中显示")
                        enabled: root.controller
                        onTriggered: {
                            if (root.controller) root.controller.revealInExplorer(modelData.path)
                        }
                    }
                }
            }
        }
    }
}

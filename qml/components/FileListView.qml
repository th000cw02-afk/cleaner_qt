import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Analyzer 1.0

ThemedPanel {
    id: listRect
    property AnalyzerController controller
    property bool useSelectedFolder: false

    ColumnLayout {
        anchors.fill: parent
        spacing: listRect.controller ? listRect.controller.themeManager.spacing : 8

        RowLayout {
            Layout.fillWidth: true
            ThemedSectionTitle {
                controller: listRect.controller
                title: useSelectedFolder ? qsTr("当前目录文件") : qsTr("文件列表")
                subtitle: qsTr("最多 500 项")
            }
            Item { Layout.fillWidth: true }
            Button {
                text: qsTr("移至回收站")
                enabled: fileList.selectedFiles.length > 0 && listRect.controller && !listRect.controller.isScanning
                onClicked: confirmDelete.open()
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ListView {
                id: fileList
                property var selectedFiles: []
                model: useSelectedFolder && listRect.controller
                       ? listRect.controller.selectedFolderFiles
                       : []

                delegate: Rectangle {
                    width: fileList.width
                    height: listRect.controller.themeManager.rowHeight
                    property bool selected: fileList.selectedFiles.indexOf(modelData.path) !== -1
                    color: selected ? listRect.controller.themeManager.selectionBackground
                                    : (mouseArea.containsMouse ? listRect.controller.themeManager.hoverBackground
                                                               : listRect.controller.themeManager.panelBackground)

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 10
                        CheckBox {
                            checked: parent.parent.selected
                            onToggled: {
                                var path = modelData.path
                                var idx = fileList.selectedFiles.indexOf(path)
                                if (checked && idx === -1) fileList.selectedFiles.push(path)
                                else if (!checked && idx !== -1) fileList.selectedFiles.splice(idx, 1)
                                fileList.selectedFiles = fileList.selectedFiles.slice()
                            }
                        }
                        Text {
                            Layout.fillWidth: true
                            text: modelData.path
                            elide: Text.ElideMiddle
                            font.pixelSize: 12
                            color: listRect.controller.themeManager.textColor
                        }
                        Text {
                            text: listRect.controller ? listRect.controller.formatFileSize(modelData.size) : ""
                            font.pixelSize: 12
                            color: listRect.controller.themeManager.secondaryTextColor
                        }
                    }

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: parent.children[0].children[0].toggle()
                    }
                }
            }
        }
    }

    Dialog {
        id: confirmDelete
        title: qsTr("确认删除")
        modal: true
        standardButtons: Dialog.Yes | Dialog.No
        Label {
            text: qsTr("将选中的 %1 个文件移至回收站？").arg(fileList.selectedFiles.length)
        }
        onAccepted: {
            if (listRect.controller) {
                listRect.controller.deleteSelectedFiles(fileList.selectedFiles)
                fileList.selectedFiles = []
            }
        }
    }

    Connections {
        target: listRect.controller
        function onSelectedFolderFilesChanged() {
            fileList.selectedFiles = []
        }
    }
}

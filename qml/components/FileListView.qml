import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: listRect
    property CleanerController controller
    
    color: "#ffffff"
    radius: 8
    border.color: "#e0e0e0"
    border.width: 1
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12
        
        RowLayout {
            Layout.fillWidth: true
            
            Text {
                text: qsTr("文件列表")
                font.pixelSize: 16
                font.bold: true
                color: "#333333"
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                id: deleteButton
                text: qsTr("删除选中")
                Layout.preferredWidth: 100
                Layout.preferredHeight: 36
                enabled: fileList.selectedFiles.length > 0 && !controller.isScanning
                
                background: Rectangle {
                    color: deleteButton.enabled ? (deleteButton.pressed ? "#d32f2f" : (deleteButton.hovered ? "#f44336" : "#f44336")) : "#cccccc"
                    radius: 6
                }
                
                contentItem: Text {
                    text: deleteButton.text
                    color: "white"
                    font.pixelSize: 13
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: {
                    if (fileList.selectedFiles.length > 0) {
                        controller.deleteSelectedFiles(fileList.selectedFiles)
                        fileList.selectedFiles = []
                    }
                }
            }
        }
        
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            
            ListView {
                id: fileList
                property var selectedFiles: []
                
                model: controller ? controller.getFileList() : []
                
                delegate: Rectangle {
                    width: fileList.width
                    height: 50
                    color: mouseArea.containsMouse ? "#f5f5f5" : (selected ? "#e3f2fd" : "#ffffff")
                    property bool selected: fileList.selectedFiles.indexOf(modelData.path) !== -1
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 12
                        
                        CheckBox {
                            checked: parent.parent.selected
                            onToggled: {
                                if (checked) {
                                    if (fileList.selectedFiles.indexOf(modelData.path) === -1) {
                                        fileList.selectedFiles.push(modelData.path)
                                    }
                                } else {
                                    var index = fileList.selectedFiles.indexOf(modelData.path)
                                    if (index !== -1) {
                                        fileList.selectedFiles.splice(index, 1)
                                    }
                                }
                            }
                        }
                        
                        Text {
                            Layout.fillWidth: true
                            text: modelData.path
                            font.pixelSize: 13
                            color: "#333333"
                            elide: Text.ElideMiddle
                        }
                        
                        Text {
                            Layout.preferredWidth: 100
                            text: controller ? controller.formatFileSize(modelData.size) : ""
                            font.pixelSize: 13
                            color: "#666666"
                            horizontalAlignment: Text.AlignRight
                        }
                        
                        Text {
                            Layout.preferredWidth: 150
                            text: modelData.lastModified
                            font.pixelSize: 12
                            color: "#999999"
                            horizontalAlignment: Text.AlignRight
                        }
                    }
                    
                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            var checkbox = parent.children[0].children[0]
                            checkbox.toggle()
                        }
                    }
                    
                    Rectangle {
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 1
                        color: "#e0e0e0"
                    }
                }
                
                header: Rectangle {
                    width: fileList.width
                    height: 40
                    color: "#f5f5f5"
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 12
                        
                        CheckBox {
                            id: selectAllCheckBox
                            onToggled: {
                                if (checked) {
                                    fileList.selectedFiles = []
                                    for (var i = 0; i < fileList.model.length; i++) {
                                        fileList.selectedFiles.push(fileList.model[i].path)
                                    }
                                } else {
                                    fileList.selectedFiles = []
                                }
                            }
                        }
                        
                        Text {
                            Layout.fillWidth: true
                            text: qsTr("文件路径")
                            font.pixelSize: 13
                            font.bold: true
                            color: "#333333"
                        }
                        
                        Text {
                            Layout.preferredWidth: 100
                            text: qsTr("大小")
                            font.pixelSize: 13
                            font.bold: true
                            color: "#333333"
                            horizontalAlignment: Text.AlignRight
                        }
                        
                        Text {
                            Layout.preferredWidth: 150
                            text: qsTr("修改时间")
                            font.pixelSize: 13
                            font.bold: true
                            color: "#333333"
                            horizontalAlignment: Text.AlignRight
                        }
                    }
                }
            }
        }
        
        Connections {
            target: controller
            function onFileListUpdated() {
                fileList.model = controller.getFileList()
            }
        }
    }
}


import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Cleaner 1.0

ApplicationWindow {
    id: window
    width: 1200
    height: 800
    visible: true
    title: qsTr("磁盘清理工具")
    
    property CleanerController controller: CleanerController {
        id: cleanerController
    }
    
    // 主布局
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16
        
        // 顶部工具栏
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            color: "#f5f5f5"
            radius: 8
            border.color: "#e0e0e0"
            border.width: 1
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 16
                
                Text {
                    text: qsTr("选择磁盘:")
                    font.pixelSize: 14
                    color: "#333333"
                }
                
                DiskSelector {
                    id: diskSelector
                    Layout.preferredWidth: 200
                    controller: cleanerController
                }
                
                Item { Layout.fillWidth: true }
                
                Button {
                    id: scanButton
                    text: cleanerController.isScanning ? qsTr("停止扫描") : qsTr("开始扫描")
                    Layout.preferredWidth: 120
                    Layout.preferredHeight: 40
                    
                    background: Rectangle {
                        color: scanButton.pressed ? "#1976d2" : (scanButton.hovered ? "#2196f3" : "#2196f3")
                        radius: 6
                    }
                    
                    contentItem: Text {
                        text: scanButton.text
                        color: "white"
                        font.pixelSize: 14
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    onClicked: {
                        if (cleanerController.isScanning) {
                            cleanerController.stopScan()
                        } else {
                            cleanerController.startScan()
                        }
                    }
                }
            }
        }
        
        // 进度显示区域
        ScanProgress {
            id: scanProgress
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            controller: cleanerController
        }
        
        // 文件列表区域
        FileListView {
            id: fileListView
            Layout.fillWidth: true
            Layout.fillHeight: true
            controller: cleanerController
        }
    }
}


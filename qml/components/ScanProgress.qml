import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Cleaner 1.0

Rectangle {
    id: progressRect
    property CleanerController controller
    
    color: "#ffffff"
    radius: 8
    border.color: "#e0e0e0"
    border.width: 1
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12
        
        Text {
            text: qsTr("扫描进度")
            font.pixelSize: 16
            font.bold: true
            color: "#333333"
        }
        
        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            Layout.preferredHeight: 8
            from: 0
            to: 100
            value: controller && controller.isScanning ? 50 : 0
            
            background: Rectangle {
                color: "#e0e0e0"
                radius: 4
            }
            
            contentItem: Item {
                implicitWidth: 200
                implicitHeight: 8
                
                Rectangle {
                    width: progressBar.visualPosition * parent.width
                    height: parent.height
                    radius: 4
                    color: "#2196f3"
                }
            }
        }
        
        GridLayout {
            Layout.fillWidth: true
            columns: 4
            rowSpacing: 8
            columnSpacing: 16
            
            Text {
                text: qsTr("状态:")
                font.pixelSize: 13
                color: "#666666"
            }
            
            Text {
                text: controller && controller.isScanning ? qsTr("扫描中...") : qsTr("就绪")
                font.pixelSize: 13
                color: controller && controller.isScanning ? "#2196f3" : "#666666"
                font.bold: controller && controller.isScanning
            }
            
            Text {
                text: qsTr("已扫描目录:")
                font.pixelSize: 13
                color: "#666666"
            }
            
            Text {
                text: controller ? controller.directoriesScanned : 0
                font.pixelSize: 13
                color: "#333333"
                font.bold: true
            }
            
            Text {
                text: qsTr("文件总数:")
                font.pixelSize: 13
                color: "#666666"
            }
            
            Text {
                text: controller ? controller.totalFiles : 0
                font.pixelSize: 13
                color: "#333333"
                font.bold: true
            }
            
            Text {
                text: qsTr("总大小:")
                font.pixelSize: 13
                color: "#666666"
            }
            
            Text {
                text: controller ? controller.formatFileSize(controller.totalSize) : "0 B"
                font.pixelSize: 13
                color: "#333333"
                font.bold: true
            }
        }
    }
}


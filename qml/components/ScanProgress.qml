import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Analyzer 1.0

ThemedPanel {
    id: root
    property AnalyzerController controller

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        ThemedSectionTitle {
            controller: root.controller
            title: qsTr("扫描进度")
        }

        ThemedProgressBar {
            Layout.fillWidth: true
            controller: root.controller
            indeterminate: root.controller && root.controller.scanProgressIndeterminate
            from: 0
            to: 100
            value: root.controller ? root.controller.scanProgress : 0
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 4
            columnSpacing: 16
            rowSpacing: 8

            Label {
                text: qsTr("状态")
                color: root.controller.themeManager.secondaryTextColor
            }
            Label {
                text: root.controller && root.controller.isScanning ? qsTr("扫描中…") : qsTr("就绪")
                font.bold: root.controller && root.controller.isScanning
                color: root.controller.themeManager.accentColor
            }
            Label {
                text: qsTr("目录数")
                color: root.controller.themeManager.secondaryTextColor
            }
            Label {
                text: root.controller ? root.controller.directoriesScanned : 0
                font.bold: true
                color: root.controller.themeManager.textColor
            }

            Label {
                text: qsTr("文件数")
                color: root.controller.themeManager.secondaryTextColor
            }
            Label {
                text: root.controller ? root.controller.totalFiles : 0
                font.bold: true
                color: root.controller.themeManager.textColor
            }
            Label {
                text: qsTr("总大小")
                color: root.controller.themeManager.secondaryTextColor
            }
            Label {
                text: root.controller ? root.controller.formatFileSize(root.controller.totalSize) : "0 B"
                font.bold: true
                color: root.controller.themeManager.textColor
            }

            Label {
                text: qsTr("耗时")
                color: root.controller.themeManager.secondaryTextColor
            }
            Label {
                text: root.controller && root.controller.scanElapsedMs > 0
                      ? (root.controller.scanElapsedMs / 1000).toFixed(1) + " s"
                      : "-"
                font.bold: true
                color: root.controller.themeManager.textColor
            }
            Label {
                text: qsTr("速率")
                color: root.controller.themeManager.secondaryTextColor
            }
            Label {
                text: root.controller && root.controller.scanRateBytesPerSec > 0
                      ? root.controller.formatFileSize(root.controller.scanRateBytesPerSec) + "/s"
                      : "-"
                font.bold: true
                color: root.controller.themeManager.textColor
            }
        }
    }
}

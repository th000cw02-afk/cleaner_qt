import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Analyzer 1.0

Dialog {
    id: root
    property AnalyzerController controller
    title: qsTr("设置")
    modal: true
    standardButtons: Dialog.Save | Dialog.Cancel
    width: 480

  onAccepted: {
        if (!controller) return
        AppSettings.excludeDirNames = excludeField.text.split("\n").filter(function(s) { return s.length > 0 })
        AppSettings.minFileSize = parseInt(minSizeField.text) || 0
        AppSettings.stopAtMountPoints = mountCheck.checked
        AppSettings.preferMftScan = mftCheck.checked
        AppSettings.skipReparseAndCloud = reparseCheck.checked
        AppSettings.theme = themeBox.currentIndex === 1 ? "dark" : "light"
        AppSettings.portableMode = portableCheck.checked
        AppSettings.customCleanupCommand = customCmdField.text
        AppSettings.checkUpdates = updateCheck.checked
        controller.themeManager.theme = AppSettings.theme
        controller.minFileSizeFilter = AppSettings.minFileSize
        controller.saveSettings()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        Label { text: qsTr("排除目录（每行一个）") }
        TextArea {
            id: excludeField
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            text: AppSettings.excludeDirNames.join("\n")
        }

        RowLayout {
            Label { text: qsTr("最小文件大小 (字节)") }
            TextField {
                id: minSizeField
                text: AppSettings.minFileSize.toString()
            }
        }

        CheckBox {
            id: mountCheck
            text: qsTr("在挂载点停止")
            checked: AppSettings.stopAtMountPoints
        }
        CheckBox {
            id: mftCheck
            text: qsTr("优先 MFT 扫描")
            checked: AppSettings.preferMftScan
        }
        CheckBox {
            id: reparseCheck
            text: qsTr("跳过重解析点/云占位文件")
            checked: AppSettings.skipReparseAndCloud
        }
        CheckBox {
            id: portableCheck
            text: qsTr("便携模式 (CleanerQt.ini)")
            checked: AppSettings.portableMode
        }
        CheckBox {
            id: updateCheck
            text: qsTr("启动时检查更新")
            checked: AppSettings.checkUpdates
        }

        RowLayout {
            Label { text: qsTr("主题") }
            ComboBox {
                id: themeBox
                model: [qsTr("浅色"), qsTr("深色")]
                currentIndex: AppSettings.theme === "dark" ? 1 : 0
            }
        }

        Label { text: qsTr("自定义清理命令 (%PATH%)") }
        TextField {
            id: customCmdField
            Layout.fillWidth: true
            text: AppSettings.customCleanupCommand
        }
    }
}

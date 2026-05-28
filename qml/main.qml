import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15
import Analyzer 1.0
import "qrc:/components"

ApplicationWindow {
    id: window
    width: 1400
    height: 920
    visible: true
    title: qsTr("磁盘空间分析 v2.0")

    AnalyzerController {
        id: analyzer
    }

    Material.theme: analyzer.themeManager.isDark ? Material.Dark : Material.Light
    Material.accent: analyzer.themeManager.accentColor
    Material.background: analyzer.themeManager.windowBackground
    Material.foreground: analyzer.themeManager.textColor
    Material.primary: analyzer.themeManager.accentColor

    color: analyzer.themeManager.windowBackground

    SettingsDialog {
        id: settingsDialog
        controller: analyzer
    }

    Shortcut { sequence: "F5"; onActivated: analyzer.startScan() }
    Shortcut { sequence: "Ctrl+S"; onActivated: analyzer.saveSnapshot("") }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: analyzer.themeManager.spacing * 2
        spacing: analyzer.themeManager.spacing + 4

        ToolBar {
            Layout.fillWidth: true
            Material.elevation: 0
            background: Rectangle {
                radius: analyzer.themeManager.radius
                color: analyzer.themeManager.panelBackground
                border.color: analyzer.themeManager.borderColor
                border.width: 1
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: analyzer.themeManager.spacing
                spacing: analyzer.themeManager.spacing

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    PathSelector {
                        Layout.fillWidth: true
                        controller: analyzer
                    }

                    Button {
                        text: analyzer.isScanning ? qsTr("停止") : qsTr("扫描")
                        highlighted: true
                        onClicked: analyzer.isScanning ? analyzer.stopScan() : analyzer.startScan()
                    }

                    Button { text: qsTr("设置"); onClicked: settingsDialog.open() }

                    Button {
                        text: qsTr("打开快照")
                        onClicked: analyzer.loadSnapshot("")
                    }
                    Button {
                        text: qsTr("管理员")
                        enabled: !analyzer.isAdmin
                        onClicked: analyzer.restartAsAdmin()
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    SearchField {
                        Layout.fillWidth: true
                        controller: analyzer
                    }

                    CheckBox {
                        text: qsTr("正则")
                        checked: analyzer.regexSearch
                        onToggled: analyzer.regexSearch = checked
                    }

                    CheckBox {
                        text: qsTr("已分配")
                        checked: analyzer.useAllocatedSize
                        onToggled: analyzer.useAllocatedSize = checked
                    }

                    Label {
                        text: analyzer.isAdmin ? qsTr("管理员") : qsTr("标准用户")
                        color: analyzer.isAdmin
                              ? analyzer.themeManager.successColor
                              : analyzer.themeManager.warningColor
                        font.bold: true
                    }
                }
            }
        }

        MessageBanner {
            Layout.fillWidth: true
            visible: analyzer.lastError.length > 0
            text: analyzer.lastError
            isError: true
            controller: analyzer
        }

        MessageBanner {
            Layout.fillWidth: true
            visible: analyzer.statusMessage.length > 0 && analyzer.lastError.length === 0
            text: analyzer.statusMessage
            isError: false
            controller: analyzer
        }

        ScanProgress {
            Layout.fillWidth: true
            Layout.preferredHeight: 130
            controller: analyzer
        }

        TabBar {
            id: viewTabs
            Layout.fillWidth: true
            TabButton { text: qsTr("树 + Treemap") }
            TabButton { text: qsTr("全部文件") }
            TabButton { text: qsTr("重复") }
            TabButton { text: qsTr("清理") }
            TabButton { text: qsTr("监控") }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: viewTabs.currentIndex

            SplitView {
                orientation: Qt.Horizontal

                FolderTreeView {
                    SplitView.preferredWidth: 380
                    SplitView.minimumWidth: 260
                    controller: analyzer
                }

                ColumnLayout {
                    SplitView.fillWidth: true
                    spacing: analyzer.themeManager.spacing

                    TreemapView {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 240
                        controller: analyzer
                    }

                    TabBar {
                        id: detailTabs
                        Layout.fillWidth: true
                        TabButton { text: qsTr("Top 文件") }
                        TabButton { text: qsTr("当前目录") }
                        TabButton { text: qsTr("扩展名") }
                        TabButton { text: qsTr("Sunburst") }
                    }

                    StackLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        currentIndex: detailTabs.currentIndex

                        TopFilesPanel { controller: analyzer }
                        FileListView { controller: analyzer; useSelectedFolder: true }
                        ExtensionStatsPanel { controller: analyzer }
                        SunburstView { controller: analyzer }
                    }
                }
            }

            AllFilesView { controller: analyzer }
            DuplicateView { controller: analyzer }
            CleanupHub { controller: analyzer }
            FileWatcherView { controller: analyzer }
        }
    }

    Connections {
        target: analyzer
        function onScanError(message) {
            errorDialog.text = message
            errorDialog.open()
        }
    }

    Dialog {
        id: errorDialog
        property string text: ""
        anchors.centerIn: parent
        title: qsTr("错误")
        modal: true
        standardButtons: Dialog.Ok
        Label {
            width: 400
            wrapMode: Text.WordWrap
            text: errorDialog.text
        }
    }
}

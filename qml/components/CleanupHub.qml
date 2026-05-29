import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Analyzer 1.0

ThemedPanel {
    id: root
    property AnalyzerController controller

    RowLayout {
        anchors.fill: parent
        spacing: root.controller ? root.controller.themeManager.spacing : 8

        ListView {
            id: moduleList
            Layout.preferredWidth: 180
            Layout.fillHeight: true
            clip: true
            model: root.controller ? root.controller.cleanupModuleIds : []

            delegate: ItemDelegate {
                width: moduleList.width
                property string moduleId: modelData
                text: moduleId
                highlighted: moduleList.currentIndex === index
                onClicked: {
                    if (!root.controller) return
                    moduleList.currentIndex = index
                    root.controller.runCleanupModule(moduleId)
                }
            }
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.controller ? root.controller.cleanupResults : []

            delegate: Rectangle {
                width: ListView.view.width
                height: root.controller.themeManager.rowHeight
                color: index % 2 ? root.controller.themeManager.zebraBackground
                                 : root.controller.themeManager.panelBackground

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    Text {
                        Layout.fillWidth: true
                        text: modelData.path || (modelData.paths ? modelData.paths[0] : "")
                        elide: Text.ElideMiddle
                        color: root.controller.themeManager.textColor
                        font.pixelSize: 12
                    }
                    Text {
                        text: modelData.type || ""
                        color: root.controller.themeManager.secondaryTextColor
                        font.pixelSize: 11
                    }
                    Button {
                        text: qsTr("删除")
                        visible: modelData.path !== undefined
                        enabled: root.controller
                        onClicked: {
                            if (root.controller)
                                root.controller.deleteSelectedFiles([modelData.path])
                        }
                    }
                }
            }
        }

        ColumnLayout {
            Layout.preferredWidth: 120
            Button {
                Layout.fillWidth: true
                text: qsTr("全选删除")
                enabled: root.controller
                onClicked: {
                    if (!root.controller) return
                    var paths = []
                    var results = root.controller.cleanupResults
                    for (var i = 0; i < results.length; i++) {
                        if (results[i].path) paths.push(results[i].path)
                    }
                    root.controller.deleteSelectedFiles(paths)
                }
            }
            Button {
                Layout.fillWidth: true
                text: qsTr("自定义命令")
                enabled: root.controller
                onClicked: {
                    if (!root.controller) return
                    var paths = []
                    var results = root.controller.cleanupResults
                    for (var i = 0; i < results.length; i++) {
                        if (results[i].path) paths.push(results[i].path)
                    }
                    root.controller.runCustomCleanupOnSelection(paths)
                }
            }
        }
    }
}

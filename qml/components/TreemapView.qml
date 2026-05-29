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

        RowLayout {
            Layout.fillWidth: true
            ThemedSectionTitle {
                controller: root.controller
                title: qsTr("Treemap")
            }
            Button {
                text: qsTr("上一级")
                enabled: root.controller && root.controller.treemapFocusPath.length > 0
                onClicked: {
                    if (root.controller) root.controller.treemapGoUp()
                }
            }
            Text {
                Layout.fillWidth: true
                text: root.controller ? root.controller.treemapFocusPath : ""
                elide: Text.ElideMiddle
                font.pixelSize: 11
                color: root.controller ? root.controller.themeManager.secondaryTextColor : "#666"
            }
        }

        Item {
            id: canvasArea
            Layout.fillWidth: true
            Layout.fillHeight: true

            Repeater {
                model: root.controller ? root.controller.treemapLayout.rects : []
                delegate: Rectangle {
                    property var item: modelData
                    x: canvasArea.width * (item.x || 0)
                    y: canvasArea.height * (item.y || 0)
                    width: Math.max(2, canvasArea.width * (item.w || 0))
                    height: Math.max(2, canvasArea.height * (item.h || 0))
                    color: item.color || root.controller.themeManager.accentColor
                    border.color: root.controller.themeManager.borderColor
                    border.width: 1

                    Text {
                        anchors.margins: 4
                        anchors.fill: parent
                        text: item.name || ""
                        font.pixelSize: 10
                        color: root.controller.themeManager.isDark ? "#eeeeee" : "#ffffff"
                        elide: Text.ElideRight
                        visible: parent.width > 36 && parent.height > 16
                    }

                    TapHandler {
                        onTapped: {
                            if (root.controller) root.controller.drillTreemap(item.path)
                        }
                        onDoubleTapped: {
                            if (root.controller) root.controller.revealInExplorer(item.path)
                        }
                    }
                }
            }
        }
    }
}

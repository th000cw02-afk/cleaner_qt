import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Analyzer 1.0

RowLayout {
    id: root
    property AnalyzerController controller
    spacing: 8

    ComboBox {
        id: driveCombo
        Layout.preferredWidth: 72
        model: controller ? controller.availableDrives : []
        onActivated: {
            if (controller) controller.scanPath = currentText
        }
        Component.onCompleted: {
            if (controller && controller.availableDrives.length > 0) {
                currentIndex = 0
                controller.scanPath = controller.availableDrives[0]
            }
        }
        Connections {
            target: controller
            function onAvailableDrivesChanged() {
                if (controller && controller.availableDrives.length > 0) {
                    driveCombo.currentIndex = 0
                    controller.scanPath = controller.availableDrives[0]
                }
            }
        }
    }

    TextField {
        id: pathField
        Layout.fillWidth: true
        text: controller ? controller.scanPath : ""
        placeholderText: qsTr("扫描路径")
        onEditingFinished: {
            if (controller) controller.scanPath = text
        }
        Connections {
            target: controller
            function onScanPathChanged() {
                pathField.text = controller.scanPath
            }
        }
    }

    Button {
        text: qsTr("浏览…")
        onClicked: {
            if (controller) controller.pickFolder()
        }
    }

    Label {
        text: qsTr("最小")
        font.pixelSize: 12
        color: controller ? controller.themeManager.secondaryTextColor : "#666"
    }

    ComboBox {
        Layout.preferredWidth: 100
        model: [
            { label: qsTr("无"), bytes: 0 },
            { label: qsTr("1 MB"), bytes: 1048576 },
            { label: qsTr("10 MB"), bytes: 10485760 },
            { label: qsTr("100 MB"), bytes: 104857600 }
        ]
        textRole: "label"
        onActivated: {
            if (controller) controller.minFileSizeFilter = model[currentIndex].bytes
        }
    }
}

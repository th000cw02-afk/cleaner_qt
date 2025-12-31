import QtQuick 2.15
import QtQuick.Controls 2.15

ComboBox {
    id: diskComboBox
    property CleanerController controller
    
    model: controller ? controller.availableDrives : []
    
    onActivated: {
        if (controller) {
            controller.currentDrive = currentText
        }
    }
    
    Component.onCompleted: {
        if (controller && controller.availableDrives.length > 0) {
            currentIndex = 0
            controller.currentDrive = controller.availableDrives[0]
        }
    }
    
    Connections {
        target: controller
        function onAvailableDrivesChanged() {
            if (controller && controller.availableDrives.length > 0) {
                currentIndex = 0
                controller.currentDrive = controller.availableDrives[0]
            }
        }
    }
    
    delegate: ItemDelegate {
        width: diskComboBox.width
        text: modelData
        font.pixelSize: 14
    }
    
    contentItem: Text {
        text: diskComboBox.displayText
        font.pixelSize: 14
        color: "#333333"
        verticalAlignment: Text.AlignVCenter
        leftPadding: 12
    }
    
    background: Rectangle {
        color: diskComboBox.hovered ? "#f0f0f0" : "#ffffff"
        border.color: diskComboBox.activeFocus ? "#2196f3" : "#e0e0e0"
        border.width: diskComboBox.activeFocus ? 2 : 1
        radius: 6
    }
    
    popup: Popup {
        y: diskComboBox.height
        width: diskComboBox.width
        implicitHeight: contentItem.implicitHeight
        padding: 4
        
        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: diskComboBox.popup.visible ? diskComboBox.delegateModel : null
            currentIndex: diskComboBox.highlightedIndex
            
            ScrollIndicator.vertical: ScrollIndicator { }
        }
        
        background: Rectangle {
            color: "#ffffff"
            border.color: "#e0e0e0"
            border.width: 1
            radius: 6
        }
    }
}


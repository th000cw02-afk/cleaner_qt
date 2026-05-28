import QtQuick 2.15
import QtQuick.Controls 2.15
import Analyzer 1.0

TextField {
    id: field
    property AnalyzerController controller
    placeholderText: qsTr("搜索路径或扩展名…")
    text: controller ? controller.searchFilter : ""
    onTextChanged: {
        if (controller) controller.searchFilter = text
    }
    Connections {
        target: controller
        function onSearchFilterChanged() {
            if (controller.searchFilter !== field.text) field.text = controller.searchFilter
        }
    }
}

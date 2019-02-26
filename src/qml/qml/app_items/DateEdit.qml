import QtQuick 2.0
import QtQuick.Controls 1.2

TextField
{
    id:root
    property string dateValue

    inputMask: "0000-00-00"
    implicitHeight: 23
    implicitWidth: 150
    signal dateChanged()

    Calendar{
        id: calendar
        anchors.topMargin: 0
        anchors.top: parent.bottom
        visible: false
        activeFocusOnTab: true
        onReleased: {
            text = date;
            text = text.substr(0, 10);
            dateValue = text;
            visible = false;
        }
    }

    Button{
        id: downBtn
        width: 22
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0
        iconSource: "qrc:/images/icons/drop.png"
        onClicked: calendar.visible = !calendar.visible
    }

    onDateValueChanged: {
        text = dateValue;
        calendar.selectedDate = dateValue;
        dateChanged()
    }

}

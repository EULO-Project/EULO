import QtQuick 2.9
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3

import "../app_pages"

Rectangle
{
    id:root
    color: "#FAFAFA"

    property string resultStr
    Component.onCompleted:
    {
        resultStr = walletModel.contractPage.getLastResult()
    }

    Label
    {
        id:title
        font.weight: Font.Bold
        font.pixelSize:15
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 20
        color: "#333333"
        text:"合约执行结果"
    }



    ScrollView
    {
        clip:true
        id:scrollArea


        anchors.top:title.bottom
        anchors.topMargin: 10
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        contentHeight: resultRec.height


        Rectangle
        {
            id:resultRec
            color: "#FAFAFA"

            width:  scrollArea.width
            height: resultText.contentHeight


            CommonTextArea
            {
                id:resultText
                font.weight: Font.Bold
                font.pixelSize:15
                anchors.top:resultRec.top
                anchors.left:resultRec.left
                anchors.right:resultRec.right
                anchors.bottom: resultRec.bottom
                anchors.margins: 20
                textColor: "#333333"
                textFormat:Text.RichText
                text:root.resultStr
                readOnly: true
            }
        }

        ScrollBar.vertical: MyScrollBar {
            id: scrollBar3
            width:10
            height:scrollArea.height
            anchors.right: scrollArea.right
            policy:ScrollBar.AsNeeded
            visible: resultRec.height > scrollArea.height
            handle.implicitHeight: 150
        }



    }



}

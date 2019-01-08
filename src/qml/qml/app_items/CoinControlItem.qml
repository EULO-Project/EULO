import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as  Controls_Style_1_4

import "../app_pages"
import "../app_dialogs"


Item
{
    id:root
    height:customChangeAddressLabel.y+50

    CoinControlDialog
    {
        id:coinControlDialog

    }


    Label {
        id:coinControlTitle
        font.weight: Font.Medium
        font.pixelSize:16
        font.letterSpacing: 0.355
        anchors.left: parent.left
        anchors.top: parent.top
        color: "#1E5569"
        text:"Coin Control Features"
    }

    CommonButton
    {
        id:openCoinControlBtn
        color: "#469AAC"
        anchors.top: coinControlTitle.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        text:"Open Coin Control"
        width:120

        onClicked:
        {
            coinControlDialog.show()
        }

    }


    Label {
        id:autoHint
        font.weight: Font.Medium
        font.pixelSize:14
        font.letterSpacing: 0.355
        anchors.left: openCoinControlBtn.right
        anchors.leftMargin: 15
        anchors.verticalCenter: openCoinControlBtn.verticalCenter
        color: "#333333"
        text:"Coins automatically selected"
        visible:true
    }

    Item{
        id:statusTable
        height:!visible?0:byteLabel.y+30
        anchors.left: openCoinControlBtn.left
        anchors.leftMargin: 15
        anchors.right:parent.right
        anchors.rightMargin:15
        anchors.top:openCoinControlBtn.bottom
        anchors.topMargin: 20
        visible:!autoHint.visible


        Label {
            id:quantityLabel
            font.weight: Font.Medium
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: parent.left
            anchors.top: parent.top
            color: "#333333"
            text:"Quantity:"
        }


        Label {
            id:quantityContent
            font.weight: Font.Normal
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: quantityLabel.left
            anchors.leftMargin: 60
            anchors.verticalCenter: quantityLabel.verticalCenter
            color: "#333333"
            text:""
        }


        Label {
            id:byteLabel
            font.weight: Font.Medium
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: quantityLabel.left
            anchors.top: quantityLabel.bottom
            anchors.topMargin: 20
            color: "#333333"
            text:"Bytes:"
        }


        Label {
            id:byteContent
            font.weight: Font.Normal
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: byteLabel.left
            anchors.leftMargin: 60
            anchors.verticalCenter: byteLabel.verticalCenter
            color: "#333333"
            text:""
        }

        Label {
            id:amountLabel
            font.weight: Font.Medium
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: quantityLabel.left
            anchors.leftMargin:180
            anchors.verticalCenter: quantityLabel.verticalCenter
            color: "#333333"
            text:"Amount:"
        }


        Label {
            id:amountContent
            font.weight: Font.Normal
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: amountLabel.left
            anchors.leftMargin: 60
            anchors.verticalCenter: quantityLabel.verticalCenter
            color: "#333333"
            text:""
        }

        Label {
            id:priorityLabel
            font.weight: Font.Medium
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: amountLabel.left
            anchors.verticalCenter: byteLabel.verticalCenter
            color: "#333333"
            text:"Priority:"
        }


        Label {
            id:priorityContent
            font.weight: Font.Normal
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: priorityLabel.left
            anchors.leftMargin: 60
            anchors.verticalCenter: byteLabel.verticalCenter
            color: "#333333"
            text:""
        }


        Label {
            id:feeLabel
            font.weight: Font.Medium
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: quantityLabel.left
            anchors.leftMargin:380
            anchors.verticalCenter: quantityLabel.verticalCenter
            color: "#333333"
            text:"Fee:"
        }


        Label {
            id:feeContent
            font.weight: Font.Normal
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: feeLabel.left
            anchors.leftMargin: 60
            anchors.verticalCenter: quantityLabel.verticalCenter
            color: "#333333"
            text:""
        }

        Label {
            id:dustLabel
            font.weight: Font.Medium
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: feeLabel.left
            anchors.verticalCenter: byteLabel.verticalCenter
            color: "#333333"
            text:"Dust:"
        }


        Label {
            id:dustContent
            font.weight: Font.Normal
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: dustLabel.left
            anchors.leftMargin: 60
            anchors.verticalCenter: byteLabel.verticalCenter
            color: "#333333"
            text:""
        }


        Label {
            id:afterFeeLabel
            font.weight: Font.Medium
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: quantityLabel.left
            anchors.leftMargin:580
            anchors.verticalCenter: quantityLabel.verticalCenter
            color: "#333333"
            text:"After Fee:"
        }


        Label {
            id:afterFeeContent
            font.weight: Font.Normal
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: afterFeeLabel.left
            anchors.leftMargin: 60
            anchors.verticalCenter: quantityLabel.verticalCenter
            color: "#333333"
            text:""
        }


        Label {
            id:changeLabel
            font.weight: Font.Medium
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: afterFeeLabel.left
            anchors.verticalCenter: byteLabel.verticalCenter
            color: "#333333"
            text:"Change:"
        }


        Label {
            id:changeContent
            font.weight: Font.Normal
            font.pixelSize:14
            font.letterSpacing: 0.355
            anchors.left: changeLabel.left
            anchors.leftMargin: 60
            anchors.verticalCenter: byteLabel.verticalCenter
            color: "#333333"
            text:""
        }

    }

    Label
    {
        id:customChangeAddressLabel
        font.weight: Font.Normal
        font.pixelSize:14
        font.letterSpacing: 0.355
        anchors.left: openCoinControlBtn.left
        anchors.leftMargin:15
        anchors.top: autoHint.visivle ? autoHint.bottom : statusTable.bottom
        anchors.topMargin:10
        color: "#333333"
        text: "Custom change address"
    }

    CommonTextField
    {
        id:customChangeAddressField
        font.weight: Font.Light
        font.pixelSize:14
        anchors.left: customChangeAddressLabel.right
        anchors.leftMargin: 20
        anchors.verticalCenter: customChangeAddressLabel.verticalCenter
        placeholderText: "Enter a EULO address (e.g. Uek2swfjkwerwherjhbk32)"
        width: 250
        enabled:false
     }


    Label
    {
        id:splitUtxoLabel
        font.weight: Font.Normal
        font.pixelSize:14
        font.letterSpacing: 0.355
        anchors.left: customChangeAddressField.right
        anchors.leftMargin:30
        anchors.verticalCenter: customChangeAddressLabel.verticalCenter
        color: "#333333"
        text: "Split UTXO"
    }

    CommonTextField
    {
        id:splitUtxoField
        font.weight: Font.Light
        font.pixelSize:14
        anchors.left: splitUtxoLabel.right
        anchors.leftMargin: 20
        anchors.verticalCenter: customChangeAddressLabel.verticalCenter
        placeholderText: "# of outputs"
        width: 100
     }

    Label
    {
        id:uuxoSizeLabel
        font.weight: Font.Normal
        font.pixelSize:14
        font.letterSpacing: 0.355
        anchors.left: splitUtxoField.right
        anchors.leftMargin:20
        anchors.verticalCenter: customChangeAddressLabel.verticalCenter
        color: "#333333"
        text: "UTXO Size: 0.000"
    }

}

import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4

import QtGraphicalEffects 1.0

import "../app_items"

Controls_1_4.Tab {

    property string balance_label_str: "余额:  ------  "
    property string ulo_blance_can_get_label_str: " ------  "
    property string ulo_blance_pending_amount_label_str: " ------  "
    property string ulo_blance_immature_amount_label_str: " ------  "
    property string ulo_blance_total_amount_label_str: " ------  "

    property string zero_blance_mature_amount_label_str: " ------  "
    property string zero_blance_unconfirmed_amount_label_str: " ------  "
    property string zero_blance_immature_amount_label_str: " ------  "
    property string zero_blance_total_amount_label_str: " ------  "


    property string combined_blance_ULO_amount_label_str: " ------  "
    property string combined_blance_locked_amount_label_str: " ------  "
    property string combined_blance_unlocked_amount_label_str: " ------  "
    property string combined_blance_zULO_amount_label_str: " ------  "
    property string combined_blance_total_amount_label_str: " ------  "


    Component.onCompleted:
    {
        walletModel.balanceChanged.connect(setBalance)
    }


    function setBalance(balance,
                        unconfirmedBalance,
                        immatureBalance,
                        zerocoinBalance,
                        unconfirmedZerocoinBalance,
                        immatureZerocoinBalance,
                        watchOnlyBalance,
                        watchUnconfBalance,
                        watchImmatureBalance)
    {
        balance_label_str =  "余额: " + walletModel.formatAmount(balance) + unitName
        ulo_blance_can_get_label_str =  walletModel.formatAmount(balance-unconfirmedBalance-immatureBalance) + unitName
        ulo_blance_pending_amount_label_str = walletModel.formatAmount(unconfirmedBalance) + unitName
        ulo_blance_immature_amount_label_str = walletModel.formatAmount(immatureBalance) + unitName
        ulo_blance_total_amount_label_str = walletModel.formatAmount(balance) + unitName


        zero_blance_mature_amount_label_str =  walletModel.formatAmount(zerocoinBalance - immatureZerocoinBalance) + unitName
        zero_blance_unconfirmed_amount_label_str = walletModel.formatAmount(unconfirmedZerocoinBalance) + unitName
        zero_blance_immature_amount_label_str = walletModel.formatAmount(immatureZerocoinBalance) + unitName
        zero_blance_total_amount_label_str = walletModel.formatAmount(zerocoinBalance) + unitName

        combined_blance_ULO_amount_label_str =  walletModel.formatAmount(balance + unconfirmedBalance) + unitName
        combined_blance_locked_amount_label_str = walletModel.formatAmount(walletModel.getlockedCoins()) + unitName
        combined_blance_unlocked_amount_label_str = walletModel.formatAmount(balance + unconfirmedBalance - walletModel.getlockedCoins()) + unitName
        combined_blance_zULO_amount_label_str = walletModel.formatAmount(zerocoinBalance) + unitName
        combined_blance_total_amount_label_str = walletModel.formatAmount(balance + unconfirmedBalance + zerocoinBalance) + unitName
    }



    Rectangle {
        anchors.fill:parent

        radius: 0
        color: "#FAFAFA"



        Label {
            id:balance_label
            font.weight: Font.Normal
            font.pixelSize:20
            anchors.top:parent.top
            anchors.left: parent.left
            anchors.topMargin: 25
            anchors.leftMargin: 22
            color: "#333333"
            text:balance_label_str
        }

        Label{
            id:status_label
            font.weight: Font.Light
            font.pixelSize:11
            anchors.top:balance_label.bottom
            anchors.left: balance_label.left
            anchors.topMargin: 18
            color: "#333333"
            text:"主节点:&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;\<font color=\"#EE637F\">启用\<\/font\>"
        }

        CommonButton{
            id:receive_btn
            color: "#469AAC"
            anchors.left: status_label.left
            anchors.top: status_label.bottom
            anchors.topMargin: 115
            width: 60
            height: 40
            radius: 2

            Image
            {
                anchors.centerIn: parent
                fillMode: Image.PreserveAspectFit
                source: "../../images/icons/receive_btn.png"
                width:24
            }

        }

        Label {
            id:receive_label
            font.weight: Font.Normal
            font.pixelSize:16
            anchors.left: receive_btn.right
            anchors.leftMargin: 11
            anchors.verticalCenter: receive_btn.verticalCenter
            color: "#333333"
            text:"接收地址"
        }

        Label{
            id:transaction_label
            font.weight: Font.Normal
            font.pixelSize:14
            anchors.left: balance_label.left
            anchors.top: receive_label.bottom
            anchors.topMargin: 82
            color: "#1E5569"
            text:"Recent transactions"

        }

        //TODO:dynamic uptate will exceed the limit of rowCount() fix this!

        ListView {
            id: recent_transaction_info_listView
            clip:true
            focus: true
            currentIndex: -1
            width:350
            boundsBehavior:Flickable.StopAtBounds
            anchors.top:transaction_label.bottom
            anchors.topMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            anchors.left: balance_label.left
            model: walletModel.transaction_ProxyModelOverView
            cacheBuffer: 1000
            spacing: 10
            delegate: ItemDelegate {
                width: 350
                height:35
                hoverEnabled: false


                Image
                {
                    id:typeImg
                    width:25
                    height:25
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left
                    fillMode: Image.PreserveAspectFit
                    verticalAlignment: Image.AlignVCenter
                    horizontalAlignment: Image.AlignTop
                    source: address__
                }

                Label {
                    id:timeLabel
                    anchors.top: parent.top
                    anchors.left: typeImg.right
                    anchors.leftMargin: 10
                    font.pixelSize: 11
                    lineHeight: 0.8
                    text:date
                    color:"#333333"
                    font.weight: Font.Light
                    font.letterSpacing: 0.5
                }

                Label {
                    id:amountLabel
                    anchors.top: parent.top
                    anchors.right: parent.right
                    font.pixelSize: 11
                    lineHeight: 0.8
                    text:amount__ + unitName
                    color:amount__ < 0 ?"#FF0000":(confirmed?"#333333":"#808080")
                    font.weight: Font.Light
                    font.letterSpacing: 0.5
                }

                Label {
                    id:addressLabel
                    anchors.top: timeLabel.bottom
                    anchors.bottomMargin: 10
                    anchors.left: timeLabel.left
                    anchors.rightMargin: 2
                    font.pixelSize: 11
                    lineHeight: 0.8
                    text:address___
                    color:amount__ < 0 ?"#808080":"#333333"
                    font.weight: Font.Light
                    font.letterSpacing: 0.5
                }

                onClicked:
                {
                    tab_change(3)
                    gotoTransactionPage(recent_transaction_info_listView.model.mapToSource(recent_transaction_info_listView.model.index(index,0)))
                }




            }





        }

        Label {
            id:transaction_status_label
            font.weight: Font.Normal
            font.pixelSize:14
            anchors.left: transaction_label.right
            anchors.leftMargin: 73
            anchors.top: transaction_label.top
            color: "#EE637F"
            text:walletModel.syncStatus

        }





       //--------------ULO Balance
        Label
        {
            id:ulo_blance_label
            font.weight: Font.Normal
            font.pixelSize:14
            anchors.right: parent.right
            anchors.rightMargin: 400
            anchors.top: parent.top
            anchors.topMargin: 35
            color: "#1E5569"
            text:"ULO Balances"
        }


        Label {
            id:ulo_blance_can_get_label
            anchors.top: ulo_blance_label.bottom
            anchors.topMargin: 10
            anchors.left: ulo_blance_label.left
            font.pixelSize: 11
            text:"可得到:"
            color:"#5B5B5B"
            font.weight: Font.Light
            font.letterSpacing: 0.5

        }

        Label {
            id:ulo_blance_can_get_amount_label
            anchors.verticalCenter: ulo_blance_can_get_label.verticalCenter
            anchors.left: ulo_blance_can_get_label.left
            anchors.leftMargin: 110
            font.pixelSize: 11
            text:ulo_blance_can_get_label_str
            color:"#5B5B5B"
            font.weight: Font.Normal
            font.letterSpacing: 0.5

        }


        Label {
            id:ulo_blance_pending_label
            anchors.top: ulo_blance_can_get_label.bottom
            anchors.topMargin: 13
            anchors.left: ulo_blance_can_get_label.left
            font.pixelSize: 11
            text:"Pending:"
            color:"#5B5B5B"
            font.weight: Font.Light
            font.letterSpacing: 0.5

        }

        Label {
            id:ulo_blance_pending_amount_label
            anchors.verticalCenter: ulo_blance_pending_label.verticalCenter
            anchors.left: ulo_blance_pending_label.left
            anchors.leftMargin: 110
            font.pixelSize: 11
            text:ulo_blance_pending_amount_label_str
            color:"#5B5B5B"
            font.weight: Font.Normal
            font.letterSpacing: 0.5

        }

        Label {
            id:ulo_blance_immature_label
            anchors.top: ulo_blance_pending_label.bottom
            anchors.topMargin: 13
            anchors.left: ulo_blance_pending_label.left
            font.pixelSize: 11
            text:"Immature:"
            color:"#5B5B5B"
            font.weight: Font.Light
            font.letterSpacing: 0.5

        }

        Label {
            id:ulo_blance_immature_amount_label
            anchors.verticalCenter: ulo_blance_immature_label.verticalCenter
            anchors.left: ulo_blance_immature_label.left
            anchors.leftMargin: 110
            font.pixelSize: 11
            text:ulo_blance_immature_amount_label_str
            color:"#5B5B5B"
            font.weight: Font.Normal
            font.letterSpacing: 0.5

        }


        Label {
            id:ulo_blance_total_label
            anchors.top: ulo_blance_immature_label.bottom
            anchors.topMargin: 13
            anchors.left: ulo_blance_immature_label.left
            font.pixelSize: 11
            text:"Total:"
            color:"#5B5B5B"
            font.weight: Font.Light
            font.letterSpacing: 0.5

        }

        Label {
            id:ulo_blance_total_amount_label
            anchors.verticalCenter: ulo_blance_total_label.verticalCenter
            anchors.left: ulo_blance_total_label.left
            anchors.leftMargin: 110
            font.pixelSize: 11
            text:ulo_blance_total_amount_label_str
            color:"#5B5B5B"
            font.weight: Font.Normal
            font.letterSpacing: 0.5

        }

        //--------------ZeroCoin Balance
        Label
        {
            id:zero_blance_label
            font.weight: Font.Normal
            font.pixelSize:14
            anchors.left: ulo_blance_total_label.left
            anchors.top: ulo_blance_total_amount_label.bottom
            anchors.topMargin: 75
            color: "#1E5569"
            text:"Zerocoin Balance"
        }


        Label {
            id:zero_blance_total_label
            anchors.top: zero_blance_label.bottom
            anchors.topMargin: 10
            anchors.left: zero_blance_label.left
            font.pixelSize: 11
            text:"Total:"
            color:"#5B5B5B"
            font.weight: Font.Light
            font.letterSpacing: 0.5

        }

        Label {
            id:zero_blance_total_amount_label
            anchors.verticalCenter: zero_blance_total_label.verticalCenter
            anchors.left: zero_blance_total_label.left
            anchors.leftMargin: 110
            font.pixelSize: 11
            text:zero_blance_total_amount_label_str
            color:"#5B5B5B"
            font.weight: Font.Normal
            font.letterSpacing: 0.5

        }

        Label {
            id:zero_blance_unconfirmed_label
            anchors.top: zero_blance_total_label.bottom
            anchors.topMargin: 13
            anchors.left: zero_blance_label.left
            font.pixelSize: 11
            text:"Unconfirmed:"
            color:"#5B5B5B"
            font.weight: Font.Light
            font.letterSpacing: 0.5

        }

        Label {
            id:zero_blance_unconfirmed_amount_label
            anchors.verticalCenter: zero_blance_unconfirmed_label.verticalCenter
            anchors.left: zero_blance_unconfirmed_label.left
            anchors.leftMargin: 110
            font.pixelSize: 11
            text:zero_blance_unconfirmed_amount_label_str
            color:"#5B5B5B"
            font.weight: Font.Normal
            font.letterSpacing: 0.5

        }


        Label {
            id:zero_blance_immature_label
            anchors.top: zero_blance_unconfirmed_label.bottom
            anchors.topMargin: 13
            anchors.left: zero_blance_label.left
            font.pixelSize: 11
            text:"Immature:"
            color:"#5B5B5B"
            font.weight: Font.Light
            font.letterSpacing: 0.5

        }

        Label {
            id:zero_blance_immature_amount_label
            anchors.verticalCenter: zero_blance_immature_label.verticalCenter
            anchors.left: zero_blance_immature_label.left
            anchors.leftMargin: 110
            font.pixelSize: 11
            text:zero_blance_immature_amount_label_str
            color:"#5B5B5B"
            font.weight: Font.Normal
            font.letterSpacing: 0.5

        }



        Label {
            id:zero_blance_mature_label
            anchors.top: zero_blance_immature_label.bottom
            anchors.topMargin: 13
            anchors.left: zero_blance_label.left
            font.pixelSize: 11
            text:"Mature:"
            color:"#5B5B5B"
            font.weight: Font.Light
            font.letterSpacing: 0.5

        }

        Label {
            id:zero_blance_mature_amount_label
            anchors.verticalCenter: zero_blance_mature_label.verticalCenter
            anchors.left: zero_blance_mature_label.left
            anchors.leftMargin: 110
            font.pixelSize: 11
            text:zero_blance_mature_amount_label_str
            color:"#5B5B5B"
            font.weight: Font.Normal
            font.letterSpacing: 0.5

        }



        //--------------Combined Balances
        Label
        {
            id:combined_blance_label
            font.weight: Font.Normal
            font.pixelSize:14
            anchors.left: ulo_blance_total_label.left
            anchors.top: zero_blance_mature_label.bottom
            anchors.topMargin: 20
            color: "#1E5569"
            text:"Combined Balances"
        }


        Label {
            id:combined_ulo_label
            anchors.top: combined_blance_label.bottom
            anchors.topMargin: 11
            anchors.left: combined_blance_label.left
            font.pixelSize: 11
            text:"ULO:"
            color:"#5B5B5B"
            font.weight: Font.Light
            font.letterSpacing: 0.5

        }

        Label {
            id:combined_ulo_label_amount_label
            anchors.verticalCenter: combined_ulo_label.verticalCenter
            anchors.left: combined_ulo_label.left
            anchors.leftMargin: 110
            font.pixelSize: 11
            text:combined_blance_ULO_amount_label_str
            color:"#5B5B5B"
            font.weight: Font.Normal
            font.letterSpacing: 0.5

        }


        Label {
            id:combined_locked_label
            anchors.top: combined_ulo_label.bottom
            anchors.topMargin: 11
            anchors.left: combined_ulo_label.left
            font.pixelSize: 11
            text:"Locked:"
            color:"#5B5B5B"
            font.weight: Font.Light
            font.letterSpacing: 0.5

        }

        Label {
            id:combined_locked_amount_label
            anchors.verticalCenter: combined_locked_label.verticalCenter
            anchors.left: combined_locked_label.left
            anchors.leftMargin: 110
            font.pixelSize: 11
            text:combined_blance_locked_amount_label_str
            color:"#5B5B5B"
            font.weight: Font.Normal
            font.letterSpacing: 0.5

        }


        Label {
            id:combined_unlocked_label
            anchors.top: combined_locked_label.bottom
            anchors.topMargin: 11
            anchors.left: combined_locked_label.left
            font.pixelSize: 11
            text:"Unlocked:"
            color:"#5B5B5B"
            font.weight: Font.Light
            font.letterSpacing: 0.5

        }

        Label {
            id:combined_unlocked_amount_label
            anchors.verticalCenter: combined_unlocked_label.verticalCenter
            anchors.left: combined_unlocked_label.left
            anchors.leftMargin: 110
            font.pixelSize: 11
            text:"0.00 ULO"
            color:"#5B5B5B"
            font.weight: Font.Normal
            font.letterSpacing: 0.5

        }


        Label {
            id:combined_zulo_label
            anchors.top: combined_unlocked_label.bottom
            anchors.topMargin: 11
            anchors.left: combined_unlocked_label.left
            font.pixelSize: 11
            text:"zULO:"
            color:"#5B5B5B"
            font.weight: Font.Light
            font.letterSpacing: 0.5

        }

        Label {
            id:combined_zulo_amount_label
            anchors.verticalCenter: combined_zulo_label.verticalCenter
            anchors.left: combined_zulo_label.left
            anchors.leftMargin: 110
            font.pixelSize: 11
            text:combined_blance_zULO_amount_label_str
            color:"#5B5B5B"
            font.weight: Font.Normal
            font.letterSpacing: 0.5

        }

        Label {
            id:combined_total_label
            anchors.top: combined_zulo_label.bottom
            anchors.topMargin: 11
            anchors.left: combined_zulo_label.left
            font.pixelSize: 11
            text:"Total:"
            color:"#5B5B5B"
            font.weight: Font.Light
            font.letterSpacing: 0.5

        }

        Label {
            id:combined_total_amount_label
            anchors.verticalCenter: combined_total_label.verticalCenter
            anchors.left: combined_total_label.left
            anchors.leftMargin: 110
            font.pixelSize: 11
            text:combined_blance_total_amount_label_str
            color:"#5B5B5B"
            font.weight: Font.Normal
            font.letterSpacing: 0.5

        }

    }

}
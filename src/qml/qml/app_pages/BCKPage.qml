import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style

import "../app_items"
import "../app_dialogs"

Controls_1_4.Tab{

    Item{
        id:root

        anchors.fill:parent
        property int pageType:rootTab.currentIndex
        property int buy: 0
        property int sell: 1
        property int transfer: 2
        property int transferFrom: 3
        property int balance: 4
        property int totalBonus: 5


        onPageTypeChanged:
        {
            if(root.pageType == root.balance)
                balanceLabel.text = qsTr("Balance: \n") + walletModel.contractPage.getBCKDatas(0,senderField.text)
            else if(root.pageType == root.totalBonus)
                balanceLabel.text = qsTr("TotalBonus: \n") + walletModel.contractPage.getBCKDatas(1,senderField.text)
        }

        Connections
        {
            target:walletModel.contractPage
            onNotifyBCKResult:
            {
                if(error)
                {
                    warningDialog.title = title
                    warningDialog.content_text = errMsg
                    warningDialog.dim_back = false
                    warningDialog.show()
                }
                else
                {
                    warningDialog.title = qsTr("Success")
                    warningDialog.content_text = qsTr("Operation Successful!\n Waiting and refreshing balance after tx is confirmed by blockchain!\n" + resultStr)
                    warningDialog.dim_back = false
                    warningDialog.show()
                    root.clearAll()
                }

            }

        }



        CommonDialog
        {
            id:warningDialog
            title: qsTr("Attention")
            confirm_btn_text: qsTr("Ok")
            cancel_btn_visible: false
            modality: Qt.ApplicationModal
            width:500
            height: 300
            dim_back:true

            onConfirmed:
            {
                warningDialog.close()
            }

        }


        function clearAll()
        {
            amountField.amountField.text = ""
            gasLimitSpin.amountField.text = gasLimitSpin.defaultAmount
            senderField.text = ""
            gasPriceField.amountField.text =  gasPriceField.coinTypeBtn.index === 0?"0.000001":gasPriceField.coinTypeBtn.index === 10?"0.001":"1"
            amountField_.text = ""
        }



        function funcBuy()
        {
            if(amountField.amountField.text.trim() == "")
            {
                warningDialog.title = qsTr("Error!")
                warningDialog.content_text = qsTr("Empty Amount");
                warningDialog.show()
                amountField.amountField.focus = true
                amountField.amountField.critical = true
                return
            }

            var paramList = new Array

            walletModel.contractPage.bckFunctions(0,
                                                  gasLimitSpin.amountField.text,
                                                  gasPriceField.coinTypeBtn.index,
                                                  gasPriceField.amountField.text,
                                                  amountField.amountField.text,
                                                  amountField.coinTypeBtn.index,
                                                  senderField.text,
                                                  paramList)

        }

        function funcSell()
        {
            if(amountField_.text.trim() == "")
            {
                warningDialog.title = qsTr("Error!")
                warningDialog.content_text = qsTr("Empty Amount");
                warningDialog.show()
                amountField_.focus = true
                amountField_.critical = true
                return
            }

            var paramList = new Array

            paramList.push(amountField_.text.trim())

            walletModel.contractPage.bckFunctions(1,
                                                  gasLimitSpin.amountField.text,
                                                  gasPriceField.coinTypeBtn.index,
                                                  gasPriceField.amountField.text,
                                                  amountField.amountField.text,
                                                  amountField.coinTypeBtn.index,
                                                  senderField.text,
                                                  paramList)

        }



        Rectangle
        {
            id:background
            anchors.fill: parent
            color: "#FAFAFA"

        }





        Rectangle
        {
            id:tabRec
            anchors.top: parent.top
            height:60
            anchors.left: parent.left
            anchors.right: parent.right
            color: "#FFFFFF"

            Controls_1_4.TabView{
                currentIndex: 0
                anchors.fill:parent
                id:rootTab



                style:Controls_1_4_style.TabViewStyle
                {
                    frameOverlap: 1
                    tab: Rectangle
                    {
                        id:tab_rec
                        color: "#FFFFFF"
                        implicitWidth: 150
                        implicitHeight: 50

                        Text
                        {
                            id: text
                            anchors.horizontalCenter: tab_rec.horizontalCenter
                            anchors.verticalCenter: tab_rec.verticalCenter
                            anchors.verticalCenterOffset: 5
                            text: styleData.title
                            color: styleData.selected ?"#489BA7":"#333333"
                            font.pixelSize: 15
                            font.weight:Font.Normal
                        }

                        Rectangle
                        {
                            color: "#489BA7"
                            height: 2
                            width: 65
                            anchors.horizontalCenter: tab_rec.horizontalCenter
                            anchors.bottom: tab_rec.bottom
                            anchors.bottomMargin: 0
                            radius:1
                            visible: styleData.selected
                        }






                    }
                    // frame: Rectangle { color: "transparent" }
                }




                Controls_1_4.Tab
                {
                    title: qsTr("Buy")

                    CommonTokenPage
                    {


                    }


                }

                Controls_1_4.Tab
                {
                    title: qsTr("Sell")

                    CommonTokenPage
                    {


                    }
                }

                Controls_1_4.Tab
                {
                    title: qsTr("Transfer")

                    CommonTokenPage
                    {


                    }
                }
                Controls_1_4.Tab
                {
                    title: qsTr("Transfer From")

                    CommonTokenPage
                    {


                    }
                }
                Controls_1_4.Tab
                {
                    title: qsTr("Balance")

                    CommonTokenPage
                    {


                    }
                }

                Controls_1_4.Tab
                {
                    title: qsTr("Total Bonus")

                    CommonTokenPage
                    {


                    }
                }


            }

        }


        CommonTextArea2
        {
            id:balanceLabel
            font.weight: Font.Normal
            font.pixelSize:13
            anchors.top:parent.top
            anchors.topMargin: 60
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.bottom: gasLimitSpin.top
            anchors.bottomMargin: 10
            text:root.pageType == root.balance?qsTr("Balance: "):qsTr("TotalBonus: ")
            visible: root.pageType == root.balance || root.pageType == root.totalBonus
            readOnly: true
            textFormat: Text.RichText
            borderVisible:false

        }


        Label
        {
            id:amountLabel_
            font.weight: Font.Normal
            font.pixelSize:13
            anchors.top:parent.top
            anchors.topMargin: 70
            anchors.left: parent.left
            anchors.leftMargin: 30
            color: "#333333"
            text:qsTr("Amount")
            visible: root.pageType == root.sell || root.pageType == root.transfer || root.pageType == root.transferFrom
        }

        CommonTextField
        {
            id:amountField_
            font.weight: Font.Light
            font.pixelSize:16
            anchors.left: amountLabel_.right
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 30
            horizontalAlignment:TextInput.AlignRight
            anchors.verticalCenter: amountLabel_.verticalCenter
            validator:RegExpValidator { regExp: /^\d+$/ }
            visible: root.pageType == root.sell || root.pageType == root.transfer || root.pageType == root.transferFrom

        }

        Label
        {
            id:amountLabel
            font.weight: Font.Normal
            font.pixelSize:13
            anchors.bottom:gasLimitLabel.top
            anchors.bottomMargin: 35
            anchors.left: parent.left
            anchors.leftMargin: 30
            color: "#333333"
            text:qsTr("Amount")
            visible: root.pageType == root.buy
        }



        AmountField
        {
            id:amountField
            anchors.verticalCenter: amountLabel.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: 145
            visible: root.pageType == root.buy
        }


        Label
        {
            id:gasLimitLabel
            font.weight: Font.Normal
            font.pixelSize:13
            anchors.bottom:senderLabel.top
            anchors.bottomMargin: 35
            anchors.left: parent.left
            anchors.leftMargin: 30
            color: "#333333"
            text:qsTr("Gas Limit")

        }

        SpinField
        {
            id:gasLimitSpin
            anchors.verticalCenter: gasLimitLabel.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 145
            width:250
            defaultAmount: walletModel.contractPage.getDefaultGasLimitOpSend()*20
        }



        Label
        {
            id:gasPriceLabel
            font.weight: Font.Normal
            font.pixelSize:13
            anchors.verticalCenter: gasLimitLabel.verticalCenter
            anchors.right: gasPriceField.left
            anchors.rightMargin: 30
            color: "#333333"
            text:qsTr("Gas Price")

        }


        AmountField
        {
            id:gasPriceField
            anchors.verticalCenter: gasLimitLabel.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20
            width:350
            factor:1
            amountField.text:gasPriceField.coinTypeBtn.index === 0?"0.000001":gasPriceField.coinTypeBtn.index === 10?"0.001":"1"
            amountField.validator: DoubleValidator{
                bottom: gasPriceField.coinTypeBtn.index === 0?0.000001:gasPriceField.coinTypeBtn.index === 10?0.001:1
                top: gasPriceField.coinTypeBtn.index === 0?99999999:gasPriceField.coinTypeBtn.index === 1?99999999999:99999999999999
                notation:DoubleValidator.StandardNotation
                decimals:gasPriceField.coinTypeBtn.index === 0?8:gasPriceField.coinTypeBtn.index === 1?5:2
            }//TODO: Here validator is not working, check it out.


        }


        Label
        {
            id:senderLabel
            font.weight: Font.Normal
            font.pixelSize:13
            anchors.bottom: tokenTableBck.top
            anchors.bottomMargin: 10
            anchors.left: parent.left
            anchors.leftMargin: 30
            color: "#333333"
            text:qsTr("Sender Address")
        }


        CommonTextField
        {
            id:senderField
            anchors.verticalCenter: senderLabel.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 145
            anchors.right: clearBtn.left
            anchors.rightMargin: 20
            showDropImg:true

            hintmodel: walletModel.contractfilterproxy.addressList

            onTextChanged:
            {
                if(root.pageType == root.balance)
                    balanceLabel.text = qsTr("Balance: \n") + walletModel.contractPage.getBCKDatas(0,senderField.text)
                else if(root.pageType == root.totalBonus)
                    balanceLabel.text = qsTr("TotalBonus: \n") + walletModel.contractPage.getBCKDatas(1,senderField.text)
            }
        }

        CommonButton
        {
            id:clearBtn
            color: "#EE637F"
            anchors.verticalCenter: senderLabel.verticalCenter
            anchors.right: confirmBtn.left
            anchors.rightMargin: 20
            text:qsTr("Clear")

            onClicked:
            {
                root.clearAll()
            }

        }

        CommonButton
        {
            id:confirmBtn
            color: "#469AAC"
            anchors.verticalCenter: senderLabel.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20
            text:qsTr("Confirm")

            onClicked:
            {
                if(root.pageType === root.buy)
                {
                    root.funcBuy()
                }
                else if(root.pageType === root.sell)
                {
                    root.funcSell()
                }

            }

        }


        Rectangle
        {
            id: tokenTableBck
            anchors.bottom:parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height:280

            property alias historyTable: tokenTable

            radius: 0
            color: "#FAFAFA"

            CommonComboBox {
                id:header_label
                width: 120
                anchors.top:parent.top
                anchors.left: parent.left
                anchors.topMargin: 20
                anchors.leftMargin: 10
                //color: "#333333"
                //text:"Use this from to request payments ALL fields are optional"
                onCurrentIndexChanged:
                {
                    currentIndex_ = index
                    if(index == 6)
                    {
                        fromDate.visible = true
                        toDate.visible = true
                    }
                    else
                    {
                        fromDate.visible = false
                        toDate.visible = false
                    }
                    walletModel.tokenfilterproxy.chooseDate(index)
                }



                model: ListModel
                {
                    ListElement { modelData: qsTr("ALL"); }
                    ListElement { modelData: qsTr("Today"); }
                    ListElement { modelData: qsTr("This week"); }
                    ListElement { modelData: qsTr("This month"); }
                    ListElement { modelData: qsTr("Last month"); }
                    ListElement { modelData: qsTr("This year"); }
                    ListElement { modelData: qsTr("Range..."); }
                }
            }

            CommonComboBox {
                id:type_label
                width: 280
                anchors.verticalCenter: header_label.verticalCenter
                anchors.left: header_label.right
                anchors.leftMargin: 3
                //color: "#333333"
                //text:"Use this from to request payments ALL fields are optional"
                onCurrentIndexChanged:
                {
                    walletModel.tokenfilterproxy.chooseType(index)
                }


                model: ListModel
                {
                    ListElement { modelData: qsTr("ALL"); }
                    ListElement { modelData: qsTr("Received with"); }
                    ListElement { modelData: qsTr("Sent to"); }
                    ListElement { modelData: qsTr("To yourself"); }

                }
            }



            CommonTextField
            {
                id:query_textField
                font.weight: Font.Light
                font.pixelSize:13
                anchors.left: type_label.right
                anchors.leftMargin: 3
                anchors.right: symbolbox.left
                anchors.rightMargin: 3
                anchors.verticalCenter: header_label.verticalCenter
                placeholderText: qsTr("Enter address or label to search")

                onTextChanged:walletModel.tokenfilterproxy.changedPrefix(text)

            }


            CommonComboBox
            {
                id:symbolbox
                width: 180
                anchors.verticalCenter: header_label.verticalCenter
                anchors.right: min_amount_textField.left
                anchors.rightMargin: 3
                //color: "#333333"
                //text:"Use this from to request payments ALL fields are optional"
                onCurrentIndexChanged:
                {
                    walletModel.tokenfilterproxy.chooseType(index)
                }

                model: walletModel.tokenfilterproxy.tokenNameList
            }

            CommonTextField
            {
                id:min_amount_textField
                width:160
                font.weight: Font.Light
                font.pixelSize:13
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: header_label.verticalCenter
                placeholderText: qsTr("Min amount")
                validator: DoubleValidator
                {
                    bottom: 0
                    decimals: 12
                    notation: DoubleValidator.StandardNotation
                }

                onTextChanged:walletModel.tokenfilterproxy.changedAmount(text)

            }

            DateEdit
            {
                id:fromDate
                anchors.top:header_label.bottom
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.topMargin: 10
                property var locale_: Qt.locale()
                dateValue:new Date().toLocaleDateString(locale_,"yyyy-MM-dd")
                visible: false
                z:tokenTable.z+1

                onDateChanged:
                {
                    if(header_label.currentIndex_ == 6)
                        walletModel.tokenfilterproxy.dateRangeChanged(fromDate.dateValue,toDate.dateValue)
                }

            }

            DateEdit
            {
                id:toDate
                anchors.top:header_label.bottom
                anchors.left: fromDate.right
                anchors.leftMargin: 30
                anchors.topMargin: 10
                property var locale_: Qt.locale()
                dateValue:new Date().toLocaleDateString(locale_,"yyyy-MM-dd")
                visible: false
                z:tokenTable.z+1

                onDateChanged:
                {
                    if(header_label.currentIndex_ == 6)
                        walletModel.tokenfilterproxy.dateRangeChanged(fromDate.dateValue,toDate.dateValue)
                }
            }



            CommonTableView
            {
                id:tokenTable
                anchors.top:fromDate.visible?fromDate.bottom:header_label.bottom
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.topMargin: 10
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                popuperWidth:200
                roles:  ["date","type","address","name","amount"]
                titles: ["Date","Type","Address","Name","Amount"]
                widths: [150,120,180,200,160,width-830]

                model: walletModel.tokenfilterproxy

                itemDelegate: textDelegate

                Component {
                    id:textDelegate

                    Text{
                        text: styleData.value.split("|")[0]
                        color: styleData.value.split("|")[1] === "6"?"#88333333": (styleData.selected? "#333333" : styleData.textColor)
                        elide:styleData.elideMode
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 12
                        font.letterSpacing: 0.5
                        font.weight: Font.Light
                    }
                }


                menuModel: ListModel
                {
                    ListElement {
                        itemData: qsTr("Copy address")
                    }
                    ListElement {
                        itemData: qsTr("Copy amount")
                    }
                    ListElement {
                        itemData: qsTr("Copy transaction ID")
                    }
                    ListElement {
                        itemData: qsTr("Copy full transaction details")
                    }
                    ListElement {
                        itemData: qsTr("Show transaction details")
                    }
                }

                onMenuPicked:
                {
                    if( menuIndex!= 4)
                        walletModel.tokenfilterproxy.menuPicked(tokenTable.currentRow,menuIndex)
                    else
                    {
                        detail_dialog.detail_dialog_str = walletModel.tokenfilterproxy.getTxDescription(tokenTable.currentRow)
                        detail_dialog.show()
                    }
                }

                onDoubleClicked:
                {
                    detail_dialog.detail_dialog_str = walletModel.tokenfilterproxy.getTxDescription(currentRow)
                    detail_dialog.show()
                }


            }

            CommonDialog
            {
                id:detail_dialog
                title:qsTr("Token details")
                cancel_btn_text: qsTr("Cancel")
                content_text: "sssssss"
                width:600
                height: 400
                modality: Qt.WindowModal
                confrim_btn_visible: false
                property string detail_dialog_str

                Item{
                    parent:detail_dialog.background_rec
                    anchors.fill: parent
                    anchors.topMargin: 25

                    CommonTextArea
                    {
                        id:detailArea
                        font.weight: Font.Light
                        font.pixelSize:13
                        anchors.fill: parent
                        anchors.margins: 10
                        anchors.bottomMargin: 50
                        textFormat: Qt.RichText
                        readOnly: true
                        wrapMode: TextEdit.Wrap
                        text:detail_dialog.detail_dialog_str



                    }
                }
            }

        }


    }





}

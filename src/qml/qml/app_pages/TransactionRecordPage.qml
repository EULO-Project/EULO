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


    Rectangle {
        id: recBck
        anchors.fill:parent

        property alias historyTable: history_table

        radius: 0
        color: "#FAFAFA"

        CommonComboBox {
            id:header_label
            width: 120
            anchors.top:parent.top
            anchors.left: parent.left
            anchors.topMargin: 20
            anchors.leftMargin: 30
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
                walletModel.chooseDate(index)
            }



            model: ListModel
            {
                ListElement { modelData: "ALL"; }
                ListElement { modelData: "Today"; }
                ListElement { modelData: "This week"; }
                ListElement { modelData: "This month"; }
                ListElement { modelData: "Last month"; }
                ListElement { modelData: "This year"; }
                ListElement { modelData: "Range..."; }
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
                walletModel.chooseType(index)
            }


            model: ListModel
            {
                ListElement { modelData: "ALL"; }
                ListElement { modelData: "Most Common"; }
                ListElement { modelData: "Received with"; }
                ListElement { modelData: "Sent to"; }
                ListElement { modelData: "Obfuscated"; }
                ListElement { modelData: "Obfuscation Make Collateral Inputs"; }
                ListElement { modelData: "Obfuscation Create Denominations"; }
                ListElement { modelData: "Obfuscation Denominate"; }
                ListElement { modelData: "Obfuscation Collateral Payment"; }
                ListElement { modelData: "To yourself"; }
                ListElement { modelData: "Contract receive"; }
                ListElement { modelData: "Contract send"; }
                ListElement { modelData: "Mined"; }
                ListElement { modelData: "Minted"; }
                ListElement { modelData: "Masternode Reward"; }
                ListElement { modelData: "Received PIV from zPIV"; }
                ListElement { modelData: "Zerocoin Mint"; }
                ListElement { modelData: "Zerocoin Spend"; }
                ListElement { modelData: "Zerocoin Spend, Change in zPIV"; }
                ListElement { modelData: "Zerocoin Spend to Self"; }
                ListElement { modelData: "Other"; }


            }
        }



        CommonTextField
        {
            id:query_textFiled
            font.weight: Font.Light
            font.pixelSize:13
            anchors.left: type_label.right
            anchors.leftMargin: 3
            anchors.right: min_amount_textFiled.left
            anchors.rightMargin: 3
            anchors.verticalCenter: header_label.verticalCenter
            placeholderText: "Enter address or label to search"

            onTextChanged:walletModel.changedPrefix(text)

        }

        CommonTextField
        {
            id:min_amount_textFiled
            width:160
            font.weight: Font.Light
            font.pixelSize:13
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.verticalCenter: header_label.verticalCenter
            placeholderText: "Min amount"
            validator: DoubleValidator
            {
                bottom: 0
                decimals: 12
                notation: DoubleValidator.StandardNotation
            }

            onTextChanged:walletModel.changedAmount(text)

        }

        DateEdit
        {
            id:fromDate
            anchors.top:header_label.bottom
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.topMargin: 10
            property var locale_: Qt.locale()
            dateValue:new Date().toLocaleDateString(locale_,"yyyy-MM-dd")
            visible: false
            z:history_table.z+1

            onDateChanged:
            {
                if(header_label.currentIndex_ == 6)
                walletModel.dateRangeChanged(fromDate.dateValue,toDate.dateValue)
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
            z:history_table.z+1

            onDateChanged:
            {
                if(header_label.currentIndex_ == 6)
                walletModel.dateRangeChanged(fromDate.dateValue,toDate.dateValue)
            }
        }



        CommonTableView
        {
            id:history_table
            anchors.top:fromDate.visible?fromDate.bottom:header_label.bottom
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.topMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 80
            roles:  ["status","date","type","address","amount"]
            titles: ["状态","时间","类型","地址","数量(ULO)"]
            widths: [50,120,180,300,160,width-830]

            model: walletModel.transaction_ProxyModel

            itemDelegate: textDelegate

            Component.onCompleted:
            {
                getColumn(0).delegate = imageDelegate
                getColumn(3).delegate = addressDelegate
            }

            onDoubleClicked:
            {
                detail_dialog.detail_dialog_str = walletModel.getTxDescription(currentRow)
                detail_dialog.show()
            }


            Connections {
                target: history_table.selection
                onSelectionChanged:
                {
                    var rows_arry = new Array
                    history_table.selection.forEach( function(rowIndex) {
                         rows_arry.push(rowIndex);
                    } )

                    if(rows_arry.length>0)
                        selected_amount_label.text = walletModel.caculateSum(rows_arry)
                }

            }

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


            Component {
                id: imageDelegate
                Item {
                    Image {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        fillMode: Image.PreserveAspectFit
                        height:16
                        cache : true;
                        asynchronous: true;
                        source: styleData === undefined?"":styleData.value
                    }
                }
            }

            Component {
                id: addressDelegate
                Item {
                    Image {
                        id:address_icon
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        fillMode: Image.PreserveAspectFit
                        height:16
                        cache : true;
                        asynchronous: true;
                        source: styleData.value.split("|")[1] === undefined  ? "" : styleData.value.split("|")[1]
                    }

                    Text{
                        id:address_item
                        anchors.left:address_icon.right
                        anchors.leftMargin: 15
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        text: styleData.value.split("|")[0]

                        color: styleData.value.split("|")[2] === "6"?"#88333333": (styleData.selected? "#333333" : styleData.textColor)
                        elide:styleData.elideMode
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 12
                        font.letterSpacing: 0.5
                        font.weight: Font.Light


                    }

                }
            }

            CommonDialog
            {
                id:detail_dialog
                title: "Transaction details"
                cancel_btn_text: "Close"
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

        Label {
            id:selected_amount_label
            //font.weight: Font.Medium
            font.pixelSize:13
            font.letterSpacing: 0.355
            anchors.bottom:parent.bottom
            textFormat: Qt.RichText
            anchors.left: parent.left
            anchors.leftMargin: 30
            color: "#333333"
        }

        CommonButton
        {
            id:export_btn
            color: "#1E5569"
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.bottom: parent.bottom
            width: 83
            height: 32
            radius: 3
            text:"导出"
            textSize:12
            letterSpacing:0

            onClicked: walletModel.exportClicked()
        }

        CommonDialog
        {
            id:export_dialog
            cancel_btn_text: "Close"
            width:300
            height: 300
            modality: Qt.WindowModal
            confrim_btn_visible: false


        }

        Connections {
            target: walletModel
            onMessage:
            {
                export_dialog.title = title
                export_dialog.content_text = message
                export_dialog.show()
            }

        }


    }

}

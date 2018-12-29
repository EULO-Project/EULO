import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style
import QtQuick.Controls 1.1 as Controls_1_1

import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.3
import "../app_pages"
import "../app_items"
import "../base_items"


CommonDialog
{
    id:root
    title: "Choose the contract for send/call"

    confrim_btn_visible:false
    cancel_btn_visible:false
    property bool forPicking: false
    signal pickAddress(string address)
    property alias contractAddressDialog: contractAddressDialog
    modality: Qt.ApplicationModal

    width:800
    height:500

    Item{
        parent:root.background_rec
        anchors.fill: parent
        anchors.topMargin: 35
        anchors.leftMargin: 17
        anchors.rightMargin: 17
        anchors.bottomMargin: 17


        ContractAddressDialog
        {
            id:contractAddressDialog
        }


        Connections {
            target: walletModel.contractfilterproxy
            onMessage:
            {
                root_window.warningDialog.title = title
                root_window.warningDialog.content_text = message
                root_window.warningDialog.dim_back = false
                root_window.warningDialog.show()
            }

        }



        Label
        {
            id:sign_label
            anchors.top:parent.top
            width:parent.width
            text: "These are your saved contracts. Always check the contract address and the ABI before sending/calling."
            font.weight: Font.Light
            font.pixelSize: 11
            wrapMode: Text.WrapAnywhere
            lineHeight: 0.8
            font.letterSpacing:0.3
        }


        CommonTableView
        {
            id:contractTable
            anchors.top:sign_label.bottom
            anchors.left: parent.left
            anchors.topMargin: 12
            anchors.right: parent.right
            height:300
            roles:  ["label","address","abi"]
            titles: ["标签","地址","Interface (ABI)"]
            widths: [200,200,100,width-440]

            selectionMode: Controls_1_1.SelectionMode.SingleSelection

            model:walletModel.contractfilterproxy

            onDoubleClicked:
            {
                if(contractTable.currentRow != -1)
                    root.pickAddress(contractTable.model.getData("address",contractTable.currentRow))

                root.close()
            }


            menuModel: ListModel
            {
                ListElement {
                    itemData: "复制标签"
                }
                ListElement {
                    itemData: "复制地址"
                }
                ListElement {
                    itemData: "复制ABI"
                }
                ListElement {
                    itemData: "编辑"
                }
                ListElement {
                    itemData: "删除"
                }
            }

            onMenuPicked:
            {
                if(menuIndex == 0)
                {
                    walletModel.setClipBoard(contractTable.model.getData("label",contractTable.currentRow))
                }
                else if(menuIndex == 1)
                {
                    walletModel.setClipBoard(contractTable.model.getData("address",contractTable.currentRow))
                }
                else if(menuIndex == 2)
                {
                    walletModel.setClipBoard(contractTable.model.getData("abi",contractTable.currentRow))
                }
                else if(menuIndex == 3)
                {
                    contractAddressDialog.currentRow = tableRow
                    contractAddressDialog.editing = true
                    contractAddressDialog.tagTextFiled.text = contractTable.model.getData("label",contractTable.currentRow)
                    contractAddressDialog.addressTextFiled.text = contractTable.model.getData("address",contractTable.currentRow)
                    contractAddressDialog.abiTextArea.text = contractTable.model.getData("abi",contractTable.currentRow)
                    contractAddressDialog.show()
                }
                else if(menuIndex == 4)
                {
                    walletModel.contractfilterproxy.deleteContract(tableRow)
                    contractTable.selection.clear()
                }


            }



        }


        CommonButton
        {
            id:new_btn
            color: "#469AAC"
            anchors.left: parent.left
            anchors.bottom: parent.bottom

            width: 80
            height: 32
            radius: 3
            text:"新地址"
            textSize:11
            letterSpacing:0

            onClicked:
            {
                contractAddressDialog.editing = false
                contractAddressDialog.tagTextFiled.text = ""
                contractAddressDialog.addressTextFiled.text = ""
                contractAddressDialog.abiTextArea.text = ""
                contractAddressDialog.show()
            }

        }

        CommonButton
        {
            id:copy_btn
            color: "#1E5569"
            anchors.left: new_btn.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 8
            width: 60
            height: 32
            radius: 3
            text:"复制"
            textSize:11
            letterSpacing:0
            enabled:contractTable.selection.count > 0

            onClicked:
            {
                walletModel.setClipBoard(contractTable.model.getData("address",contractTable.currentRow))
            }
        }


        CommonButton
        {
            id:del_btn
            color: "#EE637F"
            anchors.left: copy_btn.right
            anchors.leftMargin: 8
            anchors.bottom: parent.bottom
            width: 60
            height: 32
            radius: 3
            text:"删除"
            textSize:11
            letterSpacing:0
            enabled:contractTable.selection.count > 0

            onClicked: {
                walletModel.contractfilterproxy.deleteContract(contractTable.currentRow)
                contractTable.selection.clear()
            }

        }

        CommonButton
        {
            id:export_btn
            color: "#718BBC"
            anchors.right: pick_btn.left
            anchors.bottom: parent.bottom
            anchors.rightMargin: 8
            width: 60
            height: 32
            radius: 3
            text:"导出"
            textSize:11
            letterSpacing:0
            visible: !forPicking

            onClicked:
            {
                walletModel.contractfilterproxy.exportClicked()
            }
        }




        CommonButton
        {
            id:pick_btn
            color: "#718BBC"
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: 60
            height: 32
            radius: 3
            text:"选择"
            textSize:11
            letterSpacing:0
            onClicked: {
                if(contractTable.currentRow != -1)
                    root.pickAddress(contractTable.model.getData("address",contractTable.currentRow))
                root.close()
            }
        }



    }


}

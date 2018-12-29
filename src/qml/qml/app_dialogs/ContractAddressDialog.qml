import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style
import QtGraphicalEffects 1.0

import "../app_pages"
import "../app_items"
import "../base_items"

CommonDialog
{
    id:root
    modality: Qt.ApplicationModal
    width:600
    height: 500
    property var model
    property bool editing: false
    cancel_btn_text: "取消"
    confrim_btn_text: "确认"
    property int currentRow: -1

    property alias tagTextFiled: tag_textFiled
    property alias addressTextFiled: addressTextFiled
    property alias abiTextArea: abiTextArea

    title: editing?"修改合约地址":"新建合约地址"

    onClosing:
    {
        tag_textFiled.text = ""
        addressTextFiled.text = ""
        close_dialog()
    }




    Item{
        parent:root.background_rec
        anchors.fill: parent

        Label {
            id:tag_label
            font.weight: Font.Medium
            font.pixelSize:13
            anchors.top:parent.top
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.topMargin: 70
            color: "#333333"
            text:"标签"
        }


        CommonTextField
        {
            id:tag_textFiled
            font.weight: Font.Light
            font.pixelSize:16
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.verticalCenter: tag_label.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 120
        }

        Label {
            id:address_label
            font.weight: Font.Medium
            font.pixelSize:13
            anchors.top:tag_label.bottom
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.topMargin: 30
            color: "#333333"
            text:"合约地址"
        }


        CommonTextField
        {
            id:addressTextFiled
            font.weight: Font.Light
            font.pixelSize:16
            anchors.rightMargin: 30

            anchors.right: parent.right
            anchors.verticalCenter: address_label.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 120
        }

        Label {
            id:abiLabel
            font.weight: Font.Medium
            font.pixelSize:13
            anchors.top:address_label.bottom
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.topMargin: 30
            color: "#333333"
            text:"Interface (ABI)"
        }


        CommonTextArea
        {
            id:abiTextArea
            font.weight: Font.Light
            font.pixelSize:16
            anchors.rightMargin: 30
            anchors.right: parent.right
            anchors.top: abiLabel.top
            anchors.left: parent.left
            anchors.leftMargin: 120
            anchors.bottom:parent.bottom
            anchors.bottomMargin: 50

        }



    }




    onConfirmed:
    {
        if(addressTextFiled.text.length != 40)
        {
            addressTextFiled.critical = true
            return
        }


        var res = walletModel.contractfilterproxy.saveContract(editing?1:0,
                                                              tag_textFiled.text,
                                                              addressTextFiled.text,
                                                              abiTextArea.text,
                                                              currentRow)
        if(res !== "ok")
        {
            root_window.warningDialog.title = editing?"编辑失败":"新增失败"
            root_window.warningDialog.content_text = res
            root_window.warningDialog.dim_back = false
            root_window.warningDialog.show()
            return
        }


        root.close()
    }

    onCanceled:
    {
        root.close()
    }

}

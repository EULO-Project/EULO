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
    height: 300
    property var model
    property bool forSending: true
    property bool editing: false
    cancel_btn_text: "取消"
    confrim_btn_text: "确认"
    property int currentRow: -1

    property alias tagTextFiled: tag_textFiled
    property alias addressTextFiled: addressTextFiled

    title: editing?(forSending?"修改发送地址":"修改接收地址"):(forSending?"新发送地址":"新接收地址")

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
            anchors.leftMargin: 82
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
            text:"地址"
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
            anchors.leftMargin: 82
            placeholderText: "Enter a EULO address (e.g. u7VFR83SQbiezrW72hjcWJtcfip5krte2Z)"

            enabled: forSending
        }



    }




    onConfirmed:
    {
        if(forSending)
        {
            if(!walletModel.validateAddress(addressTextFiled.text))
            {
                root_window.warningDialog.title = "注意"
                root_window.warningDialog.content_text = "请输入有效的EULO地址"
                root_window.warningDialog.dim_back = false
                root_window.warningDialog.show()
                return
            }
        }

        var res = model.saveAddress(forSending?(editing?3:1):(editing?2:0),tag_textFiled.text,addressTextFiled.text,currentRow)

        if(res !== "ok")
        {
            root_window.warningDialog.title = "添加地址失败"
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

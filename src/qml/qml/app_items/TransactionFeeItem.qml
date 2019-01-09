import QtQuick 2.10
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0

Item {
    id:root
    property var rec_unit
    property var rec_block
    property bool minimized: false

    implicitHeight: !minimized?(swiftx_btn.y + swiftx_btn.height):(transaction_fee_btn.y+transaction_fee_btn.height + swiftx_btn.height)


    Component.onCompleted:
    {
        root_window.unitNameChanged.connect(coinTypeChange)
    }


    function coinTypeChange()
    {
        if(root_window.unitName === "  ULO"){
            amountField.coinTypeBtn.text = "ULO"
            amountField.coinTypeBtn.index = 0
        }
        else if(root_window.unitName === "  mULO"){
            amountField.coinTypeBtn.text = "mULO"
            amountField.coinTypeBtn.index = 1
        }
        else{
            amountField.coinTypeBtn.index = 2
            amountField.coinTypeBtn.text = "μULO"
        }

    }



    Label{
        id:transaction_fee_label
        font.weight: Font.Normal
        font.pixelSize:15
        anchors.left: root.left
        anchors.top: root.top
        color: "#1E5569"
        text:"Transaction Fee"

    }

    Label{
        id:transaction_fee
        font.weight: Font.Normal
        font.pixelSize:15
        anchors.left: transaction_fee_label.right
        anchors.leftMargin: 10
        anchors.verticalCenter: transaction_fee_label.verticalCenter
        color: "#1E5569"
        text:  recommended_btn.checked?"0.10000" + root_window.unitName + "/kB":(perkilo_btn.checked?"0.001/kB":"0.001" + root_window.unitName)
        visible: minimized

    }

    CommonButton
    {
        id:transaction_fee_btn
        color: "#469AAC"
        anchors.left: minimized?transaction_fee.right:transaction_fee_label.right
        anchors.verticalCenter: transaction_fee_label.verticalCenter
        anchors.leftMargin: 15
        width: 85
        height: 32
        radius: 3
        text:!minimized?"Minimize":"Choose..."
        textSize:12
        letterSpacing:0

        onClicked:
        {
            minimized = !minimized
        }
    }

    ButtonGroup {
        id: radioGroup

    }
    ButtonGroup {
        id: radioGroup_custom

    }

    CommonRadioButton{
        id:recommended_btn
        anchors.top:transaction_fee_btn.bottom
        anchors.topMargin: 20
        anchors.left: transaction_fee_label.left
        text: "Recommended " + rec_unit + " ULO/kB  " + "Estimated to begin confirmation within " + rec_block + " block(s)."
        ButtonGroup.group: radioGroup
        font.weight: Font.Medium
        font.pixelSize: 12

        height:35

        visible:!minimized

    }

    Label{
        id:confirmation_time_label
        font.weight: Font.Medium
        font.pixelSize:12
        anchors.left: recommended_btn.left
        anchors.leftMargin: 30
        anchors.top: recommended_btn.bottom
        color: "#333333"
        text:"Confirmation time"
        visible:!minimized
    }


    CommonSlider
    {
        id:confirmation_time_slider
        anchors.left: confirmation_time_label.right
        anchors.leftMargin: 8
        height:16
        anchors.verticalCenter: confirmation_time_label.verticalCenter
        enabled: recommended_btn.checked
        visible:!minimized
        from: 0
        stepSize: 1
        to: 24
        Component.onCompleted:
        {
            value = walletModel.coinControlProxy.getValue(3)
        }


        onMoved:
        {
            console.log("moving!!!")
        }
    }

    Label{
        id:confirmation_time_start_label
        font.weight: Font.Medium
        font.pixelSize:10
        anchors.left: confirmation_time_slider.left
        anchors.leftMargin: 5
        anchors.top: confirmation_time_slider.bottom
        anchors.topMargin: 0
        color: "#333333"
        text:"normal"
        visible:!minimized

    }

    Label{
        id:confirmation_time_end_label
        font.weight: Font.Medium
        font.pixelSize:10
        anchors.right: confirmation_time_slider.right
        anchors.rightMargin: 5
        anchors.top: confirmation_time_slider.bottom
        anchors.topMargin: 0
        color: "#333333"
        text:"fast"
        visible:!minimized

    }


    CommonRadioButton
    {
        id:custom_btn
        anchors.top:recommended_btn.bottom
        anchors.topMargin: 40
        anchors.left: transaction_fee_label.left
        text: "Custom:"
        ButtonGroup.group: radioGroup
        font.weight: Font.Medium
        font.pixelSize: 12
        visible:!minimized

    }


    CommonRadioButton
    {
        id:perkilo_btn
        anchors.verticalCenter: custom_btn.verticalCenter
        anchors.left: custom_btn.right
        anchors.leftMargin: 62
        text: "per kilobyte"
        ButtonGroup.group: radioGroup_custom
        font.weight: Font.Medium
        font.pixelSize: 12
        visible:!minimized
        enabled:custom_btn.checked

    }

    CommonCheckBox {
        id:pay_minimum_checkbox
        font.weight: Font.Medium
        font.pixelSize: 12
        font.letterSpacing: 0.5
        anchors.left: perkilo_btn.left
        anchors.top:amountField.bottom
        height:20
        checked: false
        text: "Pay only the minimum fee of 0.00010000 ULO/kB  (read the tooltip)"
        visible:!minimized
        enabled:custom_btn.checked

    }

    CommonRadioButton
    {
        id:total_btn
        anchors.verticalCenter: custom_btn.verticalCenter
        anchors.leftMargin: 30
        anchors.left: perkilo_btn.right
        text: "total at least"
        ButtonGroup.group: radioGroup_custom
        font.weight: Font.Medium
        font.pixelSize: 12
        visible:!minimized
        enabled:custom_btn.checked


    }

    AmountField
    {
        id:amountField
        anchors.verticalCenter: custom_btn.verticalCenter
        width: 300
        height: 32
        anchors.leftMargin: 10
        anchors.left: total_btn.right
        visible:!minimized
        enabled:custom_btn.checked
    }



    CommonCheckBox {
        id:send_as_zero_checkbox
        font.weight: Font.Medium
        font.pixelSize: 12
        font.letterSpacing: 0.5
        anchors.left: custom_btn.left
        anchors.top:pay_minimum_checkbox.bottom

        checked: false
        text: "Send ad zero-free transaction if possible (confirmation may take longer)"

        visible:!minimized

    }

    CommonCheckBox {
        id:swiftx_btn
        font.weight: Font.Medium
        font.pixelSize: 12
        font.letterSpacing: 0.5
        anchors.left: transaction_fee_label.left
        anchors.top:  !minimized?send_as_zero_checkbox.bottom:transaction_fee_btn.bottom
        anchors.topMargin: 12
        checked: false
        text: "SwitfX"
        height: 17

    }


}

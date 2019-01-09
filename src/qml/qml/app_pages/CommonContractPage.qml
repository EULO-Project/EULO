import QtQuick 2.10
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style

import "../app_items"
import "../app_dialogs"

Rectangle
{
    id:root
    color: "#FAFAFA"
    property int pageType:Create

    property int create: 0
    property int sendTo: 1
    property int call: 2

    ContractBookDialog
    {
        id:contractBookDialog

        onPickAddress:
        {
            contractAddressField.text = address
        }
    }





    Label
    {
        id:byteCodeTitle
        font.weight: Font.Bold
        font.pixelSize:15
        anchors.top:parent.top
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 20
        color: "#333333"
        text:pageType === create?"创建智能合约":(pageType === sendTo?"发送到智能合约":"调用智能合约")
    }

    Label
    {
        id:byteCodeLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top:byteCodeTitle.bottom
        anchors.left: parent.left
        anchors.topMargin: 45
        anchors.leftMargin: 30
        color: "#333333"
        text:"Bytecode"
        visible:pageType === create
    }


    CommonTextArea
    {
        id:byteCodeText
        anchors.verticalCenter: byteCodeLabel.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 130
        height: 75
        visible:pageType === create

    }





    Label
    {
        id:contractAddressLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top:byteCodeTitle.bottom
        anchors.left: parent.left
        anchors.topMargin: 25
        anchors.leftMargin: 30
        color: "#333333"
        text:"合约地址"
        visible:pageType !== create
    }


    CommonTextField
    {
        id:contractAddressField
        anchors.verticalCenter: contractAddressLabel.verticalCenter
        anchors.right: contractAddressRec.left
        anchors.left: parent.left
        anchors.leftMargin: 130
        visible:pageType !== create

        onTextChanged:
        {
            abiCodeText.text = walletModel.contractfilterproxy.updateABI(contractAddressField.text)
        }
    }


    Rectangle{
        id:contractAddressRec
        color: "#FFFFFF"
        height: contractAddressField.height
        width:  95
        visible:pageType !== create
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.verticalCenter: contractAddressLabel.verticalCenter

        RowLayout{
            anchors.centerIn: parent

            spacing: 12
            Image{
                id:contractAddressBookImg
                source: "qrc:/images/icons/addressBook.png"
                fillMode: Image.PreserveAspectFit
                Layout.preferredHeight:18
                Layout.preferredWidth:18
                property bool pressed
                property bool down
                property bool hovered

                Ripple {
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    width: 22; height: 22

                    z: -1
                    anchor: contractAddressBookImg
                    pressed: contractAddressBookImg.pressed
                    active: contractAddressBookImg.down || contractAddressBookImg.visualFocus || contractAddressBookImg.hovered
                    color: contractAddressBookImg.Material.rippleColor
                }

                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered:
                    {
                        contractAddressBookImg.hovered = true
                    }

                    onExited:
                    {
                        contractAddressBookImg.hovered = false
                    }

                    onReleased:
                    {
                        contractAddressBookImg.pressed = false
                    }

                    onPressed:
                    {
                        contractAddressBookImg.pressed = true
                    }

                    onClicked:
                    {
                        contractBookDialog.show()
                    }
                }

            }
            Image{
                id:copyImg
                source: "qrc:/images/icons/copyContract.png"
                fillMode: Image.PreserveAspectFit
                Layout.preferredHeight:15
                Layout.preferredWidth:15
                property bool pressed
                property bool down
                property bool hovered
                Ripple {
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    width: 19; height: 19

                    z: -1
                    anchor: copyImg
                    pressed: copyImg.pressed
                    active: copyImg.down || copyImg.visualFocus || copyImg.hovered
                    color: copyImg.Material.rippleColor
                }

                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered:
                    {
                        copyImg.hovered = true
                    }

                    onExited:
                    {
                        copyImg.hovered = false
                    }

                    onReleased:
                    {
                        copyImg.pressed = false
                    }

                    onPressed:
                    {
                        copyImg.pressed = true
                    }

                    onClicked:
                    {
                        contractAddressField.text = walletModel.getClipBoard("string")
                    }

                }

            }

            Image{
                id:saveContractInfoImg
                source: "qrc:/images/icons/saveToLocal.png"
                fillMode: Image.PreserveAspectFit
                Layout.preferredHeight:15
                Layout.preferredWidth:15
                property bool pressed
                property bool down
                property bool hovered
                Ripple {
                    x: (parent.width - width) / 2
                    y: (parent.height - height) / 2
                    width: 19; height: 19

                    z: -1
                    anchor: saveContractInfoImg
                    pressed: saveContractInfoImg.pressed
                    active: saveContractInfoImg.down || saveContractInfoImg.visualFocus || saveContractInfoImg.hovered
                    color: saveContractInfoImg.Material.rippleColor
                }

                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered:
                    {
                        saveContractInfoImg.hovered = true
                    }

                    onExited:
                    {
                        saveContractInfoImg.hovered = false
                    }

                    onReleased:
                    {
                        saveContractInfoImg.pressed = false
                    }

                    onPressed:
                    {
                        saveContractInfoImg.pressed = true
                    }

                    onClicked:
                    {
                        if(contractAddressField.text.length != 40)
                        {
                            contractAddressField.critical = true
                            return
                        }
                        var row = walletModel.contractfilterproxy.lookupAddress(contractAddressField.text.trim())

                        if(row !== -1)
                        {
                            contractBookDialog.contractAddressDialog.currentRow = row
                            contractBookDialog.contractAddressDialog.editing = true
                            contractBookDialog.contractAddressDialog.tagTextFiled.text = walletModel.contractfilterproxy.getData("label",row)
                            contractBookDialog.contractAddressDialog.addressTextFiled.text = walletModel.contractfilterproxy.getData("address",row)
                            contractBookDialog.contractAddressDialog.abiTextArea.text = walletModel.contractfilterproxy.getData("abi",row)
                            contractBookDialog.contractAddressDialog.show()
                        }
                        else
                        {
                            contractBookDialog.contractAddressDialog.editing = false
                            contractBookDialog.contractAddressDialog.tagTextFiled.text = ""
                            contractBookDialog.contractAddressDialog.addressTextFiled.text = contractAddressField.text.trim()
                            contractBookDialog.contractAddressDialog.abiTextArea.text = abiCodeText.text.trim()
                            contractBookDialog.contractAddressDialog.show()
                        }
                    }

                }

            }

        }

    }






    Label
    {
        id:abiCodeLabel
        font.weight: Font.Normal
        font.pixelSize:13

        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.verticalCenter: abiCodeText.verticalCenter

        color: "#333333"
        text:"Interface (ABI)"
    }


    CommonTextArea
    {
        id:abiCodeText
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.top:pageType === create?byteCodeText.bottom:contractAddressField.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 130
        height: 75

        onTextChanged:
        {
            if(walletModel.contractPage.updateContractABI(pageType,abiCodeText.text)){
                abiCodeText.critical = false
            }
            else{
                abiCodeText.critical = true
            }
        }

        ToolTip.visible: abiCodeText.critical
        ToolTip.text:qsTr("Invalid Json format")
    }

    Label
    {
        id:constructorTitle
        font.weight: Font.Bold
        font.pixelSize:15
        anchors.top:abiCodeText.bottom
        anchors.left: parent.left
        anchors.topMargin: 10
        anchors.leftMargin: 20
        color: "#333333"
        text:pageType === create?"构造器":"函数"
    }

    Rectangle
    {
        id:constructRec
        color:"#F0F0F0"
        anchors.top:constructorTitle.bottom
        anchors.left: parent.left
        anchors.leftMargin: 130
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.bottom: optionsTitle.top
        anchors.bottomMargin: 10
        property alias scrollLoader: scrollArea.scrollLoader
        ScrollView
        {
            id:scrollArea
            anchors.fill: parent
            property alias scrollLoader:scrollLoader
            clip:true


            Loader {
                id:scrollLoader

                Component.onCompleted:
                {
                    var source_ ="qrc:/qml/app_items/ContractItem.qml"
                    scrollLoader.setSource(source_, {"pageType": pageType,"width":scrollArea.width,"abiCodeText":abiCodeText})
                }


                Connections
                {
                    target:scrollLoader.item

                    onFunctionChanged:
                    {
                        if(walletModel.contractPage.isFunctionPayable(index))
                            amountField.amountField.enabled = true
                        else
                            amountField.amountField.enabled = false


                    }

                }

            }


            ScrollBar.vertical: MyScrollBar {
                id: scrollBar3
                width:10
                height:scrollArea.height
                anchors.right: scrollArea.right
                policy:ScrollBar.AsNeeded
                visible: scrollLoader.item.height > scrollArea.height
                handle.implicitHeight: 150
            }





        }

    }

    Label
    {
        id:optionsTitle
        font.weight: Font.Bold
        font.pixelSize:15
        anchors.bottom:parent.bottom
        anchors.bottomMargin:190
        anchors.left: parent.left
        anchors.leftMargin: 20
        color: "#333333"
        text:"可选项"
    }


    Label
    {
        id:amountLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top:optionsTitle.bottom
        anchors.left: parent.left
        anchors.topMargin: 20
        anchors.leftMargin: 30
        color: "#333333"
        text:"数量"
        visible: pageType === sendTo
    }


    AmountField
    {
        id:amountField
        anchors.verticalCenter: amountLabel.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 130
        visible: pageType === sendTo
    }


    Label
    {
        id:gasLimitLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top:pageType === create?optionsTitle.bottom:amountLabel.bottom
        anchors.left: parent.left
        anchors.topMargin: pageType === create?20:30
        anchors.leftMargin: 30
        color: "#333333"
        text:"Gas Limit"
        visible: pageType !== call

    }

    SpinField
    {
        id:gasLimitSpin
        anchors.verticalCenter: gasLimitLabel.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 130
        width:250
        visible: pageType !== call
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
        text:"Gas Price"
        visible: pageType !== call

    }


    AmountField
    {
        id:gasPriceField
        anchors.verticalCenter: gasLimitLabel.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 20
        width:350
        factor:1
        amountField.text:gasPriceField.coinTypeBtn.index === 0?"0.00000041":gasPriceField.coinTypeBtn.index === 1?"0.0004":"0.4"
        amountField.validator: DoubleValidator{
            bottom: gasPriceField.coinTypeBtn.index === 0?0.0000004:gasPriceField.coinTypeBtn.index === 1?0.0004:0.4
            top: gasPriceField.coinTypeBtn.index === 0?99999999:gasPriceField.coinTypeBtn.index === 1?99999999999:99999999999999
            notation:DoubleValidator.StandardNotation
            decimals:gasPriceField.coinTypeBtn.index === 0?8:gasPriceField.coinTypeBtn.index === 1?5:2
        }//TODO: Here validator is not working, check it out.

        visible: pageType !== call

    }


    Label
    {
        id:senderLabel
        font.weight: Font.Normal
        font.pixelSize:13
        anchors.top:pageType !== call?gasPriceLabel.bottom:optionsTitle.bottom
        anchors.left: parent.left
        anchors.topMargin: 30
        anchors.leftMargin: 30
        color: "#333333"
        text:"发送者地址"
    }

    CommonTextField
    {
        id:senderField
        anchors.verticalCenter: senderLabel.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 130
        anchors.right: parent.right
        anchors.rightMargin: 20
        showDropImg:true
        //readOnly: true

        hintmodel: walletModel.contractfilterproxy.addressList
    }



    CommonButton
    {
        id:confirmBtn
        color: enabled?"#469AAC":"#88469AAC"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 20
        width:pageType === sendTo?95:83
        text:pageType === create?"创建智能合约":(pageType === sendTo?"发送到智能合约":"调用智能合约")

        onClicked:
        {
            if(pageType === create)
            {
                root.createContract()
            }
            else if(pageType === sendTo)
            {
                root.sendtoContract()
            }
            else if(pageType === call)
            {
                root.callContract()
            }
        }

    }

    CommonButton
    {
        id:cancelBtn
        color: "#EE637F"
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.right: confirmBtn.left
        anchors.rightMargin: 30
        text:"清除所有"

        onClicked:
        {
            byteCodeText.text = ""
            contractAddressField.text = ""
            abiCodeText.text = ""
            amountField.amountField.text = ""

            if(pageType === create)
                gasLimitSpin.amountField.text = walletModel.contractPage.getDefaultGasLimitOpCreate()
            else
                gasLimitSpin.amountField.text = walletModel.contractPage.getDefaultGasLimitOpSend()

            senderField.text = ""
            gasPriceField.amountField.text =  gasPriceField.coinTypeBtn.index === 0?0.0000004:gasPriceField.coinTypeBtn.index === 1?0.0004:0.4
        }

    }


    CommonDialog
    {
        id:waitDialog
        modality: Qt.ApplicationModal
        property int timeLeft: 3
        property alias countDownTimer: countDownTimer

        Timer
        {
            id:countDownTimer
            interval: 1000
            onTriggered:
            {
                if(waitDialog.timeLeft > 1)
                {
                    waitDialog.timeLeft--
                    waitDialog.confrim_btn_text = "是" + "  (" + waitDialog.timeLeft + ")"
                    countDownTimer.start()
                }
                else
                {
                    waitDialog.confrim_btn_text = "是"
                    waitDialog.confrim_btn_enabled = true
                }

            }
        }


        onCanceled:
        {
          countDownTimer.stop()
        }

        onConfirmed:
        {
            waitDialog.close()

            if(pageType == create)
                walletModel.contractPage.createContractClicked(gasLimitSpin.amountField.text,
                                                           gasPriceField.coinTypeBtn.index,
                                                           gasPriceField.amountField.text,
                                                           byteCodeText.text,
                                                           senderField.text,
                                                           constructRec.scrollLoader.item.getParams()
                                                           )
            else if(pageType == sendTo)
                walletModel.contractPage.sendToContractClicked(constructRec.scrollLoader.item.getCurrentFunction(),
                                                           gasLimitSpin.amountField.text,
                                                           gasPriceField.coinTypeBtn.index,
                                                           gasPriceField.amountField.text,
                                                           contractAddressField.text,
                                                           amountField.amountField.text,
                                                           amountField.coinTypeBtn.index,
                                                           senderField.text,
                                                           constructRec.scrollLoader.item.getParams()
                                                           )




        }

    }

    function checkParams()
    {
        return constructRec.scrollLoader.item.checkParams()
    }


    function createContract()
    {
        if(byteCodeText.text === "")
        {
            byteCodeText.critical = true
            return
        }

        if(abiCodeText.critical || abiCodeText.text === "")
        {
            abiCodeText.critical = true
            return
        }



        if(!checkParams())
            return

        waitDialog.title = "Confirm contract creation."
        waitDialog.content_text = "Are you sure you want to create contract?"
        waitDialog.confrim_btn_enabled = false
        waitDialog.timeLeft = 3
        waitDialog.confrim_btn_text = "是" + "  (" + waitDialog.timeLeft + ")"
        waitDialog.countDownTimer.start()
        waitDialog.show()


    }


    function sendtoContract()
    {
        if(contractAddressField.text === "")
        {
            contractAddressField.critical = true
            return
        }

        if(abiCodeText.critical || abiCodeText.text === "")
        {
            abiCodeText.critical = true
            return
        }





        if(!checkParams())
            return




        waitDialog.title = "Confirm sending to contract."
        waitDialog.content_text = "Are you sure you want to send to the contract contract:\n" + contractAddressField.text
        waitDialog.confrim_btn_enabled = false
        waitDialog.timeLeft = 3
        waitDialog.confrim_btn_text = "是" + "  (" + waitDialog.timeLeft + ")"
        waitDialog.countDownTimer.start()
        waitDialog.show()




    }




    function callContract()
    {
        if(contractAddressField.text === "")
        {
            contractAddressField.critical = true
            return
        }

        if(abiCodeText.critical || abiCodeText.text === "")
        {
            abiCodeText.critical = true
            return
        }


        if(!checkParams())
            return


       walletModel.contractPage.callContractClicked(constructRec.scrollLoader.item.getCurrentFunction(),
                                                       contractAddressField.text,
                                                       senderField.text,
                                                       constructRec.scrollLoader.item.getParams()
                                                       )

    }



}

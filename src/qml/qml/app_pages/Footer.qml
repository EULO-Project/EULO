import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Material.impl 2.3


Rectangle
{
    property string progressbar_title
    property string progressbar_context
    property var progressbar_value
    property string progressbar_tip_str
    property string walletStatusSource

    id:root
    Row{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 15


        spacing:22
        Label{
            id:progressbar_label
            text:progressbar_title
            font.weight: Font.Light
            font.pixelSize: 13
            color: "#333333"
            verticalAlignment: Label.AlignTop
            anchors.verticalCenter: parent.verticalCenter

        }

        ProgressBar {
            id: progressbar
            value: progressbar_value === undefined?0:progressbar_value
            indeterminate: progressbar_value === -1
            //padding: 2
            anchors.verticalCenter: parent.verticalCenter

            ToolTip
            {
                id:progressbar_tip
                text:progressbar_tip_str

            }

            background: Rectangle {
                implicitWidth: 580
                implicitHeight: 16
                color: "#FAFAFA"
                radius: 7.5
                border.color: "#7BB6C3"
                border.width: 1





                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true

                    onEntered:
                    {
                        if(progressbar_tip.visible==false)
                            progressbar_tip.visible=true

                    }

                    onExited:
                    {
                        if(progressbar_tip.visible==true)
                            progressbar_tip.visible=false
                    }
                }
            }

            contentItem: Item {
                implicitWidth: 200
                implicitHeight: 4

                Rectangle {
                    width: progressbar.visualPosition * parent.width
                    height: parent.height
                    radius: 7.5
                    color: "#32788B"
                }

                Label{
                    id:progress_label
                    text:progressbar_context
                    font.weight: Font.Bold
                    font.pixelSize: 10
                    color: "#333333"
                    anchors.centerIn: parent
                }
            }



        }



    }


    Connections
    {
        target:walletModel

        onNotifyBackup:
        {
            walletBackupImg.visible = true
            backup_tip.visible = true
        }

    }

    Image{
        id:walletBackupImg
        source: "qrc:/images/icons/backup.png"
        width:20
        height:20
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 15
        anchors.right: parent.right
        anchors.rightMargin: walletStatusImg.visible?40:10
        fillMode: Image.PreserveAspectFit

        property bool pressed
        property bool down
        property bool hovered
        Ripple {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: 22; height: 22

            z: -1
            anchor: parent
            pressed: parent.pressed
            active: parent.down || parent.visualFocus || parent.hovered
            color: parent.Material.rippleColor
        }

        ToolTip
        {
            id:backup_tip
        }

        MouseArea
        {
            anchors.fill: parent
            hoverEnabled: true
            onReleased:
            {
                parent.pressed = false
            }

            onPressed:
            {
                parent.pressed = true
            }

            onEntered:
            {
                if(walletModel.checkBackupStatus())
                    backup_tip.text = qsTr("You have backed up wallet file less than 1 day.")
                else
                    backup_tip.text = qsTr("It's been more than 1 day since last time you backup your wallet.\nClick icon below to backup your wallet!")

                if(backup_tip.visible==false)
                    backup_tip.visible=true

                parent.hovered = true

            }

            onExited:
            {
                if(backup_tip.visible==true)
                    backup_tip.visible=false

                parent.hovered = false

            }

            onClicked:
            {
                title_item.backup_dialog.open()
            }

        }

    }


    Image{
        id:walletStatusImg
        source: root.walletStatusSource
        width:20
        height:20
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 15
        anchors.right: parent.right
        anchors.rightMargin: 10
        fillMode: Image.PreserveAspectFit
        visible:source != ""
        property bool pressed
        property bool down
        property bool hovered




        Ripple {
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            width: 22; height: 22

            z: -1
            anchor: parent
            pressed: parent.pressed
            active: parent.down || parent.visualFocus || parent.hovered
            color: parent.Material.rippleColor
        }
        ToolTip
        {
            id:lockStatus_tip
        }

        MouseArea
        {
            anchors.fill: parent
            hoverEnabled: true



            onReleased:
            {
                parent.pressed = false
            }

            onPressed:
            {
                parent.pressed = true
            }

            onEntered:
            {
                if(walletModel.getEncryptionStatus() === 1)
                    lockStatus_tip.text = qsTr("Click icon below to unlock your wallet")
                else
                    lockStatus_tip.text = qsTr("Click icon below to lock your wallet")

                if(lockStatus_tip.visible==false)
                    lockStatus_tip.visible=true

                parent.hovered = true

            }

            onExited:
            {
                if(lockStatus_tip.visible==true)
                    lockStatus_tip.visible=false

                parent.hovered = false

            }

            onClicked:
            {

                if(walletModel.getEncryptionStatus() === 1)
                {
                     title_item.askPassphraseDialog.show()
                }
                else
                {
                    if(walletModel.setWalletLocked(true))
                    {
                        title_item.rebuildSettingMenu()
                        root_window.warningDialog.title = qsTr("succeed")
                        root_window.warningDialog.content_text = qsTr("wallet locked successfully")
                        root_window.warningDialog.show()
                    }
                    else
                    {
                        root_window.warningDialog.title = qsTr("failed")
                        root_window.warningDialog.content_text = qsTr("something wrong")
                        root_window.warningDialog.show()
                    }


                }

            }

        }

    }

}

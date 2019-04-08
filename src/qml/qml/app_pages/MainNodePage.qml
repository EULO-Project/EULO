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
        id:root
        anchors.fill:parent

        radius: 0

        color: "#FAFAFA"

        property int timeLeft:60

        Rectangle
        {
            id:top_s
            color: "transparent"
            anchors.left: parent.left
            anchors.topMargin: 20
            anchors.top:parent.top
            anchors.leftMargin: 30
            width: 200
            height:40

            Label {
                id: note_label
                font.pixelSize: 10
                font.weight: Font.Light
                lineHeight: 0.8
                text:qsTr("Note: Status of your masternodes in local wallet can potentially be slightly incorrect.<br>Always wait for wallet to sync additional data and then double check from another node.<br>if your node should be running but you still see \"MISSING\" in \"Status\" field.")
            }

        }

        Component.onCompleted:
        {

            countDownTimer.start()

        }



        Connections
        {
            target:walletModel.masternodetableproxy

            onSetTimer:
            {
                root.timeLeft = time
                if(root.timeLeft === 60)
                    countDownTimer.start()
            }

            onMessage:
            {
                root_window.warningDialog.title = title
                root_window.warningDialog.content_text = msg
                root_window.warningDialog.dim_back = false
                root_window.warningDialog.show()
            }
        }

        Timer
        {
            id:countDownTimer
            interval: 1000
            repeat:true
            onTriggered:
            {
                root.timeLeft--
                status_label.text = qsTr("Status will be updated automatically in(sec): ") + root.timeLeft
                if(root.timeLeft === 0)
                {
                    walletModel.masternodetableproxy.updateMyNodeList(false);
                    countDownTimer.stop()
                }

            }
        }



        CommonTableView
        {
            id:masterodeTable
            anchors.top:top_s.bottom
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.topMargin: 12
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 80
            roles:  ["alias","address","protocol","status","activated","lastseen","pubkey"]
            titles: ["Alias","Address","Protocol","Status","Activated","Last Seen(UTC)","Pubkey"]
            widths: [80,150,150,150,150,200,width-920]

            model: walletModel.masternodetableproxy
            selectionMode:Controls_1_4.SelectionMode.SingleSelection

            menuModel:ListModel
            {
                ListElement
                {
                    itemData: "Start Alias"
                }
            }

            onMenuPicked:
            {
                if(masterodeTable.currentRow < 0) return
                walletModel.masternodetableproxy.startButtonClicked(masterodeTable.currentRow)
            }
        }

        CommonButton
        {
            id:start_alias_btn
            color: "#469AAC"
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.leftMargin: 30
            width: 83
            height: 32
            radius: 3
            text:qsTr("Start Alias")
            textSize:11
            letterSpacing:0
            onClicked:
            {
                if(masterodeTable.currentRow < 0) return
                if(!requestUnlock())
                    walletModel.masternodetableproxy.startButtonClicked(masterodeTable.currentRow)
            }
        }

        CommonButton
        {
            id:start_all_btn
            color: walletModel.isTestNet()?"#202020":"#1E5569"
            anchors.left: start_alias_btn.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 8
            width: 83
            height: 32
            radius: 3
            text:qsTr("Start All")
            textSize:11
            letterSpacing:0

            onClicked:
            {
                if(!requestUnlock())
                    walletModel.masternodetableproxy.startAll()
            }
        }

        CommonButton
        {
            id:start_missing_btn
            color: "#718BBC"
            anchors.left: start_all_btn.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 8
            width: 83
            height: 32
            radius: 3
            text:qsTr("Start Missing")
            textSize:11
            letterSpacing:0

            onClicked:
            {
                if(!requestUnlock())
                    walletModel.masternodetableproxy.startMissing()
            }

        }

        CommonButton
        {
            id:renew_btn
            color: "#EE637F"
            anchors.left: start_missing_btn.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 8
            width: 83
            height: 32
            radius: 3
            text:qsTr("Update")
            textSize:11
            letterSpacing:0


            onClicked:
            {
                walletModel.masternodetableproxy.updateMyNodeList()
            }
        }


        Label {
            id:status_label
            font.weight: Font.Medium
            font.pixelSize:13
            font.letterSpacing: 0.355
            anchors.left: renew_btn.right
            anchors.verticalCenter: renew_btn.verticalCenter
            anchors.leftMargin: 11
            color: "#333333"
        }

    }



}

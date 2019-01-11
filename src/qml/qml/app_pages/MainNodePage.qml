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
                text:"\<font color=\"#EE637F\">注意:\<\/font\>&nbsp;在本地钱包您主节点状态可能会稍微不正确。<br/>
                                 总是等待钱包从另一个节点同步的附加数据，然后仔细检查<br/>
                                 如果您的节点正在运行，但您仍然在\"Status\"字段中看到\"MISSING\"。"
            }
            //FIXME: 这里如果直接用Label 会导致opengl渲染失败。CommonTableView会变得高低不平。

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
                status_label.text = "Status will be updated automatically in(sec): " + root.timeLeft
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
            titles: ["Alias","地址","Protocol","Status","激活","Last Seen(UTC)","Pubkey"]
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
            text:"开始别名"
            textSize:11
            letterSpacing:0
            onClicked:
            {
                if(masterodeTable.currentRow < 0) return
                walletModel.masternodetableproxy.startButtonClicked(masterodeTable.currentRow)
            }
        }

        CommonButton
        {
            id:start_all_btn
            color: "#1E5569"
            anchors.left: start_alias_btn.right
            anchors.bottom: parent.bottom
            anchors.leftMargin: 8
            width: 83
            height: 32
            radius: 3
            text:"开始全部"
            textSize:11
            letterSpacing:0

            onClicked:
            {
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
            text:"开始MISSING"
            textSize:11
            letterSpacing:0

            onClicked:
            {
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
            text:"更新状态"
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

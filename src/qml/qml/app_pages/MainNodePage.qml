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
        anchors.fill:parent

        radius: 0
        color: "#FAFAFA"

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



        CommonTableView
        {
            id:history_table
            anchors.top:top_s.bottom
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.topMargin: 12
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 80
            roles:  ["alias","address","protocol","status","activate","last_seen","pubkey"]
            titles: ["Alias","地址","Protocol","Status","激活","Last Seen(UTC)","Pubkey"]
            widths: [80,150,150,150,150,200,width-920]


            model: ListModel {

                ListElement
                {
                    alias: "MN2";
                    address: "192.168.2.22:58802"
                    protocol: "-1"
                    status: "MISSING"
                    activate:"00m:00s"
                    last_seen:"1920-02-19 14:22:00"
                    pubkey:"123456789"
                }
                ListElement
                {
                    alias: "MN2";
                    address: "192.168.2.22:58802"
                    protocol: "-1"
                    status: "MISSING"
                    activate:"00m:00s"
                    last_seen:"1920-02-19 14:22:00"
                    pubkey:"123456789"
                }
                ListElement
                {
                    alias: "MN2";
                    address: "192.168.2.22:58802"
                    protocol: "-1"
                    status: "MISSING"
                    activate:"00m:00s"
                    last_seen:"1920-02-19 14:22:00"
                    pubkey:"123456789"
                }
                ListElement
                {
                    alias: "MN2";
                    address: "192.168.2.22:58802"
                    protocol: "-1"
                    status: "MISSING"
                    activate:"00m:00s"
                    last_seen:"1920-02-19 14:22:00"
                    pubkey:"123456789"
                }
                ListElement
                {
                    alias: "MN2";
                    address: "192.168.2.22:58802"
                    protocol: "-1"
                    status: "MISSING"
                    activate:"00m:00s"
                    last_seen:"1920-02-19 14:22:00"
                    pubkey:"123456789"
                }
                ListElement
                {
                    alias: "MN2";
                    address: "192.168.2.22:58802"
                    protocol: "-1"
                    status: "MISSING"
                    activate:"00m:00s"
                    last_seen:"1920-02-19 14:22:00"
                    pubkey:"123456789"
                }


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
            text:"Status will be updated automatically in(sec): 0"
        }

    }



}

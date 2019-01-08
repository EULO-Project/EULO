import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls.Material.impl 2.3
import QtQuick.Controls 1.4 as Controls_1_4
import QtQuick.Controls.Styles 1.4 as Controls_1_4_style
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.3
import TrafficGraphWidget 1.0
import "../app_pages"
import "../app_items"
import "../base_items"


CommonDialog
{
    id:root
    title: "Options"
    confrim_btn_visible:false
    cancel_btn_visible:false
    property alias current_index:tabview.currentIndex
    width:700
    height:700

    signal coincontrolChanged(int checked)



    Item{
        id:rootItem
        parent:root.background_rec
        anchors.fill: parent
        anchors.topMargin: 25
        anchors.bottomMargin: 60

        CommonTabView{
            id:tabview
            tab_width:40
            currentIndex: -1


            Controls_1_4.Tab {
                title: "一般设置"


                Rectangle{
                    id:tab1_rec
                    anchors.fill: parent
                    color: "#FAFAFA"

                    Label
                    {
                        id:info_label
                        font.weight: Font.Light
                        font.pixelSize:14
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.margins: 10
                        anchors.bottomMargin: 100
                        lineHeight: 1.03
                        width:200
                        wrapMode:Text.WrapAnywhere
                        font.letterSpacing:0.3
                        text:"<b>General</b><br>Client name<br>Client version<br>&nbsp;&nbsp; \
                             Using OpenSSL version<br>&nbsp;&nbsp;Using BerkeleyDB version<br>Build date<br>Startup time<br> \
                             <b>Network</b><br>Name<br>Number of connections<br>Number of Masternodes<br> \
                             <b>Block chain</b><br>Current number of blocks<br>Last block time"
                    }

                    TextEdit
                    {
                        id:info_textArea
                        font.weight: Font.Light
                        font.pixelSize:14
                        font.letterSpacing:0.3
                        anchors.left: parent.left
                        textFormat: Qt.RichText
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 100
                        anchors.topMargin: 6
                        anchors.leftMargin: 250
                        width:350
                        wrapMode:Text.WrapAnywhere
                        readOnly: true
                        selectByMouse: true

                    }

                    Label
                    {
                        id:openDebugLog_label
                        font.weight: Font.Bold
                        font.pixelSize:14
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        anchors.margins: 10
                        anchors.bottomMargin: 35
                        lineHeight: 1.035
                        width:200
                        height:30
                        wrapMode:Text.WrapAnywhere
                        font.letterSpacing:0.3
                        text:"<b>Debug log file</b>"
                    }

                    CommonButton
                    {
                        id:openDebugLog_btn
                        color: "#469AAC"
                        anchors.left: parent.left
                        anchors.bottom: parent.bottom
                        anchors.margins: 10
                        text:"打开"
                        width: 100
                        height: 28
                        textSize:11
                        onClicked:
                        {
                            rpcConsole.openDebugLogFile()
                        }
                    }



                    Component.onCompleted:
                    {
                        info_textArea.text = rpcConsole.getStartUpInfo()
                    }

                    Connections
                    {
                        target: rpcConsole

                        onInfoUpdate:
                        {
                            info_textArea.text = info
                        }
                    }

                }

            }

            Controls_1_4.Tab {
                title: "钱包"

                Rectangle{
                    id:tab2_rec
                    anchors.fill: parent
                    color: "#FAFAFA"

                    Label
                    {
                        id:expertTitle
                        anchors.top:parent.top
                        anchors.left: parent.left
                        text:"Expert"
                        font.weight: Font.Medium
                        font.pixelSize:13
                        font.letterSpacing:0.3
                    }


                    CommonCheckBox {
                        id:coincontrolEnableCheckBox
                        font.weight: Font.Medium
                        font.pixelSize: 12
                        font.letterSpacing: 0.5
                        anchors.left: perkilo_btn.left
                        anchors.top:amountField.bottom
                        height:20
                        checked: false
                        text: "启用硬币控制功能"
                        onCheckStateChanged:
                        {
                            if(checked)
                                root.coincontrolChanged(1)
                            else
                                root.coincontrolChanged(0)
                        }
                    }


                }

            }

            Controls_1_4.Tab {
                title: "网络"


                Rectangle{
                    id:tab3_rec
                    anchors.fill: parent
                    color: "#FAFAFA"




                }



            }

            Controls_1_4.Tab {
                title: "并列"

                onVisibleChanged:
                {
                    if(visible){
                        if(peerTimerStarted)
                            return

                        rpcConsole.peerTableModel_sorted.startAutoRefresh()
                        peerTimerStarted = true
                    }
                    else{
                        if(!peerTimerStarted)
                            return
                        rpcConsole.peerTableModel_sorted.stopAutoRefresh()
                        peerTimerStarted = false
                    }
                }


                Rectangle
                {
                    id:tab4_rec
                    anchors.fill: parent
                    color: "#FAFAFA"


                    CommonTableView
                    {
                        id:address_table
                        anchors.top:parent.top
                        anchors.left: parent.left
                        anchors.topMargin: 10
                        width:420
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 10
                        roles:  ["address","version","ping"]
                        titles: ["Address/Hostname","Version","Ping Time"]
                        widths: [160,120,90,width-380]
                        model: rpcConsole.peerTableModel_sorted
                        selectionMode:Controls_1_4.SelectionMode.SingleSelection
                        //dontPerformSortInner:true

                        //                        onSortIndicatorColumnChanged: {
                        //                            rpcConsole.peerTableModel_sorted.sortPeerTable(getColumnbyRole(),sortIndicatorOrder)
                        //                        }
                        //                        onSortIndicatorOrderChanged: {
                        //                            rpcConsole.peerTableModel_sorted.sortPeerTable(getColumnbyRole(),sortIndicatorOrder)
                        //                        }

                        function getColumnbyRole()
                        {
                            if(getColumn(sortIndicatorColumn).role === "address") return  0
                            else if (getColumn(sortIndicatorColumn).role === "version")return  1
                            else if (getColumn(sortIndicatorColumn).role === "ping")return  2
                        }

                        onCurrentRowChanged:
                        {
                            if(rowCount <= 0 )
                                return

                            if(currentRow != -1)
                            {
                                detail_title.text = rpcConsole.peerTableModel_sorted.getPeerTitleInfo(currentRow)
                                detail_text.text = rpcConsole.peerTableModel_sorted.getPeerDetailedInfo(currentRow)
                            }


                        }

                    }

                    Label
                    {
                        id:detail_title
                        anchors.top:parent.top
                        anchors.left: address_table.right
                        anchors.right: parent.right
                        horizontalAlignment:Text.AlignHCenter
                        height:60
                        anchors.margins: 20
                        color: "#469AAC"
                        font.weight: Font.Medium
                        font.pixelSize:14
                        font.letterSpacing:0.3

                    }

                    Label
                    {
                        id:detail_text_leading
                        anchors.top:detail_title.bottom
                        anchors.left: address_table.right
                        width:100
                        anchors.margins: 20
                        font.weight: Font.Medium
                        font.pixelSize:13
                        font.letterSpacing:0.3
                        text:"Direction\nProtocol\nVersion\nServices\nStarting Height\nSync Height\nBan Score\nConnection Time\nLast Send\nLast Receive\nBytes Send\nBytes Received\nPing Time\n"
                        visible: address_table.currentRow != -1
                    }

                    Label
                    {
                        id:detail_text
                        anchors.top:detail_title.bottom
                        anchors.left: detail_text_leading.right
                        anchors.right: parent.right
                        anchors.margins: 20
                        anchors.leftMargin: 30
                        font.weight: Font.Medium
                        font.pixelSize:13
                        font.letterSpacing:0.3

                    }


                }


            }

            Controls_1_4.Tab {
                title: "修复钱包"


                Rectangle
                {
                    id:tab5_rec
                    anchors.fill: parent
                    color: "#FAFAFA"

                    Label
                    {
                        id:wallet_repair_label
                        anchors.top:parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 20
                        font.weight: Font.Light
                        font.pixelSize:14
                        font.letterSpacing:0.3
                        wrapMode: Label.WordWrap
                        text:"<b>Wallet repair options.</b><br> \
                             The buttons below will restart the wallet with command-line options to repair the wallet, fix issues with corrput blockchain files or missing/obsolete transactions.<br>\
                             Wallet In Use:  " + rpcConsole.getWalletPath()

                    }

                        CommonButton
                        {
                            id:salvage_btn
                            color: "#469AAC"
                            anchors.left: parent.left
                            anchors.top: wallet_repair_label.bottom
                            anchors.topMargin: 80
                            text:"Salvage wallet"
                            width: 220
                            height: 28
                            textSize:13
                            onClicked:
                            {
                                rpcConsole.walletSalvage();
                            }
                        }

                        CommonButton
                        {
                            id:rescan_block_btn
                            color: "#469AAC"
                            anchors.left: parent.left
                            anchors.top: salvage_btn.bottom
                            anchors.topMargin: 15
                            text:"Rescan blockchain files"
                            width: 220
                            height: 28
                            textSize:13
                            onClicked:
                            {
                                rpcConsole.walletRescan();
                            }
                        }

                        CommonButton
                        {
                            id:recover_tx_1_btn
                            color: "#469AAC"
                            anchors.left: parent.left
                            anchors.top: rescan_block_btn.bottom
                            anchors.topMargin: 15
                            text:"Recover transactions 1"
                            width: 220
                            height: 28
                            textSize:13
                            onClicked:
                            {
                                rpcConsole.walletZaptxes1();
                            }
                        }

                        CommonButton
                        {
                            id:recover_tx_2_btn
                            color: "#469AAC"
                            anchors.left: parent.left
                            anchors.top: recover_tx_1_btn.bottom
                            anchors.topMargin: 15
                            text:"Recover transactions 2"
                            width: 220
                            height: 28
                            textSize:13
                            onClicked:
                            {
                                rpcConsole.walletZaptxes2()
                            }
                        }

                        CommonButton
                        {
                            id:upgrade_format_btn
                            color: "#469AAC"
                            anchors.left: parent.left
                            anchors.top: recover_tx_2_btn.bottom
                            anchors.topMargin: 15
                            text:"Upgrade wallet format"
                            width: 220
                            height: 28
                            textSize:13
                            onClicked:
                            {
                                rpcConsole.walletUpgrade()
                            }
                        }

                        CommonButton
                        {
                            id:rebuild_index_btn
                            color: "#469AAC"
                            anchors.left: parent.left
                            anchors.top: upgrade_format_btn.bottom
                            anchors.topMargin: 15
                            text:"Rebuild index"
                            width: 220
                            height: 28
                            textSize:13
                            onClicked:
                            {
                                rpcConsole.walletReindex()
                            }
                        }


                        CommonButton
                        {
                            id:delete_local_blockchain_btn
                            color: "#469AAC"
                            anchors.left: parent.left
                            anchors.top: rebuild_index_btn.bottom
                            anchors.topMargin: 15
                            text:"Delete local blockchain Folders"
                            width: 220
                            height: 28
                            textSize:13
                            onClicked:
                            {
                                confirm_dialog.show()
                            }


                            CommonDialog
                            {
                                id:confirm_dialog
                                title: "resync wallet"
                                confrim_btn_text: "Yes"
                                cancel_btn_text: "No"
                                content_text: "This will delete your local blockchain folders and the wallet will synchronize the complete Blockchain from scratch.\nConfirm?"
                                width:400
                                height: 300
                                modality: Qt.WindowModal
                                onConfirmed:
                                {
                                    rpcConsole.walletResync();
                                }

                            }
                        }

                        Label
                        {
                            id:options_label
                            anchors.left: salvage_btn.right
                            anchors.leftMargin: 20
                            anchors.top: wallet_repair_label.bottom
                            anchors.topMargin: 80
                            font.weight: Font.Light
                            font.pixelSize:14
                            font.letterSpacing:0.3
                            lineHeight: 2.18
                            text:"-salvagewallet:
-rescan:
-zapwallettxes=1:
-zapwallettxes=2:
-upgradewallet:
-reindex:
-resync:"

                        }

                        Label
                        {
                            id:description_label
                            anchors.right: parent.right
                            anchors.rightMargin: 20
                            width:250
                            anchors.top: wallet_repair_label.bottom
                            anchors.topMargin: 80
                            font.weight: Font.Light
                            font.pixelSize:13
                            font.letterSpacing:0.3
                            lineHeight: 0.97
                            wrapMode: Label.WordWrap
                            text:"Attempt to recover private keys from a corrupt wallet.dat.
Rescan the block chain for missing wallet transactions.
Recover transactions from blockchain(keep meta-data e.g. account owner)
Recover transactions from blockchain(drop meta-data)
Upgrade wallet to latest format on startup.(Note:this is NOT an update of the wallet itself!)
Rebuild block chain index from current blk001??.dat files.
Deletes all local blockchain folders so the wallet synchronizes from scratch."

                        }


                    }

                }


            }

        }



        CommonButton
        {
            id:restoreBtn
            color: "#469AAC"
            anchors.left:rootItem.left
            anchors.bottom: parent.bottom
            anchors.bottomMargin:20
            text:"重置选项"
            width: 100
            height: 28
            textSize:9
            onClicked: {

            }
        }



        CommonButton
        {
            id:cancleBtn
            color: "#EE637F"
            anchors.left:rootItem.right
            anchors.bottom: parent.bottom
            anchors.bottomMargin:20
            text:"取消"
            width: 70
            height: 28
            textSize:9
            onClicked: {
                root.close()
            }
        }



        CommonButton
        {
            id:confirmBtn
            color: "#469AAC"
            anchors.right:cancleBtn.left
            anchors.rightMargin:10
            anchors.bottom: parent.bottom
            anchors.bottomMargin:20
            text:"确认"
            width: 70
            height: 28
            textSize:9
            onClicked: {
                root.close()
            }
        }



    }

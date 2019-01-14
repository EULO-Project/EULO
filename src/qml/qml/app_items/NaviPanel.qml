import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3

Row{

    NaviItem
    {
        id:index
        icon:"../../images/navi/shouye.png"
        title: "概述"
        picked: true
        index_:0
    }

    NaviItem
    {
        id:send
        icon:"../../images/navi/fasong.png"
        title: "发送"
        picked: false
        index_:1
    }

    NaviItem
    {
        id:receive
        icon:"../../images/navi/jieshou.png"
        title: "接收"
        picked: false
        index_:2
    }

    NaviItem
    {
        id:record
        icon:"../../images/navi/jilv.png"
        title: "交易记录"
        picked: false
        index_:3
    }

    NaviItem
    {
        id:privacy
        icon:"../../images/navi/privacy.png"
        title: "隐私"
        picked: false
        index_:4
    }

    NaviItem
    {
        id:mainnode
        icon:"../../images/navi/jiedian.png"
        title: "主节点"
        picked: false
        index_:5
    }

    NaviItem
    {
        id:contractNode
        icon:"../../images/navi/heyue.png"
        title: "智能合约"
        picked: false
        index_:6

        SecondaryNaviPanel
        {
            id:secondaryPanel
            y: 80
            x:contractNode.width/2 - width/2


            model: ListModel
            {
                ListElement {name:"1"  }
                ListElement {name:"1"  }

                function getItemData(index) {
                           if ( getItemData[ "text" ] === undefined) {
                               getItemData.text = [
                                   qsTr( "Smart Contract" ) ,
                                   qsTr( "URC Token" )
                               ]
                           }
                           return getItemData.text[index]
                  }
            }

        }

        onClicked:
        {
            secondaryPanel.openMenu()
        }



    }

    function reset_all_items()
    {
        index.picked = false
        send.picked = false
        receive.picked = false
        record.picked = false
        privacy.picked = false
        mainnode.picked = false
        contractNode.picked = false

    }


    function gotoTxpage()
    {
        reset_all_items()
        record.picked = true
        tab_change(3)
    }

    function gotoReceivepage()
    {
        reset_all_items()
        receive.picked = true
        tab_change(2)
    }


}

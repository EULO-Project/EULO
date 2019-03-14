import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import QtGraphicalEffects 1.0

Rectangle{
    id:root
    implicitHeight: 80
    implicitWidth: 95
    color: walletModel.isTestNet()?"#202020":"#1E5569"
    property string title
    property string icon
    property bool picked
    property bool hovered

    property int index_
    signal clicked()

    LinearGradient {
        source: parent;
        visible: picked || hovered

        width:parent.width
        height:parent.height
        gradient: Gradient {
            GradientStop{ position: 0.0; color: hovered?(walletModel.isTestNet()?"#9B202020":"#9B1E5569"):(walletModel.isTestNet()?"#202020":"#1E5569");}
            GradientStop{ position: 1.0; color: hovered?(walletModel.isTestNet()?"#9B202020":"#9B1E5569"):(walletModel.isTestNet()?"#202020":"#1E5569");}
        }
        start: Qt.point(parent.width/2,0);
        end: Qt.point(parent.width/2, parent.height);
    }


    Item{
        anchors.fill: parent

        Image{
            id:navi_img
            source: icon
            fillMode: Image.PreserveAspectFit
            anchors.top:parent.top
            anchors.topMargin: 15
            anchors.horizontalCenter: parent.horizontalCenter
            width:33
            height:33

        }

        Label{
            id:navi_label
            text:title
            color: "#FFFFFF"
            anchors.top:navi_img.bottom
            anchors.topMargin: 8
            font.pixelSize: 12
            anchors.horizontalCenter: parent.horizontalCenter
            font.weight: Font.Normal
        }
    }

    MouseArea
    {
        hoverEnabled: true
        anchors.fill: parent

        onEntered:
        {
            if(!picked)
            {
              hovered = true
            }
        }

        onExited:
        {
            hovered = false
        }

        onClicked:
        {
            if(!picked)
            {
                reset_all_items()
                picked = true

                if(index_ != 6)
                    tab_change(index_)


            }
            root.clicked()

        }

    }

}

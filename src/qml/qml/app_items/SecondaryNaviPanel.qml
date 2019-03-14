import QtQuick 2.0
import QtQuick.Controls 2.3

Popup {
    id:root

    width:listview.contentWidth/2
    padding: 0
    property alias model: listview.model
    property int height_: 35

    function openMenu()
    {
        if(animateWidth_out.running)
            animateWidth_out.stop()

        animateWidth_in.start()

        root.open()
    }

    function closeMenu()
    {
        if(animateWidth_in.running)
            animateWidth_in.stop()

        animateWidth_out.start()

        root.close()
    }

    onAboutToHide:
    {
        closeMenu()
    }

    PropertyAnimation {id: animateWidth_in; targets: root; properties: "width"; to: listview.contentWidth; duration: 200}
    PropertyAnimation {id: animateWidth_out; targets: root; properties: "width"; to: listview.contentWidth/2; duration: 200}

    contentItem: ListView {
        id:listview
        clip: false
        width:contentWidth
        height: height_
        boundsBehavior:Flickable.StopAtBounds
        orientation: ListView.Horizontal




        currentIndex: 0

        delegate:MenuItem {
            width: itemLabel.width
            height:height_
            Label
            {
                id:itemLabel
                //anchors.fill: parent
                height:height_
                width: contentWidth+10


                text: name
                font.weight: Font.Medium
                font.pixelSize:13
                font.letterSpacing: 0.355
                verticalAlignment: Label.AlignVCenter
                horizontalAlignment: Label.AlignHCenter
                color: "#FAFAFA"


            }

            background: Rectangle
            {
                color:hovered?"#246775":"#2e7e90"
            }


            MouseArea
            {
                anchors.fill:parent
                hoverEnabled: true
                onClicked: {

                    if(index === 0)
                        tab_change(6)
                    else if(index === 1)
                        tab_change(7)
                    else if(index === 2)
                        tab_change(8)



                    root.closeMenu()
                }



            }

        }
    }
}

import QtQuick 2.10
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import QtQuick.Controls 1.4 as Controls_1_4


Rectangle
{
    property string progressbar_title
    property string progressbar_context
    property var progressbar_value
    property string progressbar_tip_str

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

//                Controls_1_4.ProgressBar
//                {
//                    id:progressbar
//                    maximumValue:100
//                    value:1

//                    style: Controls_1_4_style.ProgressBarStyle {
//                        background: Rectangle {
//                            radius: 7.5
//                            color: "#FAFAFA"
//                            border.color: "#7BB6C3"
//                            border.width: 1
//                            implicitWidth: 615
//                            implicitHeight: 16
//                        }
//                        progress: Rectangle {
//                            color: "#32788B"
//                            border.color: "#32788B"
//                            radius: 7.5
//                            //z:background.z-1
//                            Rectangle {
//                                color: "#32788B"
//                                border.color: "#32788B"
//                                width:7.5
//                                anchors.right: parent.right
//                                anchors.rightMargin: 0
//                                height: parent.height
//                                visible: parent.width>7.5?true:false

//                            }
//                        }
//                    }



//                }
    }
    //}

}

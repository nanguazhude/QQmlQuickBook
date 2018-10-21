import QtQuick 2.11
import QtGraphicalEffects 1.0

RectangleNotExist {
    
    width : 1024 ;
    height : 1024 ;
    color : Qt.rgba(0.5,0.5,0.5,0) ;

    Grid{
        anchors.fill: parent ;
        columns: 16 ;
        Repeater{
            model: 16 * 16 ;
            delegate: Rectangle{
                width: 1024 / 16 ;
                height: 1024 /16 ;
                color: Qt.rgba(0,0,0,0)
                border.color: Qt.rgba(0,0,0,1)
                border.width: 1;

                Text {
                    id : _id_text
                    anchors.fill: parent ;
                    text: index ;
                    font.pixelSize: 1024 / 32;
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment :Text.AlignHCenter;
                }

                DropShadow {
                        anchors.fill: parent
                        horizontalOffset: 1
                        verticalOffset: 1
                        radius: 3.0
                        samples: 17
                        color: Qt.rgba(0.3,0.3,0.3,0.3)
                        source: _id_text
                    }

            }
        }
    }

}














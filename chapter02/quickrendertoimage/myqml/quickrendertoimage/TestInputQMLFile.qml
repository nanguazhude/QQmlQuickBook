import QtQuick 2.11
import QtGraphicalEffects 1.0

Rectangle{
    
    width : 1024 ;
    height : 1024 ;
    color : Qt.rgba(0.8,0.8,0.8,0.5) ;

    Grid{
        anchors.fill: parent ;
        columns: 8 ;
        Repeater{
            model: 8 * 8 ;
            delegate: Rectangle{

                width: 1024 / 8 ;
                height: 1024 /8 ;
                color: String(index).indexOf('6')>(-1) ?
                           Qt.rgba(1,0,0,0.3) :
                           Qt.rgba(0,0,0,0.3)
                border.color: String(index).indexOf('6')>(-1) ?
                                  Qt.rgba(0,1,1,1) :
                                  Qt.rgba(0,0,0,1)
                border.width: 2 ;

                Text {
                    id : _id_text
                    anchors.fill: parent ;
                    text: index ;
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment :Text.AlignHCenter;
                    font.strikeout: text.indexOf('7')>(-1);
                    fontSizeMode: Text.Fit;
                    minimumPixelSize: 1024 / 8;
                    font.pixelSize: 1024;
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














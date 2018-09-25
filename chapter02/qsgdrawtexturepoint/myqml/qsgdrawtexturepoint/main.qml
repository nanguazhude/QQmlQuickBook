import QtQuick 2.11
import QtQuick.Window 2.11
import myqml.qsgdrawpoint 1.0

Window {

    visible: true
    width: 640
    height: 480
    title: qsTr("qsgdrawtexturepoint")
    id : _window;
    color: Qt.rgba(0.3,0.35,0.35,1)

    Component{
        id : _dynamic_object_componet
        QuickTexturePoint{
            clip : true
            property int iCount : 0
            property double xRate : 1;
            property double yRate : 1;
            property double zRate : 1;
            pointSize : 128
            pointColor : Qt.rgba( 0.3+Math.random()/1.5,0.3+Math.random()/1.5,0.3+Math.random()/1.5,1 )
            rotation : Math.random() * 180

            Timer{
                interval: 800+Math.random()*300
                repeat: true
                running: true
                onTriggered: {
                    var rotationTmp = parent.rotation;
                    rotationTmp += ((Math.random())>0.5?1:-1)*(Math.random()*5+1);
                    if(rotationTmp>360){    rotationTmp-=360; }
                    else if(rotationTmp<0){ rotationTmp+=360; }
                    parent.rotation = rotationTmp ;
                }
            }

        }
    }

    /*用于调试坐标是否正确*/
    Component{
        id : _draw_border
        Rectangle{
            anchors.fill: parent
            color: Qt.rgba(0,0,0,0)
            border.width: 2
            border.color: Qt.rgba(0,0,0,1)
        }
    }

    Component.onCompleted: {
        
        var numberToImageIndex = function(num){
            if( num < 10 ){  return "00" + num + ".png" ; }
            if( num < 100 ){ return "0"  + num + ".png" ; }
            return num.toString() + ".png"              ;
        }

        for( var i =0; i<128 ; ++i ){
            var obj = _dynamic_object_componet.createObject(_window);
            obj.xRate = 0.8 * (Math.random()-0.5) + 0.5 ;
            obj.yRate = 0.8 * (Math.random()-0.5) + 0.5 ;
            obj.zRate = Math.random();
            obj.y = Qt.binding( function(){ return _window.height * this.yRate - this.width/2  ; } )
            obj.x = Qt.binding( function(){ return _window.width  * this.xRate - this.height/2  ; } )
            obj.z = Qt.binding( function(){ return this.zRate; } )
            obj.iCount = i
            obj.imageIndex = numberToImageIndex( (i%78)/*最多到077.png*/ );
            //_draw_border.createObject(obj)/*用于调试坐标是否正确*/;
        }

    }

}


/*ListView 的代理*/
import QtQuick 2.11
import QtQuick.Controls 1.4
import myqml.animationandstate 1.0

Button {

    id : _id_button
    height: 32 ;
    text : programName ;
    tooltip: qsTr("案例所在文件：") + programSource ;

    function loadSourceFunction(){
        var obj = QmlLocalFileLoader.getSource(_id_button,programSource);
        if(obj){
            obj.show();
        }
    }/*loadSourceFunction*/

    onClicked : {
        loadSourceFunction();
    }

}/*Button*/






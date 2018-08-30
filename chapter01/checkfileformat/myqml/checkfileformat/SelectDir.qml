/*SelectDir.qml*/
import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2

Item {
    Button {
        id : _id_button;
        objectName: "_id_button"
        anchors.centerIn: parent;
        width:  180;
        height: 64;
        text : qsTr("选择文件夹") ;
        flat: false;
        font.pixelSize: 30;
    }
}






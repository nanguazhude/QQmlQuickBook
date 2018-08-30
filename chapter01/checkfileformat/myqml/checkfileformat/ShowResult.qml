/*ShowResult.qml*/
import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls 1.4

TreeView{
    width : parent.width;
    height: parent.height;
    model: rootWindow ;

    TableViewColumn {
        title: qsTr("序列")
        role: "fileName"
        width: parent.width;
    }

}










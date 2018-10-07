import QtQuick 2.11
import QtQuick.Window 2.11
import "main_private" as MainPrivate

DemoBasic {

    id : _id_window;
    title: qsTr("animationandstate")

   MainPrivate.MainListView{
       anchors.fill: parent ;
   }

}/*Window*/














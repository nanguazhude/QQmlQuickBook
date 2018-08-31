/*ShowResult.qml*/
import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.11
import QtQuick.Controls 2.2

ColumnLayout{

    spacing: 2;

    TreeView{

        width : parent.width;
        model: rootWindow ;

        TableViewColumn {
            title: qsTr("序列")
            role: "fileName"
            width: parent.width;
        }

        Layout.preferredWidth: parent.width;
        Layout.fillHeight : true;

    }

    Button{
        width : parent.width;
        Layout.preferredWidth: parent.width;
        Layout.preferredHeight: 28 ;
        text: qsTr("强制加BOM");
        font.pixelSize: 22;
        onClicked : { rootWindow.forceAddBom(); }
    }

}









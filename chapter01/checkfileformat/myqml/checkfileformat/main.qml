/*main.qml*/
import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

Window {
    id : _id_window;
    visible: true;
    width: 640;
    height: 480;
    title: qsTr("checkfileformat");
    color: Qt.rgba(0.6,0.6,0.6,1);
    minimumHeight: 64;
    minimumWidth: 256;
    property bool checkFinished : false ;
    signal toCheckDir();

    FileDialog {
        id: _id_fileDialog
        title: qsTr("请选择一个目录")
        folder: "../../../"
        selectFolder: true;
        onAccepted: {
            rootWindow.checkTheDir( _id_fileDialog.folder ) ;
        }
        onRejected: {
            _id_window.checkFinished = false ;
        }
    }/*~FileDialog*/

    onToCheckDir: { _id_fileDialog.visible = true; }
    Connections{
        target: rootWindow ;
        onFinishedCheckTheDir : {
            _id_window.checkFinished = true;
        }
    }

    Loader{
        id : _id_loader;
        anchors.fill: parent ;
        source : _id_window.checkFinished ? "ShowResult.qml" : "SelectDir.qml";
        onLoaded: {
            if(checkFinished){ return ; }
            var varButtonItem = null;
            for(var varItem in item.children){
                var varThisItem = item.children[varItem];
                if( varThisItem.objectName === "_id_button"){
                    varButtonItem = varThisItem;
                    break;
                }
            }
            if(varButtonItem === null){
                console.log("can not find _id_button")
                return;
            }
            varButtonItem.clicked.connect( function(){
                _id_window.toCheckDir() ;
            } ) ;
        }
    }/*~Loader*/

}/*~Window*/









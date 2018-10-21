
import QtQuick 2.11
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.3

Rectangle{

    id : _id_root
    color: Qt.rgba(0.5,0.5,0.5,1);

    signal runARender(var qmlPath, var savePath)

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent ;

        RowLayout {
        width: parent.width;

            Button {
                Layout.preferredWidth: 128
                id: _id_source_slect_button
                text: qsTr("选择源")
                onClicked: {
                    _id_file_dialog_0.visible=true;
                }
                FileDialog {
                    id: _id_file_dialog_0
                    title: qsTr("选择要渲染的qml文件")
                    selectMultiple: false
                    onAccepted: {
                        _id_source_qml.text = _id_file_dialog_0.fileUrl
                        _id_file_dialog_0.visible=false;
                    }
                     nameFilters: [ "QML file (*.qml)" ]
                }

            }

            TextField {
                readOnly: false
                selectByMouse: true
                Layout.fillWidth:true
                id: _id_source_qml
                text: qsTr(".qml")
            }

        }

        RowLayout {
            width: parent.width;
            Button {
                Layout.preferredWidth: 128
                id: _id_image_save_button
                text: qsTr("保存图像名称")
                onClicked: {
                    _id_file_dialog_1.visible=true;
                }
                FileDialog {
                    id: _id_file_dialog_1
                    title: qsTr("选择渲染qml文件保存路径")
                    selectMultiple: false
                    onAccepted: {
                        _id_image_save_path.text = _id_file_dialog_1.fileUrl
                        _id_file_dialog_0.visible=false;
                    }
                     nameFilters: [ "IMAGE file (*.png)" ]
                }

            }

            TextField {
                readOnly: false
                selectByMouse: true
                Layout.fillWidth:true
                id: _id_image_save_path
                text: qsTr(".png")
            }


        }

        Button {
            Layout.fillWidth:true
            width: parent.width;
            id: _id_run_button
            text: qsTr("执行")
            onClicked: {
                _id_root.runARender(_id_source_qml.text,_id_image_save_path.text)
            }
        }
    }



}/*Rectangle*/














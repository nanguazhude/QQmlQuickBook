/*ListView 的模型*/
import QtQuick 2.11

ListModel {

    /****/
    ListElement {
        programName : "DemoPropertyAnimation" ;
        programSource : "../DemoPropertyAnimation.qml";
    }

    /****/
    ListElement {
        programName : "DemoParallelStateAnimation" ;
        programSource : "../DemoParallelStateAnimation.qml";
    }

    /****/
    ListElement{
        programName : "DemoParallelStateSelectRun"
        programSource : "../DemoParallelStateSelectRun.qml"
    }

    /****/
    ListElement{
        programName : "DemoCircleState"
        programSource : "../DemoCircleState.qml"
    }

    /****/
    ListElement{
        programName : "DemoCircleStatePause"
        programSource : "../DemoCircleStatePause.qml"
    }

    /****/
    ListElement{
        programName : "DemoCircleStateQuit"
        programSource : "../DemoCircleStateQuit.qml"
    }

}


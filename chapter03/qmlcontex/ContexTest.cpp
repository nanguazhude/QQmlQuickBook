#include "ContexTest.hpp"
#include "RootWindow.hpp"
#include <QtQml/QtQml>
#include <QtQuick/QtQuick>
#include <algorithm>

extern void contexTest(RootWindow * argWindow);

inline QObject * findChild(QObject * arg, const QString & name) {
    if (arg->objectName() == name) {
        return arg;
    }
    return arg->findChild<QObject*>(name);
}

template<typename T>
inline bool isSame(std::initializer_list<T * >arg) {
    if (arg.size() < 2) {
        return true;
    }

    auto varPos = arg.begin();
    auto * varFirst = *varPos;
    const auto varEnd = arg.end();

    for (++varPos; varPos != varEnd; ++varPos) {
        if ((*varPos) != (varFirst)) {
            return false;
        }
    }

    return true;

}

void contexTest(RootWindow * argWindow) {

    QQmlEngine * varEngine = argWindow->getEngine();
    auto * varRootObject = qobject_cast<QQuickItem*>(argWindow->getRootObject());
    QQmlContext * varRootContex = argWindow->getRootContext();

    /*默认contex...*/
    {/*0*/

        auto varRect0 = findChild(varRootObject, QStringLiteral("rect0"));
        auto varRect1 = findChild(varRootObject, QStringLiteral("rect1"));
        auto varRect2 = findChild(varRootObject, QStringLiteral("rect2"));

        auto varRect0Contex = qmlContext(varRect0);
        auto varRect1Contex = qmlContext(varRect1);
        auto varRect2Contex = qmlContext(varRect2);

        auto varRect0ParentContx = varRect0Contex->parentContext();
        auto varRect1ParentContx = varRect1Contex->parentContext();
        auto varRect2ParentContx = varRect2Contex->parentContext();

        qDebug() << QStringLiteral("rect0,rect1,rect2 contex is same : ")
            << isSame({ varRect0Contex ,varRect1Contex ,varRect2Contex });
        qDebug() << QStringLiteral("rect0,rect1,rect2 parent contex is same : ")
            << isSame({ varRect0ParentContx ,varRect1ParentContx ,varRect2ParentContx });
        qDebug() << QStringLiteral("rec0,rect1,rect2 contex is root contex : ")
            << isSame({ varRect0Contex ,varRect1Contex ,varRect2Contex,varRootContex });
        qDebug() << QStringLiteral("rect0,rect1,rect2 parent contex is root contex : ")
            << isSame({ varRect0ParentContx ,varRect1ParentContx ,varRect2ParentContx,varRootContex });
        qDebug() << QStringLiteral("root contex object : ") << varRootContex->contextObject();
        qDebug() << QStringLiteral("rect0 contex object : ") << varRect0Contex->contextObject();
        qDebug() << QStringLiteral("rect0 contex object is rect0 : ") 
            << isSame({ varRect0Contex->contextObject() ,varRect0 });
        qDebug() << QStringLiteral("rect0 contex object is rect1 : ") 
            << isSame({ varRect0Contex->contextObject() ,varRect1 });
        qDebug() << QStringLiteral("rect0 contex object is rect2 : ") 
            << isSame({ varRect0Contex->contextObject() ,varRect2 });

    }/*0*/

    /*自定义contex...*/
    {/*1*/
        /*创建一个新的contex*/
        auto varDynamicContex = sstdNew<QQmlContext>(qmlContext(varRootObject));
        {/*设置关联对象属性*/
            auto varAttachedProperty = sstdNew<ContexTest>();
            varAttachedProperty->setYValue(99);
            varAttachedProperty->setParent(varDynamicContex)/*内存管理...*/;
            varDynamicContex->setContextObject(varAttachedProperty);
        }
        varDynamicContex->setContextProperty(QStringLiteral("backgroundColor"), QColor(255, 7, 7, 255));
        QQmlComponent varComponent{ varEngine };
        varComponent.setData(QByteArray(u8R"(/*这里必须是UTF8编码*/
import QtQuick 2.11
Rectangle{
    objectName : "dynamicrect"
    width : 80 ;
    height : 80 ;    
    x : xValue /*read from parent contex*/;
    y : yValue /*read from contextObject propery*/ ;
    color : backgroundColor /*read from contextProperty */ ;
    Text {
        text : parent.objectName
        anchors.centerIn: parent
    }/*Text*/
    Component.onCompleted: { console.log( "dynamic set parent : " + parent ) ; }
}/*Rectangle*/
)"), QUrl(QStringLiteral("GoodLuck.qml")));
        /*开始创建*/
        auto varDynamicObject = qobject_cast<QQuickItem*>(varComponent.beginCreate(varDynamicContex));
        /*动态设置属性*/
        varDynamicContex->setParent(varDynamicObject)/*内存管理...*/;
        varDynamicObject->setParent(varRootObject)/*内存管理...*/;
        varDynamicObject->setParentItem(varRootObject)/*逻辑父类...*/;
        /*结束创建*/
        varComponent.completeCreate();

        qDebug() << "dynamic rect contex is same to create contex : "
            << isSame({ varDynamicContex ,qmlContext(varDynamicObject) });
        qDebug() << "dynamic rect contex is same to create contex : "
            << isSame({ varDynamicContex ,qmlContext(varDynamicObject)->parentContext() });

    }/*2*/

}















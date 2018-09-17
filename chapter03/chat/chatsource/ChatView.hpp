#pragma once

#include <sstd_memory.hpp>
#include <QtQuick/qquickitem.h>
#include <QtGui/qtextdocument.h>

namespace sstd {
    class ChatViewPrivate;
    class ChatView : public QQuickItem {
        Q_OBJECT
            /*此属性应当继承于TextEdit或者TextArea*/
        Q_PROPERTY(QQmlComponent * textEditDelegate READ getTextEditDelegate WRITE setTextEditDelegate NOTIFY textEditDelegateChanged)
            /*此属性用于定位TextFrame，最好是透明的*/
        Q_PROPERTY(QQmlComponent * textFrameDelegate READ getTextFrameDelegate WRITE setTextFrameDelegate NOTIFY textFrameDelegateChanged)
    public:
        ChatView(QQuickItem *parent = nullptr);
        static void registerChat();
    public:
        QTextDocument * getTextDocument() const;
        QQuickItem * getTextEditObject() const;
        QQmlComponent * getTextEditDelegate() const;
        QQmlComponent * getTextFrameDelegate() const;
    public:
        void setTextEditDelegate(QQmlComponent *);
        void setTextFrameDelegate(QQmlComponent *);
        Q_SIGNAL void textEditDelegateChanged();
        Q_SIGNAL void textFrameDelegateChanged();
    private:
        using Super = QQuickItem;
        ChatViewPrivate * thisp = nullptr;
        void $p$LoadTextEditDelegate();
        void $p$LoadTextFrameDelegate();
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(ChatView)
    };
}/*namespace sstd*/




















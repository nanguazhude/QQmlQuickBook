#include "ChatView.hpp"
#include "TextFrameItem.hpp"
#include "ChatViewPrivate.hpp"
#include "ChatDocumentLayout.hpp"
#include "qml_chat_qtextdocumentlayout_p.h"
#include <QtGui>
#include <QtCore>
#include <QtQuick>

namespace sstd {

    ChatViewPrivate::ChatViewPrivate(ChatView *parent) :Super(parent) {

    }

    ChatView::ChatView(QQuickItem *parent) : Super(parent) {
        thisp = sstdNew<ChatViewPrivate>(this);
    }

    void ChatView::registerChat() {
        qmlRegisterType<ChatView>("myqml.chat", 1, 0, "SSTDChat");
    }

    QTextDocument * ChatView::getTextDocument() const {
        return thisp->$m$Document;
    }

    QQuickItem * ChatView::getTextEditObject() const {
        return thisp->$m$TextEdit;
    }

    QQmlComponent * ChatView::getTextEditDelegate() const {
        return thisp->$m$TextEditDelegate;
    }

    QQmlComponent * ChatView::getTextFrameDelegate() const {
        return thisp->$m$TextFrameDelegate;
    }

    void ChatView::setTextEditDelegate(QQmlComponent *arg) {
        if (getTextEditDelegate() == arg) { return; }
        if (thisp->$m$TextEditDelegate) {
            qDebug() << "can not change text edit delegate";
            return;
        }
        thisp->$m$TextEditDelegate = arg;
        $p$LoadTextEditDelegate();
        textEditDelegateChanged();
    }

    void ChatView::setTextFrameDelegate(QQmlComponent *arg) {
        if (getTextFrameDelegate() == arg) { return; }
        if (thisp->$m$TextFrameDelegate) {
            qDebug() << "can not change text frame delegate";
            return;
        }
        thisp->$m$TextFrameDelegate = arg;
        textFrameDelegateChanged();
    }

    void ChatView::$p$LoadTextEditDelegate() {
        if (nullptr == thisp->$m$TextEditDelegate) { return; }
        auto varContext = thisp->$m$TextEditDelegate->creationContext();
        assert(varContext);
        if (thisp->$m$TextEdit) {
            delete thisp->$m$TextEdit;
            thisp->$m$TextEdit = nullptr;
        }
        /*创建TextEdit*/
        thisp->$m$TextEdit =
            sstd_unsafe_cast<QQuickItem *>(thisp->$m$TextEditDelegate->beginCreate(varContext));
        assert(thisp->$m$TextEdit);
        thisp->$m$TextEdit->setParent(this);
        thisp->$m$TextEdit->setParentItem(this);
        thisp->$m$TextEdit->setProperty("textMargin",0);
        thisp->$m$TextEditDelegate->completeCreate();
        {/*设置TextView的Width和Height*/
            QQmlExpression varSetElementWidth(varContext, thisp->$m$TextEdit,
                QStringLiteral(R"(
 width  = Qt.binding( function() { return parent.width  } );
 height = Qt.binding( function() { return parent.height } );
 )"));
            varSetElementWidth.evaluate();
        }
        /*获得文档*/
        auto varQuickTextDocument = thisp->$m$TextEdit->property("textDocument").value<QQuickTextDocument*>();
        assert(varQuickTextDocument);
        thisp->$m$Document = varQuickTextDocument->textDocument();
        assert(thisp->$m$Document);
        /*设置文档布局*/
        auto varLayout = sstdNew<ChatDocumentLayout>(this, thisp->$m$Document);
        thisp->$m$Document->setDocumentLayout(varLayout);
        if constexpr (false) {
            connect(varLayout, &QAbstractTextDocumentLayout::documentSizeChanged,
                this, [this](const QSizeF & arg) {
                this->setHeight(std::max(this->height(), arg.height()));
            });
        }
    }

    void ChatView::$p$LoadTextFrameDelegate() {
        if (nullptr == thisp->$m$TextFrameDelegate) { return; }
        auto varContex = thisp->$m$TextFrameDelegate->creationContext();
        assert(varContex);
    }

    ChatDocumentLayout::ChatDocumentLayout(ChatView * a, QTextDocument *doc) :Super(doc) {
        $m$ChatView = a;
    }

    void ChatView::appendChatFrame(TextFrameItem*arg) {
        assert(arg);
        assert(arg->getTextFrame() == nullptr);

        {
            QTextCursor varTC{ thisp->$m$Document->rootFrame() };
            varTC.movePosition(QTextCursor::End);
        }

    }

}/*namespace sstd*/













#pragma once

#include <sstd_memory.hpp>
#include <ConstructQSurface.hpp>
#include <QtQuick/qquickitem.h>
#include <QtQuick/qsgrendernode.h>

namespace sstd {

    class QuickSimpleTriangleNode :
        public QSGRenderNode,
        public sstd::OpenGLFunctions {
        using Super = QSGRenderNode;
    public:

        QSGRenderNode::RenderingFlags flags() const override;
        QSGRenderNode::StateFlags changedStates() const override;
        QRectF rect() const override;
        void render(const QSGRenderNode::RenderState *state) override;
        void releaseResources() override;
        ~QuickSimpleTriangleNode();
        QuickSimpleTriangleNode(QQuickItem *);
    private:
        QQuickItem * mmm_Item = nullptr;
        GLuint mmm_Program = 0;
        GLuint mmm_Buffer = 0;
        GLuint mmm_VAO = 0;
        void ppp_Clean();
        void ppp_InitProgram(std::string_view,std::string_view);
        void ppp_InitBuffer();
        void ppp_InitVAO();
    private:
        SSTD_MEMORY_DEFINE(QuickSimpleTriangleNode)
    };

    class QuickSimpleTriangle : public QQuickItem {
        Q_OBJECT
    private:
        using Super = QQuickItem;
    public:
        QuickSimpleTriangle(Super * parent = nullptr);
        QSGNode * updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData) override;
    private:
        SSTD_MEMORY_QOBJECT_DEFINE(QuickSimpleTriangle)
    };

}/*namespace sstd*/















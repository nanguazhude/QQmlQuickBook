#pragma once

#include <sstd_memory.hpp>
#include <sstd_RenderPack.hpp>

#include <QWindow>
#include <QMatrix4x4>
#include <QThread>
#include <QWaitCondition>
#include <QMutex>

class QOpenGLContext;
class QOpenGLFramebufferObject;
class QOffscreenSurface;
class QQuickRenderControl;
class QQuickWindow;
class QQmlEngine;
class QQmlComponent;
class QQuickItem;

class CubeRenderer;

class QuickRenderer : public QObject {
    Q_OBJECT

public:
    QuickRenderer();

    void requestInit();
    void requestRender();
    void requestResize();
    void requestStop();

    QWaitCondition *cond() {
        return &m_cond;
    }

    QMutex *mutex() {
        return &m_mutex;
    }

    void setContext(QOpenGLContext *ctx) {
        m_context = ctx;
    }

    void setSurface(QOffscreenSurface *s) {
        m_surface = s;
    }

    void setWindow(QWindow *w) {
        m_window = w;
    }

    void setQuickWindow(QQuickWindow *w) {
        //m_quickWindow = w;
    }

    void setRenderControl(QQuickRenderControl *r) {
        m_renderControl = r;
    }

    void aboutToQuit();

public:
    bool event(QEvent *e) override;
    void init();
    void cleanup();
    void ensureFbo();
    void render(QMutexLocker *lock);

    QWaitCondition m_cond;
    QMutex m_mutex;
    QOpenGLContext *m_context;
    QOffscreenSurface *m_surface;
    QWindow *m_window;
    //QQuickWindow *m_quickWindow;
    QQuickRenderControl *m_renderControl;
    CubeRenderer *m_cubeRenderer;
    QMutex m_quitMutex;
    bool m_quit;

    std::shared_ptr<sstd::RenderPack> mmm_RenderPack;

};

namespace sstd {

    class Window : public QWindow {
        Q_OBJECT

    public:
        Window();
        ~Window();

    protected:
        void exposeEvent(QExposeEvent *e) override;
        void resizeEvent(QResizeEvent *e) override;
        void mousePressEvent(QMouseEvent *e) override;
        void mouseReleaseEvent(QMouseEvent *e) override;
        bool event(QEvent *e) override;

    private slots:
        void run();
        void requestUpdate();
        void polishSyncAndRender();

    private:
        void startQuick(const QUrl &filename);
        void updateSizes();

        QuickRenderer *m_quickRenderer;
        QThread *m_quickRendererThread;

        QOpenGLContext *m_context;
        QOffscreenSurface *m_offscreenSurface;
        QQuickRenderControl *m_renderControl;
        //QQuickWindow *m_quickWindow;
        QQmlEngine *m_qmlEngine;
        QQmlComponent *m_qmlComponent;
        QQuickItem *m_rootItem;
        bool m_quickInitialized;
        bool m_psrRequested;

        std::shared_ptr<sstd::RenderPack> mmm_RenderPack;

    };

}/*namespace sstd*/

 

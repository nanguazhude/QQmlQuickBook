#include "sstd_Render.hpp"
#include <QtCore/qthread.h>
#include <QtCore/qcoreapplication.h>
#include <QtGui/qopenglframebufferobject.h>
#include <QtQuick/qquickwindow.h>
#include <QtQml/QtQml>
#include "sstd_RenderControl.hpp"


namespace sstd {

    namespace {

        inline auto getEventID() {
            const static auto varEventID = static_cast<QEvent::Type>(QEvent::registerEventType());
            return varEventID;
        }

        enum class EventType : std::size_t {
            resize_event = (1 << 1),
            render_event = (1 << 2),
        };

        class Event : public QEvent {
            QPointer<Render> mmm_Render;
            void(*mmm_Destory)(Render *);
        public:
            Event(Render * argRender, void(*argCleanFunction)(Render *) = nullptr) :
                QEvent(getEventID()),
                mmm_Render(argRender),
                mmm_Destory(argCleanFunction) {
            }
            ~Event() {
                auto varRender = mmm_Render.data();
                if (varRender&&mmm_Destory) {
                    mmm_Destory(varRender);
                }
            }

            virtual EventType event_type() const = 0;

        private:
            SSTD_MEMORY_DEFINE(Event)
        };


        class ResizeEvent : public Event {
        public:
            using Event::Event;
            EventType event_type() const override {
                return EventType::resize_event;
            }
        private:
            SSTD_MEMORY_DEFINE(ResizeEvent)
        };

    }

    /*构造渲染环境...*/
    Render::Render(std::shared_ptr<RenderPack> arg) {
        this->setRenderPack(std::move(arg));
        /*******************************/
        /*连接信号槽*/
        connect(this, &Render::windowSizeChanged, this, &Render::ppp_WindowSizeChanged);
        /*******************************/
        /*初始化资源*/
        this->ppp_ConstructAll();
    }

    /*清理渲染环境...*/
    Render::~Render() {
        this->ppp_CleanAll();
    }

    /*构造渲染环境...*/
    void Render::ppp_ConstructAll() {

        if (true == mmm_isInit) {
            return;
        }
        mmm_isInit = true;
        if (true == mmm_isDestory) {
            return;
        }

        this->setRenderSurface(this->getRenderPack()->sourceOffscreenSurface);

        /*创建OpenGL环境*/
        this->setRenderContex(sstdNew<QOpenGLContext>())/*contex in this thread!*/;
        this->getRenderContex()->setFormat(this->getRenderSurface()->format());
        this->getRenderContex()->create()/*创建OpengGL环境*/;
        this->getRenderContex()->makeCurrent(this->getRenderSurface())/*将此环境身为OpenGL当前环境*/;

        assert(dynamic_cast<sstd::RenderControl*>(this->getRenderPack()->sourceRenderConotrl));
        /*创建渲染窗口*/
        this->setRenderControl(static_cast<sstd::RenderControl*>(this->getRenderPack()->sourceRenderConotrl));
        this->setRenderSource(this->getRenderPack()->sourceWindow);
        
        /*创建QQml引擎*/
        this->setRenderSourceEngine(sstdNew<QQmlEngine>());
        if ( this->getRenderSourceEngine()->incubationController() ) {
            this->getRenderSourceEngine()->setIncubationController(this->getRenderSource()->incubationController());
        }

        /*设置RenderControl的线程*/
        this->getRenderControl()->prepareThread(QThread::currentThread());

        connect(this->getRenderControl(), &QQuickRenderControl::renderRequested, this, &Render::renderEventFunction);
        connect(this->getRenderControl(), &QQuickRenderControl::sceneChanged, this, &Render::renderEventFunction);

        /*初始化窗口大小*/
        {
            const QSize varResizeSize{ 128,128 };
            this->setWindowSize(varResizeSize);
            this->resizeEventFunction(varResizeSize);
        }
    }

    /*清理渲染数据...*/
    void Render::ppp_CleanAll() {
        auto varSurface = std::move(mmm_RenderPack);
        if (true == mmm_isDestory) {
            return;
        }
        mmm_isDestory = true;
        if (false == mmm_isInit) {
            return;
        }
        class DestoryLock {
            Render * const thisd;
        public:
            DestoryLock(Render * r) :thisd(r) {
                thisd->getRenderContex()->makeCurrent(thisd->getRenderSurface());
            }
            ~DestoryLock() {
                thisd->renderThreadQuit();
                thisd->getRenderContex()->doneCurrent();
                delete thisd->getRenderContex();
            }
        } varLock{ this };
        if (mmm_Target) {
            delete mmm_Target;
            mmm_Target = nullptr;
        }
        if (mmm_SourceEngine) {
            delete mmm_SourceEngine;
        }
    }

    RenderControl * Render::getRenderControl() const {
        return mmm_RenderControl;
    }

    void Render::setRenderControl(RenderControl *arg) {
        assert(nullptr == mmm_RenderControl);
        mmm_RenderControl = arg;
    }

    QOpenGLContext * Render::getRenderContex() const {
        return mmm_RenderContext;
    }

    void Render::setRenderContex(QOpenGLContext * arg) {
        assert(nullptr == mmm_RenderContext);
        mmm_RenderContext = arg;
    }

    QQuickWindow * Render::getRenderSource() const {
        return mmm_SourceQuickWindow;
    }

    void Render::setRenderSource(QQuickWindow * arg) {
        assert(nullptr == mmm_SourceQuickWindow);
        mmm_SourceQuickWindow = arg;
    }

    QOffscreenSurface * Render::getRenderSurface() const {
        return mmm_RenderSurface;
    }

    void Render::setRenderSurface(QOffscreenSurface * arg) {
        assert(nullptr == mmm_RenderSurface);
        mmm_RenderSurface = arg;
    }

    QQmlEngine * Render::getRenderSourceEngine()const {
        return mmm_SourceEngine;
    }

    void Render::setRenderSourceEngine(QQmlEngine * arg) {
        assert(nullptr == mmm_SourceEngine);
        mmm_SourceEngine = arg;
    }

    void Render::setRenderPack(std::shared_ptr<RenderPack>&& arg) {
        assert(nullptr == mmm_RenderPack.get());
        mmm_RenderPack = std::move(arg);
    }

    RenderPack * Render::getRenderPack() const {
        return mmm_RenderPack.get();
    }

    /**过滤调整窗口大小事件**/
    void Render::ppp_WindowSizeChanged() {
        if (mmm_isInit == false) {/*strange*/
            return;
        }

        assert(QThread::currentThread() == this->thread());
        /*事件队列里面至少有一个resize event了，不必再次抛出*/
        if (mmm_CountWindowSizeEvent > 1) {
            return;
        }

        ++mmm_CountWindowSizeEvent;
        QCoreApplication::postEvent(this, sstdNew<ResizeEvent>(this,
            [](Render * r) {--(r->mmm_CountWindowSizeEvent); }));

    }

    /**调整窗口大小**/
    void Render::resizeEventFunction(const QSize & argWindowSize) {
        assert(QThread::currentThread() == this->thread());

    }

    void Render::ppp_Event(QEvent * arg) {
        auto varEvent = static_cast<Event *>(arg);
        switch (varEvent->event_type()) {
            case EventType::resize_event:return resizeEventFunction(this->getWindowSize());
            case EventType::render_event:return renderEventFunction();
        }
    }

    /**再次绘制**/
    void Render::renderEventFunction() {

        class RenderLock final {
            Render * mmm_Render;
        public:
            RenderLock(Render * arg):mmm_Render(arg) {
            }
            ~RenderLock() {
                mmm_Render->renderChanged(mmm_Render->mmm_RenderResult);
            }
        } varLock{this};

    }

    bool Render::event(QEvent * arg) {
        if (arg->type() == getEventID()) {
            ppp_Event(arg);
            return true;
        }
        return QObject::event(arg);
    }

    void Render::ppp_RenderAgain() {
    }

}/*namespace sstd*/
















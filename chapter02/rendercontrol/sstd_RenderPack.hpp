#pragma once

#include <sstd_memory.hpp>
#include <QtQuick/qquickwindow.h>
#include <QtGui/qoffscreensurface.h>
#include <QtQuick/qquickrendercontrol.h>
#include <ConstructQSurface.hpp>

namespace sstd {

    /*必须在主线程创建的元素*/
    class RenderPack : std::enable_shared_from_this<RenderPack> {
    public:
        QQuickWindow * const sourceWindow/*用于渲染QML的组件*/;
        QQuickRenderControl * const sourceRenderConotrl/*用于控制QQuickWindow的渲染过程*/;
        QOffscreenSurface * const sourceOffscreenSurface/*用于初始化当前线程OpenGL环境*/;
        virtual ~RenderPack() = default;
        RenderPack() :
            sourceWindow(nullptr),
            sourceOffscreenSurface(nullptr),
            sourceRenderConotrl{ nullptr }{}
    protected:
        RenderPack(QQuickWindow * a,
            QOffscreenSurface *b,
            QQuickRenderControl *c) :
            sourceWindow(a),
            sourceOffscreenSurface(b),
            sourceRenderConotrl(c) {
        }
        RenderPack(QOffscreenSurface *b,
            QQuickWindow * a,
            QQuickRenderControl *c) :
            sourceWindow(a),
            sourceOffscreenSurface(b),
            sourceRenderConotrl(c) {
        }
    private:
        SSTD_MEMORY_DEFINE(RenderPack)
    };

    namespace private_sstd {

        class PackRawData {
        public:
            QQuickWindow * sourceWindow;
            QOffscreenSurface * sourceOffscreenSurface;
            QQuickRenderControl * sourceRenderConotrl;
        };

        template<typename T>
        class DefaultRenderPack : public RenderPack {
        public:

            static inline T * createRenderControl() {
                return sstdNew<T>();
            }

            static inline void destoryRenderControl(QQuickRenderControl * arg) {
                delete arg;
            }

            static inline QQuickWindow * createWinow(QQuickRenderControl *renderControl) {
                return sstdNew<QQuickWindow>(renderControl);
            }

            static inline void destoryWindow(QQuickWindow * arg) {
                delete arg;
            }

            static inline QOffscreenSurface * createOffscreenSurface() {
                return sstdNew<QOffscreenSurface>();
            }

            static inline void destoryOffscreenSurface(QOffscreenSurface * arg) {
                delete arg;
            }

            static inline PackRawData create() {
                PackRawData varAns;

                const auto varGLF = sstd::getDefaultOpenGLFormat();
                {/**/
                    varAns.sourceOffscreenSurface = createOffscreenSurface();
                    varAns.sourceOffscreenSurface->setFormat(varGLF);
                    varAns.sourceOffscreenSurface->create();
                }
                {
                    varAns.sourceRenderConotrl = createRenderControl();
                }
                {
                    varAns.sourceWindow = createWinow(varAns.sourceRenderConotrl);
                }
                return varAns;
            }

            DefaultRenderPack() :DefaultRenderPack(create()) {
            }

            DefaultRenderPack(const PackRawData &varAns) :RenderPack(varAns.sourceWindow,
                varAns.sourceOffscreenSurface,
                varAns.sourceRenderConotrl) {

            }

            ~DefaultRenderPack() {
                destoryWindow(this->sourceWindow);
                destoryRenderControl(this->sourceRenderConotrl);
                destoryOffscreenSurface(this->sourceOffscreenSurface);
            }

        };

    }/*private_sstd*/

    template<typename T>
    inline std::shared_ptr<RenderPack> createDefaultRenderPack() {
        return sstd::make_shared<private_sstd::DefaultRenderPack<T>>();
    }

}/*namespace sstd*/





















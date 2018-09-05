#include <sstd_glew.hpp>
#include <sstd_memory.hpp>
#include "Window.hpp"
#include <QtCore/qcoreapplication.h>

template<typename _T_>
class BasicGLIndex {
protected:
    class Data {
    public:
        GLuint const _m_index;
        Data(GLuint x) :_m_index(x) {}
        ~Data() {
            _T_::_p_destory(&_m_index);
        }
    private:
        SSTD_MEMORY_DEFINE(Data)
    };
    std::shared_ptr<Data> _m_data;
    inline GLuint _p_get_data() const {
        if (_m_data) {
            return _m_data->_m_index;
        }
        return 0;
    }
    inline bool _p_to_bool() const {
        if (_m_data) {
            return true;
        }
        else { return false; }
    }
    BasicGLIndex() {}
    BasicGLIndex(GLuint n) :_m_data(sstd::make_shared<Data>(n)) {
    }
};

class GLProgram : private BasicGLIndex<GLProgram> {
private:
    using Super = BasicGLIndex<GLProgram>;
    template<typename T> friend class BasicGLIndex;
    static inline void _p_destory(const GLuint *a) {
        glDeleteProgram(*a);
    }
public:
    GLProgram() = default;
    GLProgram&operator=(const GLProgram&) = default;
    GLProgram&operator=(GLProgram &&) = default;
    GLProgram(const GLProgram&) = default;
    GLProgram(GLProgram &&) = default;
    explicit GLProgram(GLuint a) :Super(a) {}
    explicit operator bool() const { return Super::_p_to_bool(); }
    operator GLuint() const { return Super::_p_get_data(); }
private:
    SSTD_MEMORY_DEFINE(GLProgram)
};

static inline GLProgram getProgram(int argWidth,int argHeight){
    return {};
}

static inline void process_image(const QImage argInput, QImage & argOutPut) {
    GLProgram varProgram = getProgram(argInput.width(), argInput.height());
}

void Window::openglProcessImage(const QImage & argInput,QImage * argOutPut) {
    if (argOutPut == nullptr) { return; }
    if (argInput.width() < 1) { return; }
    if (argInput.height() < 1) { return; }
    /*clean output*/
    *argOutPut = {};
    /*copy input and convert format*/
    process_image(argInput.convertToFormat(QImage::Format_RGBA8888),*argOutPut);
    return;
}

Window::Window(){
    this->setSurfaceType(QWindow::OpenGLSurface);
    this->create();
    $m$OpenGLContex.create();
    $m$OpenGLContex.makeCurrent(this);
}

void Thread::init() {
    __init();
    //$m$Window = $m$ThreadBody->$m$Window;
    //connect(this, &Thread::openglProcessImage,
    //    $m$ThreadBody->$m$Window.get(), &Window::openglProcessImage,
    //    Qt::BlockingQueuedConnection);
}

Thread::Thread() {
    this->moveToThread(qApp->thread());
    connect(this,&QThread::finished,this,&QObject::deleteLater,Qt::QueuedConnection);
    this->start();
    while (false == $m$ValueSet.load()) {}
    connect(this, &Thread::__init, 
        $m$ThreadBody, &ThreadBody::init,
        Qt::QueuedConnection);
}

void Thread::run() {
    ThreadBody varThreadBody;
    $m$ThreadBody = &varThreadBody;
    this->$m$ValueSet.store(true);
    this->exec();
}

void ThreadBody::init() {
    $m$Window = sstd::make_shared<Window>();
}


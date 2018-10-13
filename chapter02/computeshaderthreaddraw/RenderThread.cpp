#include <sstd_glew.hpp>
#include "RenderSize.hpp"
#include "RenderThread.hpp"
#include <QtCore/qstring.h>
#include <QtCore/qdebug.h>
#include <QtGui/qimage.h>
#include <QtCore/qcoreapplication.h>
#include <QtGui/qwindow.h>
#include <QtGui/qoffscreensurface.h>
#include <QtQuick/qquickwindow.h>
#include <QtQuick/qquickrendercontrol.h>
#include <QtGui/qopenglcontext.h>
#include <QtGui/qoffscreensurface.h>
#include <ConstructQSurface.hpp>

extern bool glewInitialize();

sstd::RenderThread::RenderThread(QWindow * arg) : mmm_DrawWindow(arg) {
    assert(qApp);
    assert(QThread::currentThread()==qApp->thread());
    {
        /*thread完成时自删除*/
        this->moveToThread(qApp->thread());
        connect(this, &QThread::finished, this, &QThread::deleteLater);
        connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
            if (this->isRunning()) {
                this->quit();
                return;
            }
            this->deleteLater();
        });
    }

}

void sstd::RenderThread::start(const QString & arg) {
    mmm_ImageFileName = arg;
    Super::start();
}

namespace {

    class Render {
        QOpenGLContext * mmm_OpenGLContex = nullptr;
    public:
        Render(sstd::RenderThread * arg) {
            /*this function will run in any thread*/
            mmm_OpenGLContex = sstdNew<QOpenGLContext>();
            mmm_OpenGLContex->setFormat(arg->getDrawWindow()->requestedFormat());
            mmm_OpenGLContex->create();
            mmm_OpenGLContex->makeCurrent(arg->getDrawWindow());
            glewInitialize();
        }
        ~Render() {
            mmm_OpenGLContex->doneCurrent();
            delete mmm_OpenGLContex;
        }
        SSTD_MEMORY_DEFINE(Render)
    };

}/*namespace*/

namespace {

    FINAL_CLASS_TYPE_ASSIGN(ProgramType1, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(ProgramType2, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(ImageTextureType, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(AtomicCountType, sstd::NumberWrapType<GLuint>);

    inline GLuint buildComputerShader(std::string_view varShaderSource){
        auto varShader = glCreateShader(GL_COMPUTE_SHADER);

        {
            GLint varSL = static_cast<GLint>(varShaderSource.size());
            const char *varS[]{ varShaderSource.data() };
            glShaderSource(varShader, 1, varS, &varSL);
            glCompileShader(varShader);
        }

        if constexpr (false) {
            auto printErrorDetail = [](GLuint e) {
                GLint log_length;
                glGetShaderiv(e, GL_INFO_LOG_LENGTH, &log_length);
                log_length += 16;

                /*获得一段内存，并初始化为0*/
                sstd::string infos_;
                infos_.resize(log_length);

                char * info = infos_.data();
                glGetShaderInfoLog(e, log_length, nullptr, info);
                qDebug() << info;
            };
            printErrorDetail(varShader);
        }

        auto varProgram = glCreateProgram();
        glAttachShader(varProgram, varShader);
        glLinkProgram(varProgram);
        glDeleteShader(varShader);

        return varProgram;
    }

    using PrivateGLRenderData = std::tuple<ProgramType1, ProgramType2> ;
    class GLRenderData : public PrivateGLRenderData {
    public:

        GLRenderData() : PrivateGLRenderData(0,0) {


        }

        ~GLRenderData() {
            glDeleteProgram( std::get<ProgramType1>(*this) );
            glDeleteProgram( std::get<ProgramType2>(*this) );
        }

    };


}/*namespace*/

void sstd::RenderThread::run() try {
      
    /*create a render ... */
    std::unique_ptr<Render> varRender{ sstdNew<Render>(this) };
    GLRenderData varRenderData;
   
    std::get<ProgramType1>(varRenderData) = buildComputerShader(u8R"(
/*计算着色器，用于生成图像*/
#version 450

layout(local_size_x = 1       , 
       local_size_y = 1       ,
       local_size_z = 1    ) in ;

layout(binding = 0,r32f)        uniform image2D  argImageInputOutput  ;
layout(binding = 1,offset = 0 ) uniform atomic_uint argRenderCount    ;

void main(void) {
    ivec2 varPos   = ivec2( gl_WorkGroupID.xy ) ;
    


}

)"sv);

    std::get<ProgramType2>(varRenderData) = buildComputerShader(u8R"(
/*计算着色器，将生成的图像调整到[0-255]*/
#version 450

layout(local_size_x = 1       , 
       local_size_y = 1       ,
       local_size_z = 1    ) in ;

layout(binding = 0,r32f)  uniform image2D  argImageInput  ;
layout(binding = 1,r8ui)  uniform uimage2D argImageOutput ;
layout(binding = 2 )      uniform uint     argRenderMax   ;

void main(void) {
     ivec2 varPos   = ivec2( gl_WorkGroupID.xy          ) ;
     float varColor = imageLoad(argImageInput , varPos  ) ;
     varColor /= argRenderMax  ;
     varColor *= 255           ;
     imageStore( argImageOutput , varPos , int(varColor)) ;
}


)"sv);


    glUseProgram(std::get<ProgramType1>(varRenderData));


    glUseProgram(std::get<ProgramType2>(varRenderData));
    
    
    
    glFinish();

}
catch (...) {
    qDebug() << "unkonw error ! ";
}

sstd::RenderThread::~RenderThread() {
}

/** QQuickRenderControl Example  **/














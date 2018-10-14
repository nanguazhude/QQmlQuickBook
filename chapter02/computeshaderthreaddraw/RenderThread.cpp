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

sstd::RenderThread::RenderThread(RootWindow * arg) : mmm_DrawWindow(arg) {
    assert(qApp);
    assert(QThread::currentThread() == qApp->thread());
    mmm_PixRatio = mmm_DrawWindow->devicePixelRatio();
    mmm_Mutex = mmm_DrawWindow->getMutex();
    /*lock the window not quit before this thread quit ...*/
    mmm_Mutex->addRenderCount();

    {
        /*thread完成时自删除*/
        this->moveToThread(qApp->thread());
        connect(this, &QThread::finished, this, &QThread::deleteLater, Qt::QueuedConnection);
        connect(this, &QThread::finished, this, [this]() {
            /*tell the window can quit ...*/
            mmm_Mutex->subRenderCount();
        }, Qt::DirectConnection);
        connect(qApp, &QCoreApplication::aboutToQuit, this, [this]() {
            if (this->isRunning()) {
                this->quit();
                return;
            }
            this->deleteLater();
        });
    }

}

namespace {

    class Render {
        QOpenGLContext * mmm_OpenGLContex{ nullptr };
        RootWindow * mmm_TargetWindow{ nullptr };
        sstd::RenderThread * mmm_RenderThread{ nullptr };
    public:
        /*this function will run in any thread*/
        Render(sstd::RenderThread * arg) {
            /*共享DrawWindow的opengl contex*/
            mmm_TargetWindow = arg->getDrawWindow();
            mmm_OpenGLContex = sstdNew<QOpenGLContext>();
            mmm_OpenGLContex->setFormat(mmm_TargetWindow->requestedFormat());
            mmm_OpenGLContex->create();
            mmm_OpenGLContex->setShareContext(mmm_TargetWindow->getContex());
            /*将draw window设置为当前绘制窗口*/
            mmm_OpenGLContex->makeCurrent(mmm_TargetWindow);
            glewInitialize();
        }
        ~Render() {
            glFinish();
            if (false == mmm_TargetWindow->getMutex()->isDestory()) {
                mmm_OpenGLContex->swapBuffers(mmm_TargetWindow);
            }
            mmm_OpenGLContex->doneCurrent();
            delete mmm_OpenGLContex;
        }
    public:
        QOpenGLContext * getContex() const {
            return mmm_OpenGLContex;
        }
    public:
        SSTD_MEMORY_DEFINE(Render)
    };

}/*namespace*/

namespace {

    inline GLuint buildVFShader(std::string_view varVShaderSource, std::string_view varFShaderSource) {

        class ShaderFree {
        public:
            std::array<GLuint, 2> data;
            inline ShaderFree() {
                data[0] = 0;
                data[1] = 0;
            }
            inline ~ShaderFree() {
                glDeleteShader(data[1]);
                glDeleteShader(data[0]);
            }
        } varShaders{};

        GLuint * varShader = &(varShaders.data[0]);
        varShader[0] = glCreateShader(GL_VERTEX_SHADER);
        varShader[1] = glCreateShader(GL_FRAGMENT_SHADER);

        {
            const GLchar * sources[] = { varVShaderSource.data(),varFShaderSource.data() };
            GLint lengths[] = {
                (GLint)(varVShaderSource.size()),
                (GLint)(varFShaderSource.size())
            };

            glShaderSource(varShader[0], 1, &sources[0], &lengths[0]);
            glShaderSource(varShader[1], 1, &sources[1], &lengths[1]);

            glCompileShader(varShader[0]);
            glCompileShader(varShader[1]);

        }

        GLuint varProgram = glCreateProgram();
        glAttachShader(varProgram, varShader[0]);
        glAttachShader(varProgram, varShader[1]);
        glLinkProgram(varProgram);

        if constexpr (true) {

            auto printProgramInfo = [](GLuint e) {
                /*获得错误大小*/
                GLint log_length = 0;
                glGetProgramiv(e, GL_INFO_LOG_LENGTH, &log_length);
                log_length += 16;

                /*获得一段内存，并初始化为0*/
                sstd::string infos_;
                infos_.resize(log_length);

                /*获得错误并输出*/
                char * info = infos_.data();
                glGetProgramInfoLog(e, 1024, nullptr, info);
                qDebug() << info;
            };

            printProgramInfo(varProgram);
        }

        return varProgram;

    }

    inline GLuint buildComputerShader(std::string_view varShaderSource) {
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

    FINAL_CLASS_TYPE_ASSIGN(ProgramGetNumberImageType, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(ProgramNumberImageToIndexType, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(ProgramIndexToColorImageType, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(ImageFloatIndexTextureType, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(ImageAtomicMaxValueBufferType, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(AtomicCountType, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(ImageIndex256Type, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(SimpleTextureVAO, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(SimpleTextureVAOBuffer, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(SimpleTextureColorMapBuffer, sstd::NumberWrapType<GLuint>);

    using PrivateGLRenderData = std::tuple<
        ProgramGetNumberImageType,
        ProgramNumberImageToIndexType,
        ProgramIndexToColorImageType,
        ImageFloatIndexTextureType,
        ImageAtomicMaxValueBufferType,
        ImageIndex256Type,
        SimpleTextureVAO,
        SimpleTextureVAOBuffer,
        SimpleTextureColorMapBuffer
    >;
    class GLRenderData final : public PrivateGLRenderData {
    public:

        GLRenderData() : PrivateGLRenderData(0, 0, 0, 0, 0, 0,0,0,0) {
        }

        ~GLRenderData() {
            glDeleteProgram(std::get<ProgramGetNumberImageType>(*this));
            glDeleteProgram(std::get<ProgramNumberImageToIndexType>(*this));
            glDeleteProgram(std::get<ProgramIndexToColorImageType>(*this));
            glDeleteTextures(1, std::get<ImageFloatIndexTextureType>(*this).pointer());
            glDeleteTextures(1, std::get<ImageIndex256Type>(*this).pointer());
            glDeleteBuffers(1, std::get<ImageAtomicMaxValueBufferType>(*this).pointer());
            glDeleteVertexArrays(1,std::get<SimpleTextureVAO>(*this).pointer());
            glDeleteBuffers(1,std::get<SimpleTextureVAOBuffer>(*this).pointer());
            glDeleteBuffers(1,std::get<SimpleTextureColorMapBuffer>(*this).pointer());
        }

    };


}/*namespace*/

void sstd::RenderThread::run() try {

    /*the last render just return*/
    if (mmm_Mutex->isDestory()) {
        return;
    }

    /*create a render ... */
    std::unique_ptr<Render> varRender{ sstdNew<Render>(this) };

    GLRenderData varRenderData;

    /*开始opengl debug 调试*/
    gl_debug_function_lock();

    const auto varFBOIndex = varRender->getContex()->defaultFramebufferObject();

    const auto varFBOWidth = static_cast<int>(mmm_PixRatio * getRenderWidth());
    const auto varFBOHeight = static_cast<int>(mmm_PixRatio * getRenderHeight());

    if (varFBOWidth < 1) {
        return;
    }

    if (varFBOHeight < 1) {
        return;
    }

    /*TODO : set target fbo*/
    glViewport(0, 0, varFBOWidth, varFBOHeight);

    std::array<GLfloat, 4> mmm_ClearColor{ 1.0f,1.0f, 1.0f, 1.0f };
    std::array<GLfloat, 1> mmm_ClearDepth{ 0.0f };
    /*清空颜色缓冲区和深度缓冲区*/
    glClearNamedFramebufferfv(varFBOIndex, GL_COLOR, 0/*draw buffer*/, mmm_ClearColor.data());
    glClearNamedFramebufferfv(varFBOIndex, GL_DEPTH, 0/*draw buffer*/, mmm_ClearDepth.data());

    std::get<ProgramGetNumberImageType>(varRenderData) = buildComputerShader(u8R"(
/*计算着色器，用于生成图像*/
#version 460

layout(local_size_x = 1       , 
       local_size_y = 1       ,
       local_size_z = 1    ) in ;

layout(binding =  0, r32f)        uniform writeonly image2D argImageOutput  ;
layout(binding =  1, offset = 0 ) uniform atomic_uint argRenderMax          ;
layout(location = 2)              uniform vec2 C = vec2( 1 , 1)             ;
layout(location = 3)              uniform uint varMaxValue = (1024*1024)    ;

void main(void) {

    ivec2  varPos   = ivec2( gl_WorkGroupID.xy ) ;
    uint   varCount = 0 ;
    
    vec2 Z = gl_WorkGroupID.xy;
    vec2 Z_squared; 
    for( ;  varCount < varMaxValue ; ++varCount ){
        Z_squared.x = Z.x * Z.x - Z.y * Z.y;
        Z_squared.y = 2.0 * Z.x * Z.y;
        Z = Z_squared + C;
        if( Z.x > 2 ){ break ; }
        if( Z.y > 2 ){ break ; }
    }

    atomicCounterMax( argRenderMax , varCount );
    imageStore( argImageOutput , varPos , vec4(varCount,0,0,1) ) ;

}

/**
https://www.khronos.org/opengl/wiki/Atomic_Counter
**/

)"sv);

    std::get<ProgramNumberImageToIndexType>(varRenderData) = buildComputerShader(u8R"(
/*计算着色器，将值缩放到[0-255]*/
#version 450

layout(local_size_x = 1       , 
       local_size_y = 1       ,
       local_size_z = 1    ) in ;

layout(binding = 0,r32f)  uniform readonly  image2D  argImageInput  ;
layout(binding = 1,r8ui)  uniform writeonly uimage2D argImageOutput ;
layout(location = 2 )     uniform uint      argRenderMax            ;

void main(void) {
     ivec2 varPos   = ivec2( gl_WorkGroupID.xy          )   ;
     float varColor = imageLoad(argImageInput , varPos  ).r ;
     if(argRenderMax>0){
        varColor /= argRenderMax  ;
        varColor *= 255           ;
     }else{
        varColor = 0              ;
     }
     imageStore( argImageOutput , varPos , uvec4( clamp( int(varColor) , 0 , 255 ) ,0,0,1) ) ;
}

)"sv);

    std::get<ProgramNumberImageToIndexType>(varRenderData) = buildVFShader(
        u8R"(
/*简单顶点着色器，用于渲染一个图片*/
#version 450

layout( location = 0 ) in vec4 argPosition;
layout( location = 1 ) in vec4 argTexturePos;
out vec4 ioTexturePos ;
void main(){
    ioTexturePos = argTexturePos ;
    gl_Position = argPosition    ;
}

)"sv,
u8R"(
/*简单片段着色器，用于给索引图片着色*/
#version 450

in vec4 ioTexturePos         ;
out vec4 outColor            ;
uniform sampler2D argTexture ;

void main(){
    float varColorInputIndex = texture2D( argTexture , ioTexturePos.xy ).r ;
    float x = varColorInputIndex/256;
    outColor = vec4(x,x,x,1);
}

)"sv);

    /*创建Texture*/
    glCreateTextures(GL_TEXTURE_2D, 1, std::get<ImageFloatIndexTextureType>(varRenderData).pointer());
    /*分配内存*/
    /*https://www.khronos.org/opengl/wiki/Texture_Storage */
    glTextureStorage2D(std::get<ImageFloatIndexTextureType>(varRenderData), 1, GL_R32F, varFBOWidth, varFBOHeight);

    /*创建Texture*/
    glCreateTextures(GL_TEXTURE_2D, 1, std::get<ImageIndex256Type>(varRenderData).pointer());
    glTextureStorage2D(std::get<ImageIndex256Type>(varRenderData), 1, GL_R8UI, varFBOWidth, varFBOHeight);

    /*创建原子计数器*/
    glCreateBuffers(1, std::get<ImageAtomicMaxValueBufferType>(varRenderData).pointer());
    {
        const std::array<GLuint, 4> varTmpInitData{ 0,0,0,0 };
        glNamedBufferStorage(std::get<ImageAtomicMaxValueBufferType>(varRenderData),
            sizeof(varTmpInitData),
            varTmpInitData.data(),
            GL_DYNAMIC_STORAGE_BIT);
    }

    /*生成图像*/
    {
        glUseProgram(std::get<ProgramGetNumberImageType>(varRenderData));
        glBindImageTexture(0, std::get<ImageFloatIndexTextureType>(varRenderData), 0, false, 0, GL_WRITE_ONLY, GL_R32F);
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, std::get<ImageAtomicMaxValueBufferType>(varRenderData));
        {/*set C*/
            std::array<GLfloat, 2> varC{ 1.0f,1.0f };
            glUniform2fv(2, 1, varC.data());
        }
        {/*set MaxValue*/
            std::array<GLuint, 1> varMaxValue{ 1024 * 1024 };
            glUniform1ui(3, varMaxValue[0]);
        }
        glDispatchCompute(varFBOWidth, varFBOHeight, 1);
    }

    /*缩放到[0-255]*/
    {
        glUseProgram(std::get<ProgramNumberImageToIndexType>(varRenderData));
        glBindImageTexture(0, std::get<ImageFloatIndexTextureType>(varRenderData), 0, false, 0, GL_READ_ONLY, GL_R32F);
        glBindImageTexture(1, std::get<ImageIndex256Type>(varRenderData), 0, false, 0, GL_WRITE_ONLY, GL_R8UI);
        glBindBufferBase(GL_UNIFORM_BUFFER, 2, std::get<ImageAtomicMaxValueBufferType>(varRenderData));
        glDispatchCompute(varFBOWidth, varFBOHeight, 1);
    }

    /*着色*/
    {
        glUseProgram(std::get<ProgramIndexToColorImageType>(varRenderData));
        
    }


} catch (...) {
    qDebug() << "unkonw error ! ";
}

sstd::RenderThread::~RenderThread() {
}

/** QQuickRenderControl Example  **/
//曼德布罗特集合分形(Mandelbort Set Fractal) 使用复数函数公式F(z) = z^2 + c其中












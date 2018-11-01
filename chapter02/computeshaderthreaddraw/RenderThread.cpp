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

        if constexpr (false) {

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
    FINAL_CLASS_TYPE_ASSIGN(ImageIndex256Type, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(SimpleTextureVAO, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(SimpleTextureVAOBuffer, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(SimpleTextureVAOIndexBuffer, sstd::NumberWrapType<GLuint>);
    FINAL_CLASS_TYPE_ASSIGN(SimpleTextureMappedColorBuffer, sstd::NumberWrapType<GLuint>);

    using PrivateGLRenderData = std::tuple<
        ProgramGetNumberImageType,
        ProgramNumberImageToIndexType,
        ProgramIndexToColorImageType,
        ImageFloatIndexTextureType,
        ImageAtomicMaxValueBufferType,
        ImageIndex256Type,
        SimpleTextureVAO,
        SimpleTextureVAOBuffer,
        SimpleTextureVAOIndexBuffer,
        SimpleTextureMappedColorBuffer
    >;
    class GLRenderData final : public PrivateGLRenderData {
    public:

        GLRenderData() : PrivateGLRenderData(0, 0, 0, 0, 0, 0, 0, 0, 0, 0) {
        }

        ~GLRenderData() {
            glDeleteProgram(std::get<ProgramGetNumberImageType>(*this));
            glDeleteProgram(std::get<ProgramNumberImageToIndexType>(*this));
            glDeleteProgram(std::get<ProgramIndexToColorImageType>(*this));
            glDeleteTextures(1, std::get<ImageFloatIndexTextureType>(*this).pointer());
            glDeleteTextures(1, std::get<ImageIndex256Type>(*this).pointer());
            glDeleteBuffers(1, std::get<ImageAtomicMaxValueBufferType>(*this).pointer());
            glDeleteVertexArrays(1, std::get<SimpleTextureVAO>(*this).pointer());
            glDeleteBuffers(1, std::get<SimpleTextureVAOBuffer>(*this).pointer());
            glDeleteBuffers(1, std::get<SimpleTextureVAOIndexBuffer>(*this).pointer());
            glDeleteBuffers(1, std::get<SimpleTextureMappedColorBuffer>(*this).pointer());
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

    glBindFramebuffer(GL_FRAMEBUFFER, varFBOIndex);
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
layout(location = 2)              uniform vec2 C1 = vec2( 0.5 , 0.5)        ;
layout(location = 3)              uniform int varMaxValue = (1024*1024)    ;

void main(void) {

    ivec2  varPos   = ivec2( gl_WorkGroupID.xy ) ;
    int   varCount = 0 ;
    
    vec2 Z = vec2(0);
    vec2 C = 2 * gl_WorkGroupID.xy ;
    C.x /= gl_NumWorkGroups.x;
    C.y /= gl_NumWorkGroups.y;
    C.x -= C1.x              ;
    C.y -= C1.y              ;
    vec2 Z_squared; 
    for( ;  varCount < varMaxValue ; ++varCount ){
        Z_squared.x = Z.x * Z.x - Z.y * Z.y;
        Z_squared.y = 2.0 * Z.x * Z.y      ;
        Z = Z_squared + C                  ;
        if( length(Z) > 1.9999999999 ){ break; }
    }

    if( varCount >= varMaxValue ){ varCount = 0 ; }
    atomicCounterMax( argRenderMax , varCount );
    imageStore( argImageOutput , varPos , vec4(varCount,1,1,1) ) ;

}


)"sv);

    std::get<ProgramNumberImageToIndexType>(varRenderData) = buildComputerShader(u8R"(
/*计算着色器，将值缩放到[0-255]*/
#version 450

layout(local_size_x = 1       , 
       local_size_y = 1       ,
       local_size_z = 1    ) in ;

layout (binding = 0,r32f)  uniform readonly  image2D  argImageInput  ;
layout (binding = 1,r8ui)  uniform writeonly uimage2D argImageOutput ;
/*glUniformBlockBinding*/
layout (std140 ,binding = 2 ) uniform InputUniformBlock {
    int argRenderMax ;
}  ;
 

void main(void) {
     ivec2 varPos   = ivec2( gl_WorkGroupID.xy          )   ;
     float varColor = imageLoad(argImageInput , varPos  ).r ;
     if( argRenderMax>0 ){
        varColor /= argRenderMax  ;
        varColor *= 255          ;
     }else{
        varColor = 0              ;
     }
     imageStore( argImageOutput , varPos , uvec4( clamp( int(varColor) , 0 , 255) , 0 , 0 , 1 ) ) ;
}

)"sv);

    std::get<ProgramIndexToColorImageType >(varRenderData) = buildVFShader(
        u8R"(
/*简单顶点着色器，用于渲染一个图片*/
#version 450

layout( location = 0 ) in vec4 argPosition  ;
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

layout ( std140 ,binding = 2 ) uniform ColorMapBlock {
    vec4 argMappedColor[256];
};

in vec4 ioTexturePos                            ;
out vec4 outColor                               ;
layout (binding=1) uniform sampler2D argTexture ;

void main(){
    float varColorInputIndex = texture2D( argTexture , ioTexturePos.xy ).r ;
    varColorInputIndex = sqrt(varColorInputIndex);
    outColor = argMappedColor[ clamp( int( 512 * varColorInputIndex ) , 0 , 255 ) ] ;
}

)"sv);

    if constexpr (true) {
        /*创建Texture*/
        glCreateTextures(GL_TEXTURE_2D, 1, std::get<ImageFloatIndexTextureType>(varRenderData).pointer());
        /*分配内存*/
        glTextureStorage2D(std::get<ImageFloatIndexTextureType>(varRenderData), 1, GL_R32F, varFBOWidth, varFBOHeight);

        /*创建Texture*/
        glCreateTextures(GL_TEXTURE_2D, 1, std::get<ImageIndex256Type>(varRenderData).pointer());
        glTextureStorage2D(std::get<ImageIndex256Type>(varRenderData), 1, GL_R8, varFBOWidth, varFBOHeight);

        /*创建原子计数器*/
        glCreateBuffers(1, std::get<ImageAtomicMaxValueBufferType>(varRenderData).pointer());
        {
            const std::array<GLuint, 4> varTmpInitData{ 0,0,0,0 };
            glNamedBufferStorage(std::get<ImageAtomicMaxValueBufferType>(varRenderData),
                sizeof(varTmpInitData),
                varTmpInitData.data(),
                GL_DYNAMIC_STORAGE_BIT);
        }
    }

    /*生成图像*/
    if constexpr (true) {
        glUseProgram(std::get<ProgramGetNumberImageType>(varRenderData));
        glBindImageTexture(0, std::get<ImageFloatIndexTextureType>(varRenderData), 0, false, 0, GL_WRITE_ONLY, GL_R32F);
        glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, std::get<ImageAtomicMaxValueBufferType>(varRenderData));
        {/*set C*/
            const std::array<GLfloat, 2> varC{ 1.5,1 };
            glUniform2fv(2, 1, varC.data());
        }
        {/*set MaxValue*/
            const std::array<int, 1> varMaxValue{ 512 };
            glUniform1i(3, varMaxValue[0]);
        }
        glDispatchCompute(varFBOWidth, varFBOHeight, 1);
    }

    /*用于调试，返回生成的图像*/
    if constexpr (false) {
        glFinish();
        sstd::vector<GLfloat> varData;
        varData.resize(varFBOHeight * varFBOWidth);
        glGetTextureImage(std::get<ImageFloatIndexTextureType>(varRenderData),
            0,
            GL_RED, GL_FLOAT,
            static_cast<int>(varData.size() * sizeof(GLfloat)),
            varData.data());
        qDebug() << std::any_of(varData.begin(), varData.end(), [](const auto & i) {return i != 0; });
    }

    /*缩放到[0-255]*/
    if constexpr (true) {
        glUseProgram(std::get<ProgramNumberImageToIndexType>(varRenderData));
        glBindImageTexture(0, std::get<ImageFloatIndexTextureType>(varRenderData), 0, false, 0, GL_READ_ONLY, GL_R32F);
        glBindImageTexture(1, std::get<ImageIndex256Type>(varRenderData), 0, false, 0, GL_WRITE_ONLY, GL_R8UI);
        glBindBuffer(GL_UNIFORM_BUFFER, std::get<ImageAtomicMaxValueBufferType>(varRenderData));
        glBindBufferBase(GL_UNIFORM_BUFFER, 2, std::get<ImageAtomicMaxValueBufferType>(varRenderData));
        glDispatchCompute(varFBOWidth, varFBOHeight, 1);
    }

    /*用于调试，返回生成的图像*/
    if constexpr (false) {
        glFinish();
        sstd::vector<unsigned char> varData;
        varData.resize((1 + varFBOHeight)* (1 + varFBOWidth), char(0));
        glGetTextureImage(std::get<ImageIndex256Type>(varRenderData),
            0,
            GL_RED, GL_UNSIGNED_BYTE,
            static_cast<int>(varData.size() * sizeof(unsigned char)),
            varData.data());
        qDebug() << std::any_of(varData.begin(), varData.end(), [](const auto & i) {return i != 0; });
    }

    /*着色*/
    {
        glUseProgram(std::get<ProgramIndexToColorImageType>(varRenderData));
        glBindTexture(GL_TEXTURE_2D, std::get<ImageIndex256Type>(varRenderData));
        glActiveTexture(GL_TEXTURE0 + 1);
        glBindTextureUnit(1, std::get<ImageIndex256Type>(varRenderData));
        glCreateVertexArrays(1, std::get<SimpleTextureVAO>(varRenderData).pointer());
        glBindVertexArray(std::get<SimpleTextureVAO>(varRenderData));
        glCreateBuffers(1, std::get<SimpleTextureVAOBuffer>(varRenderData).pointer());
        glCreateBuffers(1, std::get<SimpleTextureVAOIndexBuffer>(varRenderData).pointer());
        class DataRow {
        public:
            GLfloat x, y, z, w;
            GLfloat s, t, p, q;
        };
        constexpr const static std::array<DataRow, 4> varVAOData{ DataRow {-1,1,0,1,/**/0,1,0,1},
        DataRow {-1,-1,0,1,/**/0,0,0,1},
        DataRow {1,-1,0,1,/**/1,0,0,1},
        DataRow {1,1,0,1,/**/1,1,0,1}
        };
        constexpr const static std::array<std::uint16_t, 6> varVAOIndex{
            3,2,1,
            3,1,0
        };

        glNamedBufferData(std::get<SimpleTextureVAOBuffer>(varRenderData), sizeof(varVAOData), varVAOData.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexArrayVertexBuffer(std::get<SimpleTextureVAO>(varRenderData), 0, std::get<SimpleTextureVAOBuffer>(varRenderData), 0, sizeof(DataRow));
        glVertexArrayAttribFormat(std::get<SimpleTextureVAO>(varRenderData), 0, 4, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(std::get<SimpleTextureVAO>(varRenderData), 0, 0);

        glEnableVertexAttribArray(1);
        glVertexArrayVertexBuffer(std::get<SimpleTextureVAO>(varRenderData), 1, std::get<SimpleTextureVAOBuffer>(varRenderData), (sizeof(DataRow) >> 1), sizeof(DataRow));
        glVertexArrayAttribFormat(std::get<SimpleTextureVAO>(varRenderData), 1, 4, GL_FLOAT, false, 0);
        glVertexArrayAttribBinding(std::get<SimpleTextureVAO>(varRenderData), 1, 1);

        glNamedBufferData(std::get<SimpleTextureVAOIndexBuffer>(varRenderData), sizeof(varVAOIndex), varVAOIndex.data(), GL_STATIC_DRAW);
        glVertexArrayElementBuffer(std::get<SimpleTextureVAO>(varRenderData), std::get<SimpleTextureVAOIndexBuffer>(varRenderData));

        glCreateBuffers(1, std::get<SimpleTextureMappedColorBuffer>(varRenderData).pointer());
        /*Matlab: colormap(parula(256)) colormap */
        constexpr const static std::array< std::array<GLfloat, 4>, 256 > varMappedBufferData{
#include "RenderThreadColorMap"
        };
        glNamedBufferData(std::get<SimpleTextureMappedColorBuffer>(varRenderData), sizeof(varMappedBufferData), varMappedBufferData.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, std::get<SimpleTextureMappedColorBuffer>(varRenderData));
        glBindBufferBase(GL_UNIFORM_BUFFER, 2, std::get<SimpleTextureMappedColorBuffer>(varRenderData));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    }


} catch (...) {
    qDebug() << "unkonw error ! ";
}

sstd::RenderThread::~RenderThread() {
}

/** QQuickRenderControl Example  **/
//曼德布罗特集合分形(Mandelbort Set Fractal) 使用复数函数公式F(z) = z^2 + c其中












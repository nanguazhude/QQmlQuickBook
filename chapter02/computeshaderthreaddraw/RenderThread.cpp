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
    FINAL_CLASS_TYPE_ASSIGN(AtomicCountType, sstd::NumberWrapType<GLuint>);
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

layout ( std140 ,binding = 2 ) uniform ExampleBlock {
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
std::array<GLfloat, 4>{0.2422f,0.1504f,0.6603f,1.0f },
std::array<GLfloat, 4>{0.2444f,0.1534f,0.6728f,1.0f},
std::array<GLfloat, 4>{0.2464f,0.1569f,0.6847f,1.0f},
std::array<GLfloat, 4>{0.2484f,0.1607f,0.6961f,1.0f},
std::array<GLfloat, 4>{0.2503f,0.1648f,0.7071f,1.0f},
std::array<GLfloat, 4>{0.2522f,0.1689f,0.7179f,1.0f},
std::array<GLfloat, 4>{0.2540f,0.1732f,0.7286f,1.0f},
std::array<GLfloat, 4>{0.2558f,0.1773f,0.7393f,1.0f},
std::array<GLfloat, 4>{0.2576f,0.1814f,0.7501f,1.0f},
std::array<GLfloat, 4>{0.2594f,0.1854f,0.7610f,1.0f},
std::array<GLfloat, 4>{0.2611f,0.1893f,0.7719f,1.0f},
std::array<GLfloat, 4>{0.2628f,0.1932f,0.7828f,1.0f},
std::array<GLfloat, 4>{0.2645f,0.1972f,0.7937f,1.0f},
std::array<GLfloat, 4>{0.2661f,0.2011f,0.8043f,1.0f},
std::array<GLfloat, 4>{0.2676f,0.2052f,0.8148f,1.0f},
std::array<GLfloat, 4>{0.2691f,0.2094f,0.8249f,1.0f},
std::array<GLfloat, 4>{0.2704f,0.2138f,0.8346f,1.0f},
std::array<GLfloat, 4>{0.2717f,0.2184f,0.8439f,1.0f},
std::array<GLfloat, 4>{0.2729f,0.2231f,0.8528f,1.0f},
std::array<GLfloat, 4>{0.2740f,0.2280f,0.8612f,1.0f},
std::array<GLfloat, 4>{0.2749f,0.2330f,0.8692f,1.0f},
std::array<GLfloat, 4>{0.2758f,0.2382f,0.8767f,1.0f},
std::array<GLfloat, 4>{0.2766f,0.2435f,0.8840f,1.0f},
std::array<GLfloat, 4>{0.2774f,0.2489f,0.8908f,1.0f},
std::array<GLfloat, 4>{0.2781f,0.2543f,0.8973f,1.0f},
std::array<GLfloat, 4>{0.2788f,0.2598f,0.9035f,1.0f},
std::array<GLfloat, 4>{0.2794f,0.2653f,0.9094f,1.0f},
std::array<GLfloat, 4>{0.2798f,0.2708f,0.9150f,1.0f},
std::array<GLfloat, 4>{0.2802f,0.2764f,0.9204f,1.0f},
std::array<GLfloat, 4>{0.2806f,0.2819f,0.9255f,1.0f},
std::array<GLfloat, 4>{0.2809f,0.2875f,0.9305f,1.0f},
std::array<GLfloat, 4>{0.2811f,0.2930f,0.9352f,1.0f},
std::array<GLfloat, 4>{0.2813f,0.2985f,0.9397f,1.0f},
std::array<GLfloat, 4>{0.2814f,0.3040f,0.9441f,1.0f},
std::array<GLfloat, 4>{0.2814f,0.3095f,0.9483f,1.0f},
std::array<GLfloat, 4>{0.2813f,0.3150f,0.9524f,1.0f},
std::array<GLfloat, 4>{0.2811f,0.3204f,0.9563f,1.0f},
std::array<GLfloat, 4>{0.2809f,0.3259f,0.9600f,1.0f},
std::array<GLfloat, 4>{0.2807f,0.3313f,0.9636f,1.0f},
std::array<GLfloat, 4>{0.2803f,0.3367f,0.9670f,1.0f},
std::array<GLfloat, 4>{0.2798f,0.3421f,0.9702f,1.0f},
std::array<GLfloat, 4>{0.2791f,0.3475f,0.9733f,1.0f},
std::array<GLfloat, 4>{0.2784f,0.3529f,0.9763f,1.0f},
std::array<GLfloat, 4>{0.2776f,0.3583f,0.9791f,1.0f},
std::array<GLfloat, 4>{0.2766f,0.3638f,0.9817f,1.0f},
std::array<GLfloat, 4>{0.2754f,0.3693f,0.9840f,1.0f},
std::array<GLfloat, 4>{0.2741f,0.3748f,0.9862f,1.0f},
std::array<GLfloat, 4>{0.2726f,0.3804f,0.9881f,1.0f},
std::array<GLfloat, 4>{0.2710f,0.3860f,0.9898f,1.0f},
std::array<GLfloat, 4>{0.2691f,0.3916f,0.9912f,1.0f},
std::array<GLfloat, 4>{0.2670f,0.3973f,0.9924f,1.0f},
std::array<GLfloat, 4>{0.2647f,0.4030f,0.9935f,1.0f},
std::array<GLfloat, 4>{0.2621f,0.4088f,0.9946f,1.0f},
std::array<GLfloat, 4>{0.2591f,0.4145f,0.9955f,1.0f},
std::array<GLfloat, 4>{0.2556f,0.4203f,0.9965f,1.0f},
std::array<GLfloat, 4>{0.2517f,0.4261f,0.9974f,1.0f},
std::array<GLfloat, 4>{0.2473f,0.4319f,0.9983f,1.0f},
std::array<GLfloat, 4>{0.2424f,0.4378f,0.9991f,1.0f},
std::array<GLfloat, 4>{0.2369f,0.4437f,0.9996f,1.0f},
std::array<GLfloat, 4>{0.2311f,0.4497f,0.9995f,1.0f},
std::array<GLfloat, 4>{0.2250f,0.4559f,0.9985f,1.0f},
std::array<GLfloat, 4>{0.2189f,0.4620f,0.9968f,1.0f},
std::array<GLfloat, 4>{0.2128f,0.4682f,0.9948f,1.0f},
std::array<GLfloat, 4>{0.2066f,0.4743f,0.9926f,1.0f},
std::array<GLfloat, 4>{0.2006f,0.4803f,0.9906f,1.0f},
std::array<GLfloat, 4>{0.1950f,0.4861f,0.9887f,1.0f},
std::array<GLfloat, 4>{0.1903f,0.4919f,0.9867f,1.0f},
std::array<GLfloat, 4>{0.1869f,0.4975f,0.9844f,1.0f},
std::array<GLfloat, 4>{0.1847f,0.5030f,0.9819f,1.0f},
std::array<GLfloat, 4>{0.1831f,0.5084f,0.9793f,1.0f},
std::array<GLfloat, 4>{0.1818f,0.5138f,0.9766f,1.0f},
std::array<GLfloat, 4>{0.1806f,0.5191f,0.9738f,1.0f},
std::array<GLfloat, 4>{0.1795f,0.5244f,0.9709f,1.0f},
std::array<GLfloat, 4>{0.1785f,0.5296f,0.9677f,1.0f},
std::array<GLfloat, 4>{0.1778f,0.5349f,0.9641f,1.0f},
std::array<GLfloat, 4>{0.1773f,0.5401f,0.9602f,1.0f},
std::array<GLfloat, 4>{0.1768f,0.5452f,0.9560f,1.0f},
std::array<GLfloat, 4>{0.1764f,0.5504f,0.9516f,1.0f},
std::array<GLfloat, 4>{0.1755f,0.5554f,0.9473f,1.0f},
std::array<GLfloat, 4>{0.1740f,0.5605f,0.9432f,1.0f},
std::array<GLfloat, 4>{0.1716f,0.5655f,0.9393f,1.0f},
std::array<GLfloat, 4>{0.1686f,0.5705f,0.9357f,1.0f},
std::array<GLfloat, 4>{0.1649f,0.5755f,0.9323f,1.0f},
std::array<GLfloat, 4>{0.1610f,0.5805f,0.9289f,1.0f},
std::array<GLfloat, 4>{0.1573f,0.5854f,0.9254f,1.0f},
std::array<GLfloat, 4>{0.1540f,0.5902f,0.9218f,1.0f},
std::array<GLfloat, 4>{0.1513f,0.5950f,0.9182f,1.0f},
std::array<GLfloat, 4>{0.1492f,0.5997f,0.9147f,1.0f},
std::array<GLfloat, 4>{0.1475f,0.6043f,0.9113f,1.0f},
std::array<GLfloat, 4>{0.1461f,0.6089f,0.9080f,1.0f},
std::array<GLfloat, 4>{0.1446f,0.6135f,0.9050f,1.0f},
std::array<GLfloat, 4>{0.1429f,0.6180f,0.9022f,1.0f},
std::array<GLfloat, 4>{0.1408f,0.6226f,0.8998f,1.0f},
std::array<GLfloat, 4>{0.1383f,0.6272f,0.8975f,1.0f},
std::array<GLfloat, 4>{0.1354f,0.6317f,0.8953f,1.0f},
std::array<GLfloat, 4>{0.1321f,0.6363f,0.8932f,1.0f},
std::array<GLfloat, 4>{0.1288f,0.6408f,0.8910f,1.0f},
std::array<GLfloat, 4>{0.1253f,0.6453f,0.8887f,1.0f},
std::array<GLfloat, 4>{0.1219f,0.6497f,0.8862f,1.0f},
std::array<GLfloat, 4>{0.1185f,0.6541f,0.8834f,1.0f},
std::array<GLfloat, 4>{0.1152f,0.6584f,0.8804f,1.0f},
std::array<GLfloat, 4>{0.1119f,0.6627f,0.8770f,1.0f},
std::array<GLfloat, 4>{0.1085f,0.6669f,0.8734f,1.0f},
std::array<GLfloat, 4>{0.1048f,0.6710f,0.8695f,1.0f},
std::array<GLfloat, 4>{0.1009f,0.6750f,0.8653f,1.0f},
std::array<GLfloat, 4>{0.0964f,0.6789f,0.8609f,1.0f},
std::array<GLfloat, 4>{0.0914f,0.6828f,0.8562f,1.0f},
std::array<GLfloat, 4>{0.0855f,0.6865f,0.8513f,1.0f},
std::array<GLfloat, 4>{0.0789f,0.6902f,0.8462f,1.0f},
std::array<GLfloat, 4>{0.0713f,0.6938f,0.8409f,1.0f},
std::array<GLfloat, 4>{0.0628f,0.6972f,0.8355f,1.0f},
std::array<GLfloat, 4>{0.0535f,0.7006f,0.8299f,1.0f},
std::array<GLfloat, 4>{0.0433f,0.7039f,0.8242f,1.0f},
std::array<GLfloat, 4>{0.0328f,0.7071f,0.8183f,1.0f},
std::array<GLfloat, 4>{0.0234f,0.7103f,0.8124f,1.0f},
std::array<GLfloat, 4>{0.0155f,0.7133f,0.8064f,1.0f},
std::array<GLfloat, 4>{0.0091f,0.7163f,0.8003f,1.0f},
std::array<GLfloat, 4>{0.0046f,0.7192f,0.7941f,1.0f},
std::array<GLfloat, 4>{0.0019f,0.7220f,0.7878f,1.0f},
std::array<GLfloat, 4>{0.0009f,0.7248f,0.7815f,1.0f},
std::array<GLfloat, 4>{0.0018f,0.7275f,0.7752f,1.0f},
std::array<GLfloat, 4>{0.0046f,0.7301f,0.7688f,1.0f},
std::array<GLfloat, 4>{0.0094f,0.7327f,0.7623f,1.0f},
std::array<GLfloat, 4>{0.0162f,0.7352f,0.7558f,1.0f},
std::array<GLfloat, 4>{0.0253f,0.7376f,0.7492f,1.0f},
std::array<GLfloat, 4>{0.0369f,0.7400f,0.7426f,1.0f},
std::array<GLfloat, 4>{0.0504f,0.7423f,0.7359f,1.0f},
std::array<GLfloat, 4>{0.0638f,0.7446f,0.7292f,1.0f},
std::array<GLfloat, 4>{0.0770f,0.7468f,0.7224f,1.0f},
std::array<GLfloat, 4>{0.0899f,0.7489f,0.7156f,1.0f},
std::array<GLfloat, 4>{0.1023f,0.7510f,0.7088f,1.0f},
std::array<GLfloat, 4>{0.1141f,0.7531f,0.7019f,1.0f},
std::array<GLfloat, 4>{0.1252f,0.7552f,0.6950f,1.0f},
std::array<GLfloat, 4>{0.1354f,0.7572f,0.6881f,1.0f},
std::array<GLfloat, 4>{0.1448f,0.7593f,0.6812f,1.0f},
std::array<GLfloat, 4>{0.1532f,0.7614f,0.6741f,1.0f},
std::array<GLfloat, 4>{0.1609f,0.7635f,0.6671f,1.0f},
std::array<GLfloat, 4>{0.1678f,0.7656f,0.6599f,1.0f},
std::array<GLfloat, 4>{0.1741f,0.7678f,0.6527f,1.0f},
std::array<GLfloat, 4>{0.1799f,0.7699f,0.6454f,1.0f},
std::array<GLfloat, 4>{0.1853f,0.7721f,0.6379f,1.0f},
std::array<GLfloat, 4>{0.1905f,0.7743f,0.6303f,1.0f},
std::array<GLfloat, 4>{0.1954f,0.7765f,0.6225f,1.0f},
std::array<GLfloat, 4>{0.2003f,0.7787f,0.6146f,1.0f},
std::array<GLfloat, 4>{0.2061f,0.7808f,0.6065f,1.0f},
std::array<GLfloat, 4>{0.2118f,0.7828f,0.5983f,1.0f},
std::array<GLfloat, 4>{0.2178f,0.7849f,0.5899f,1.0f},
std::array<GLfloat, 4>{0.2244f,0.7869f,0.5813f,1.0f},
std::array<GLfloat, 4>{0.2318f,0.7887f,0.5725f,1.0f},
std::array<GLfloat, 4>{0.2401f,0.7905f,0.5636f,1.0f},
std::array<GLfloat, 4>{0.2491f,0.7922f,0.5546f,1.0f},
std::array<GLfloat, 4>{0.2589f,0.7937f,0.5454f,1.0f},
std::array<GLfloat, 4>{0.2695f,0.7951f,0.5360f,1.0f},
std::array<GLfloat, 4>{0.2809f,0.7964f,0.5266f,1.0f},
std::array<GLfloat, 4>{0.2929f,0.7975f,0.5170f,1.0f},
std::array<GLfloat, 4>{0.3052f,0.7985f,0.5074f,1.0f},
std::array<GLfloat, 4>{0.3176f,0.7994f,0.4975f,1.0f},
std::array<GLfloat, 4>{0.3301f,0.8002f,0.4876f,1.0f},
std::array<GLfloat, 4>{0.3424f,0.8009f,0.4774f,1.0f},
std::array<GLfloat, 4>{0.3548f,0.8016f,0.4669f,1.0f},
std::array<GLfloat, 4>{0.3671f,0.8021f,0.4563f,1.0f},
std::array<GLfloat, 4>{0.3795f,0.8026f,0.4454f,1.0f},
std::array<GLfloat, 4>{0.3921f,0.8029f,0.4344f,1.0f},
std::array<GLfloat, 4>{0.4050f,0.8031f,0.4233f,1.0f},
std::array<GLfloat, 4>{0.4184f,0.8030f,0.4122f,1.0f},
std::array<GLfloat, 4>{0.4322f,0.8028f,0.4013f,1.0f},
std::array<GLfloat, 4>{0.4463f,0.8024f,0.3904f,1.0f},
std::array<GLfloat, 4>{0.4608f,0.8018f,0.3797f,1.0f},
std::array<GLfloat, 4>{0.4753f,0.8011f,0.3691f,1.0f},
std::array<GLfloat, 4>{0.4899f,0.8002f,0.3586f,1.0f},
std::array<GLfloat, 4>{0.5044f,0.7993f,0.3480f,1.0f},
std::array<GLfloat, 4>{0.5187f,0.7982f,0.3374f,1.0f},
std::array<GLfloat, 4>{0.5329f,0.7970f,0.3267f,1.0f},
std::array<GLfloat, 4>{0.5470f,0.7957f,0.3159f,1.0f},
std::array<GLfloat, 4>{0.5609f,0.7943f,0.3050f,1.0f},
std::array<GLfloat, 4>{0.5748f,0.7929f,0.2941f,1.0f},
std::array<GLfloat, 4>{0.5886f,0.7913f,0.2833f,1.0f},
std::array<GLfloat, 4>{0.6024f,0.7896f,0.2726f,1.0f},
std::array<GLfloat, 4>{0.6161f,0.7878f,0.2622f,1.0f},
std::array<GLfloat, 4>{0.6297f,0.7859f,0.2521f,1.0f},
std::array<GLfloat, 4>{0.6433f,0.7839f,0.2423f,1.0f},
std::array<GLfloat, 4>{0.6567f,0.7818f,0.2329f,1.0f},
std::array<GLfloat, 4>{0.6701f,0.7796f,0.2239f,1.0f},
std::array<GLfloat, 4>{0.6833f,0.7773f,0.2155f,1.0f},
std::array<GLfloat, 4>{0.6963f,0.7750f,0.2075f,1.0f},
std::array<GLfloat, 4>{0.7091f,0.7727f,0.1998f,1.0f},
std::array<GLfloat, 4>{0.7218f,0.7703f,0.1924f,1.0f},
std::array<GLfloat, 4>{0.7344f,0.7679f,0.1852f,1.0f},
std::array<GLfloat, 4>{0.7468f,0.7654f,0.1782f,1.0f},
std::array<GLfloat, 4>{0.7590f,0.7629f,0.1717f,1.0f},
std::array<GLfloat, 4>{0.7710f,0.7604f,0.1658f,1.0f},
std::array<GLfloat, 4>{0.7829f,0.7579f,0.1608f,1.0f},
std::array<GLfloat, 4>{0.7945f,0.7554f,0.1570f,1.0f},
std::array<GLfloat, 4>{0.8060f,0.7529f,0.1546f,1.0f},
std::array<GLfloat, 4>{0.8172f,0.7505f,0.1535f,1.0f},
std::array<GLfloat, 4>{0.8281f,0.7481f,0.1536f,1.0f},
std::array<GLfloat, 4>{0.8389f,0.7457f,0.1546f,1.0f},
std::array<GLfloat, 4>{0.8495f,0.7435f,0.1564f,1.0f},
std::array<GLfloat, 4>{0.8600f,0.7413f,0.1587f,1.0f},
std::array<GLfloat, 4>{0.8703f,0.7392f,0.1615f,1.0f},
std::array<GLfloat, 4>{0.8804f,0.7372f,0.1650f,1.0f},
std::array<GLfloat, 4>{0.8903f,0.7353f,0.1695f,1.0f},
std::array<GLfloat, 4>{0.9000f,0.7336f,0.1749f,1.0f},
std::array<GLfloat, 4>{0.9093f,0.7321f,0.1815f,1.0f},
std::array<GLfloat, 4>{0.9184f,0.7308f,0.1890f,1.0f},
std::array<GLfloat, 4>{0.9272f,0.7298f,0.1973f,1.0f},
std::array<GLfloat, 4>{0.9357f,0.7290f,0.2061f,1.0f},
std::array<GLfloat, 4>{0.9440f,0.7285f,0.2151f,1.0f},
std::array<GLfloat, 4>{0.9523f,0.7284f,0.2237f,1.0f},
std::array<GLfloat, 4>{0.9606f,0.7285f,0.2312f,1.0f},
std::array<GLfloat, 4>{0.9689f,0.7292f,0.2373f,1.0f},
std::array<GLfloat, 4>{0.9770f,0.7304f,0.2418f,1.0f},
std::array<GLfloat, 4>{0.9842f,0.7330f,0.2446f,1.0f},
std::array<GLfloat, 4>{0.9900f,0.7365f,0.2429f,1.0f},
std::array<GLfloat, 4>{0.9946f,0.7407f,0.2394f,1.0f},
std::array<GLfloat, 4>{0.9966f,0.7458f,0.2351f,1.0f},
std::array<GLfloat, 4>{0.9971f,0.7513f,0.2309f,1.0f},
std::array<GLfloat, 4>{0.9972f,0.7569f,0.2267f,1.0f},
std::array<GLfloat, 4>{0.9971f,0.7626f,0.2224f,1.0f},
std::array<GLfloat, 4>{0.9969f,0.7683f,0.2181f,1.0f},
std::array<GLfloat, 4>{0.9966f,0.7740f,0.2138f,1.0f},
std::array<GLfloat, 4>{0.9962f,0.7798f,0.2095f,1.0f},
std::array<GLfloat, 4>{0.9957f,0.7856f,0.2053f,1.0f},
std::array<GLfloat, 4>{0.9949f,0.7915f,0.2012f,1.0f},
std::array<GLfloat, 4>{0.9938f,0.7974f,0.1974f,1.0f},
std::array<GLfloat, 4>{0.9923f,0.8034f,0.1939f,1.0f},
std::array<GLfloat, 4>{0.9906f,0.8095f,0.1906f,1.0f},
std::array<GLfloat, 4>{0.9885f,0.8156f,0.1875f,1.0f},
std::array<GLfloat, 4>{0.9861f,0.8218f,0.1846f,1.0f},
std::array<GLfloat, 4>{0.9835f,0.8280f,0.1817f,1.0f},
std::array<GLfloat, 4>{0.9807f,0.8342f,0.1787f,1.0f},
std::array<GLfloat, 4>{0.9778f,0.8404f,0.1757f,1.0f},
std::array<GLfloat, 4>{0.9748f,0.8467f,0.1726f,1.0f},
std::array<GLfloat, 4>{0.9720f,0.8529f,0.1695f,1.0f},
std::array<GLfloat, 4>{0.9694f,0.8591f,0.1665f,1.0f},
std::array<GLfloat, 4>{0.9671f,0.8654f,0.1636f,1.0f},
std::array<GLfloat, 4>{0.9651f,0.8716f,0.1608f,1.0f},
std::array<GLfloat, 4>{0.9634f,0.8778f,0.1582f,1.0f},
std::array<GLfloat, 4>{0.9619f,0.8840f,0.1557f,1.0f},
std::array<GLfloat, 4>{0.9608f,0.8902f,0.1532f,1.0f},
std::array<GLfloat, 4>{0.9601f,0.8963f,0.1507f,1.0f},
std::array<GLfloat, 4>{0.9596f,0.9023f,0.1480f,1.0f},
std::array<GLfloat, 4>{0.9595f,0.9084f,0.1450f,1.0f},
std::array<GLfloat, 4>{0.9597f,0.9143f,0.1418f,1.0f},
std::array<GLfloat, 4>{0.9601f,0.9203f,0.1382f,1.0f},
std::array<GLfloat, 4>{0.9608f,0.9262f,0.1344f,1.0f},
std::array<GLfloat, 4>{0.9618f,0.9320f,0.1304f,1.0f},
std::array<GLfloat, 4>{0.9629f,0.9379f,0.1261f,1.0f},
std::array<GLfloat, 4>{0.9642f,0.9437f,0.1216f,1.0f},
std::array<GLfloat, 4>{0.9657f,0.9494f,0.1168f,1.0f},
std::array<GLfloat, 4>{0.9674f,0.9552f,0.1116f,1.0f},
std::array<GLfloat, 4>{0.9692f,0.9609f,0.1061f,1.0f},
std::array<GLfloat, 4>{0.9711f,0.9667f,0.1001f,1.0f},
std::array<GLfloat, 4>{0.9730f,0.9724f,0.0938f,1.0f},
std::array<GLfloat, 4>{0.9749f,0.9782f,0.0872f,1.0f},
std::array<GLfloat, 4>{0.9769f,0.9839f,0.0805f,1.0f},
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












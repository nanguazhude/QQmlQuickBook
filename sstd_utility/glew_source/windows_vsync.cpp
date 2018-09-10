/*just for windows*/
#if defined(Q_OS_WIN)||defined(_WIN32)||defined(_WIN64)

#include <GL/glew.h>
#include <GL/wglew.h>

extern void ___glEnableGLVSNC();
extern void ___glDisableGLVSNC();

/*打开垂直同步*/
void ___glEnableGLVSNC() {
    wglSwapIntervalEXT(true);
}

/*关闭垂直同步*/
void ___glDisableGLVSNC() {
    wglSwapIntervalEXT(false);
}

#endif


/*
http://www.itkeyword.com/doc/4782487057909779x120/how-to-enable-vsync-in-opengl
*/
//wglSwapIntervalEXT

/**
opengl默认情况下，SwapBuffers是和屏幕刷新率同步的，所以你设置的显示器刷新率是60的话，你的opengl程序的FPS只有在60帧。包括nehe的例子程序都是如此，如果要关闭垂直同步功能，需要使用opengl的扩展接口。贴上代码。
typedef void (APIENTRY *PFNWGLEXTSWAPCONTROLPROC) (int);
typedef int (*PFNWGLEXTGETSWAPINTERVALPROC) (void);
PFNWGLEXTSWAPCONTROLPROC wglSwapIntervalEXT = NULL;
PFNWGLEXTGETSWAPINTERVALPROC wglGetSwapIntervalEXT = NULL;
// 初始化函数指针接口
bool InitVSync()
{
char* extensions = (char*)glGetString(GL_EXTENSIONS);
if (strstr(extensions,"WGL_EXT_swap_control")) {
wglSwapIntervalEXT = (PFNWGLEXTSWAPCONTROLPROC)wglGetProcAddress("wglSwapIntervalEXT");
wglGetSwapIntervalEXT = (PFNWGLEXTGETSWAPINTERVALPROC)wglGetProcAddress("wglGetSwapIntervalEXT");
return true;
}

return false;
}

// 判断当前状态是否为垂直同步
bool IsVSyncEnabled()
{
return (wglGetSwapIntervalEXT() > 0);
}

// 开启和关闭垂直同步
void SetVSyncState(bool enable)
{
if (enable)
wglSwapIntervalEXT(1);
else
wglSwapIntervalEXT(0);
}

使用方法:

bool isOk = InitVSync();
if (isOk) {
SetVSyncState(false);
}

这样在初始化的地方使用这些代码可以关闭垂直同步功能，这样FPS可以变的很高。

注意: 需要加入下面的这些代码才能正常编译。

// 包含windows的头文件
#include <windows.h>
// 包含opengl需要用到的头文件(完整的vs编辑器中会自带这些文件)
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>
// 设置链接时的库文件
#pragma comment(lib, "OPENGL32.LIB")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")
**/



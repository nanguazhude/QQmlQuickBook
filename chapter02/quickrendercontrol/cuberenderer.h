#pragma once
#include <sstd_RenderPack.hpp>
 
class CubeRenderer
{
public:
    CubeRenderer( std::shared_ptr<sstd::RenderPack> );
    ~CubeRenderer();

    void render(QWindow *w, QOpenGLContext *share, uint texture);
    void resize(int,int) {
    }

private:
    void init(QWindow *w, QOpenGLContext *share);
    
    std::shared_ptr<sstd::RenderPack> mmm_RenderPack;
};



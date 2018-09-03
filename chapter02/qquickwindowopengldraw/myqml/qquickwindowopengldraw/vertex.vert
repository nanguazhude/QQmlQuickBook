#version 450

layout(location=0) in vec4 iposition;
layout(location=1) in vec4 icolor;
layout(location=2) uniform mat4 imvp;

out vec4 inoutcolor;

void main(void){
    gl_Position =  imvp * iposition ;
    inoutcolor = icolor;
}

/*测试中文注释*/
//https://github.com/g-truc/glm







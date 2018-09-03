#version 450
smooth in vec2 tuv;
flat in int tID;

layout(binding=0) uniform sampler2DArray texture_array_  ;
out vec4 color;

void main(){
    color = texture( texture_array_ , vec3(tuv,tID) );
    //if( color.a < 0.00001 ){ discard ; }/*if blend not suppord*/
}

/*测试中文注释*/
//参考自：
//opengl superbible 7
//alienrain


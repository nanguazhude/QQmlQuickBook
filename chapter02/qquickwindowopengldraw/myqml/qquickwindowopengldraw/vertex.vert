#version 450

uniform mat4 points=mat4(
    0,0.25,0,1,       /*0,1*/
    0,0,0,1,   /*0,0*/
    0.25,0.25,0,1,    /*1,1*/
    0.25,0,0,1 /*1,0*/
);

layout( location = 0 ) in vec4 offsetValue;
smooth out vec2 tuv;
flat out int tID;

void main(){
    gl_Position = points[ gl_VertexID ];
    tuv.x = gl_Position.x*4 ;
    tuv.y = gl_Position.y*4 ;
    tID = gl_InstanceID;
    gl_Position.xy += offsetValue.xy   ;
}

/*测试中文注释*/
//参考自：
//opengl superbible 7
//alienrain







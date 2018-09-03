#version 450

uniform mat4 points=mat4(
    0,   0.25 , 0 , 1,     /*0,1*/
    0,   0 ,    0 , 1,     /*0,0*/
    0.25,0.25 , 0 , 1,     /*1,1*/
    0.25,0,     0 , 1      /*1,0*/
);

layout( location = 0 ) in vec4 offsetValue;
smooth out vec2 tuv;
flat out int tID;

void main(){
    float co = cos(offsetValue[2])                                   ;
    float so = sin(offsetValue[2])                                 ;
    mat2 rot = mat2(vec2(co, so),                                   
                    vec2(-so, co))                                 ;
    vec4 positionPoint = points[ gl_VertexID ]                     ; 
    tuv.x = positionPoint.x * 4                                    ; 
    tuv.y = positionPoint.y * 4                                    ; 
    tID   = gl_InstanceID                                          ;
    vec2 pos = positionPoint.xy;
     pos -= vec2(0.125)                      ; 
     pos = rot * pos ;

    pos += 1.75*( offsetValue.xy - vec2(0.5))            ; 
   
   
    gl_Position.xy = pos                                           ;
    gl_Position.z = offsetValue[3]                                 ;
    gl_Position.w = 1                                              ;
}

/*测试中文注释*/
//参考自：
//opengl superbible 7
//alienrain

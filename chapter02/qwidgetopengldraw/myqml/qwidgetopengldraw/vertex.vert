#version 450

uniform mat4 vertices=mat4(
 -0.5000,   -0.5000,         0,    1.0000,//first column
  0.5000,   -0.5000,         0,    1.0000,//second column
       0,    0.5000,         0,    1.0000,//third column
  1.0000,    1.0000,    1.0000,    1.0000 //fourth column
);

void main(void){
    gl_Position = vertices[gl_VertexID];
}

/*测试中文注释*/








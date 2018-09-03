#version 450

//layout(location=0) in vec4 ixyrz ;

 //out VS_OUT {
 //  flat int alien  ; /*纹理索引*/
 //  vec2 tc         ; /*纹理坐标*/
// } vs_out;

void main(void){

     const vec2[4] position = vec2[4](
                 vec2(-0.5, -0.5),
                 vec2( 0.5, -0.5),
                 vec2(-0.5,  0.5),
                 vec2( 0.5,  0.5));

//    vs_out.tc =  position[gl_VertexID].xy + vec2(0.5);
   vec2 tc = position[gl_VertexID].xy + vec2(0.5);
//    float co = cos(ixyrz[2]);
//    float so = sin(ixyrz[2]);

//    mat2 rot = mat2(vec2( co, so),
//                    vec2(-so, co));

//     vec2 pos = 0.25 * rot * position[gl_VertexID];

//     gl_Position = vec4(pos.x + ixyrz[0],
//                        pos.y + ixyrz[1],
//                       ixyrz[3], 1.0);

    gl_Position = vec4( tc.x , tc.y ,0.5,1 );

//    vs_out.alien = gl_InstanceID % 64;

}

/*测试中文注释*/
//参考自：
//opengl superbible 7
//alienrain







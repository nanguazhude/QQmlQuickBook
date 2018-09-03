#version 450

out vec4 color;

//in VS_OUT {
 //    flat int alien;
 //    vec2 tc;
//} fs_in;

//layout(binding=0) uniform sampler2DArray tex_aliens;  

void main(void){
    //color = texture(tex_aliens, vec3(fs_in.tc, float(fs_in.alien)));
    color = vec4(1,1,1,1);
}

/*测试中文注释*/
//参考自：
//opengl superbible 7
//alienrain


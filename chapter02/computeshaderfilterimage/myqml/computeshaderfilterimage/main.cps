/*OpenGL 版本*/
#version 450 core

layout(local_size_x = 3       , 
       local_size_y = 3       ,
       local_size_z = 1    ) in ;

/**
Matlab :
>> x = fspecial('gaussian',[3 3],1)

x =

    0.0751    0.1238    0.0751
    0.1238    0.2042    0.1238
    0.0751    0.1238    0.0751
**/
uniform mat3 Gaussian(
    0.0751 ,   0.1238 ,   0.0751,
    0.1238 ,   0.2042 ,   0.1238,
    0.0751 ,   0.1238 ,   0.0751,
);

layout(binding = 0,rgba8ui) readonly uniform image2D  ImageInput;
layout(binding = 1,rgba8ui) writeonly uniform image2D ImageOutput;

shared vec4 TmpData[9];

void main(void){

    ivec2 pos = ivec2( gl_WorkGroupID.xy ) - ivec2(1) ;
    pos += ivec2( gl_LocalInvocationID.xy );

    TmpData[gl_LocalInvocationIndex] = 
    Gaussian[gl_LocalInvocationID.x][gl_LocalInvocationID.y]*
    vec4(imageLoad(ImageInput,pos));

    barrier()/*等待此工作组完成*/;
    memoryBarrierShared()/*强制更新shared数据，避免数据存在缓存没有更新*/;

    if(0==gl_LocalInvocationIndex){
        vec4 ans =  TmpData[0] +
                    TmpData[1] +
                    TmpData[2] +
                    TmpData[3] +
                    TmpData[4] +
                    TmpData[5] +
                    TmpData[6] +
                    TmpData[7] +
                    TmpData[8] ;

        imageStore( ImageOutput,
                    ivec2(gl_WorkGroupID.xy),
                    uvec4(ans)
                  );
    }

}

/*测试中文注释*/


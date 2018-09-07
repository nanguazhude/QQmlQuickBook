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

//gl_NumWorkGroups : 此例子相当于图像的长宽
//gl_WorkGroupID : 此例子相当于像素的位置
//gl_WorkGroupSize : 此例子相当于滤波器的尺寸[3 , 3]
//gl_LocalInvocationID :此例子相当于当前滤波器像素位置
//gl_LocalInvocationIndex : 是gl_LocalInvocationID的一种扁平化方式
//gl_GlobalInvocationID ： gl_WorkGroupID * gl_WorkGroupSize + gl_LocalInvocationID

void main(void){
    /*计算当前坐标*/
    ivec2 pos = ivec2( gl_WorkGroupID.xy ) - ivec2(1) ;
    pos += ivec2( gl_LocalInvocationID.xy );

    /*像素超出边界则用最邻近替代*/
    if(pos.x >= gl_NumWorkGroups.x){pos.x = gl_NumWorkGroups.x-1;}
    if(pos.y >= gl_NumWorkGroups.y){pos.y = gl_NumWorkGroups.y-1;}
    if(pos.x < 0){pos.x=0;}
    if(pos.y < 0){pos.y=0;}

    /*计算当前像素权重*/
    TmpData[gl_LocalInvocationIndex] = 
    Gaussian[gl_LocalInvocationID.x][gl_LocalInvocationID.y]*
    vec4(imageLoad(ImageInput,pos));

    barrier()/*等待此工作组完成*/;
    memoryBarrierShared()/*强制更新shared数据，避免数据存在缓存没有更新*/;

    /*保证结果只写入一次*/
    if(0==gl_LocalInvocationIndex){
        /*计算结果*/
        vec4 ans =  TmpData[0] +
                    TmpData[1] +
                    TmpData[2] +
                    TmpData[3] +
                    TmpData[4] +
                    TmpData[5] +
                    TmpData[6] +
                    TmpData[7] +
                    TmpData[8] ;
        /*结果写入缓存*/
        imageStore( ImageOutput,
                    ivec2(gl_WorkGroupID.xy),
                    uvec4(ans)
                  );
    }/*if(0==*/

}

/*测试中文注释*/


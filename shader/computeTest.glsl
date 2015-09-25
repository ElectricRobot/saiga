##GL_COMPUTE_SHADER

#version 430


layout(binding=3, rgba16) uniform image2D destTex;

layout(local_size_x = 32, local_size_y = 32) in;


 void main() {

     ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);


     //Store operations to any texel that is outside the boundaries of the bound image will do nothing.
     vec4 color = vec4(1,0,0,1);
     imageStore(destTex, storePos, color);
 }

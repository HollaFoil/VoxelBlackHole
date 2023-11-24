#version 430 core
#extension GL_ARB_compute_shader : enable
#extension GL_ARB_shader_storage_buffer_object : enable

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1)in;
layout(rgba32f, binding = 0) uniform image2D img_output;

//uniform vec4 vertexColor;
uniform float time;


//if u change these parameters here but not in the other file, a little trolling will happen :)
const float SCR_WIDTH = 800;
const float SCR_HEIGHT = 600;

//sphere location
vec3 spherecenter =  vec3(0.0f, 0.0f, 40.0f);
float voxel = 10.0f;
float radius = 40.0f;

const int max_steps = 300;
const  vec2 screen =  vec2(SCR_WIDTH/2, SCR_HEIGHT/2);



// Transformations
uniform mat3 basis = mat3(1);
uniform vec3 translation = vec3(0 , 0, 0);

vec4 CastRay(vec3 origin, vec3 direction){
      float sgn_x = direction.x > 0 ? 1 : -1;
      float sgn_y = direction.y > 0 ? 1 : -1;
      float sgn_z = direction.z > 0 ? 1 : -1;
      float offset_x = direction.x > 0 ? 0 : 1;
      float offset_y = direction.y > 0 ? 0 : 1;
      float offset_z= direction.z > 0 ? 0 : 1;
      vec3 grid_coordinates = floor(origin/voxel);

      int step = 0;
      vec3 pos = origin;
      float dist = distance( grid_coordinates, spherecenter);
      int state_last = -1;

      while( (dist > radius) && (step< max_steps)){
            step++;
            float dtx = ((sgn_x + offset_x + grid_coordinates.x)*voxel - pos.x)/direction.x;
            float dty = ((sgn_y + offset_y + grid_coordinates.y)*voxel - pos.y)/direction.y;
            float dtz = ((sgn_z + offset_z + grid_coordinates.z)*voxel - pos.z)/direction.z;
            if(dtx < dty && dtx < dtz){
                  pos += direction*dtx;
                  grid_coordinates.x += sgn_x;
                  state_last = 0;

            }else if(dty < dtz){
                  pos += direction*dty;
                  grid_coordinates.y += sgn_y;
                  state_last = 1;

            }else{
                  pos +=direction*dtz;
                  grid_coordinates.z += sgn_z;
                  state_last = 2;
            }
            dist = distance( grid_coordinates, spherecenter);
      }
      vec4 color;
      if (step == max_steps) color = vec4(0, 0, 0, 1);
      else if ( state_last == 0) color = vec4(1,0,0,1);
      else if ( state_last == 1) color = vec4(0,1,0,1) ;
      else color = vec4(0,0,1,1) ;
      return color;
}

float rand(vec2 co){
      return fract(sin(dot(co, vec2(time, 78.233))) * 43758.5453);
}

vec2 RebaseCoordinateSystem(vec2 pos){
      pos -= screen;
     // pos *= voxel; //=> convert to voxels
      return pos;
}
void main(){
      //the computations are not efficient but if ur asking that question u are a  :nerd
      vec2 pos = RebaseCoordinateSystem(gl_GlobalInvocationID.xy);
      //spherecenter = basis*vec3(spherecenter.x, spherecenter.y, spherecenter.z*voxel);
      //the above line breaks spacetime itself so use with caution (or rather dont cuz it may crash the graphics card)
      vec3 position = basis*(vec3(pos, 0)+translation);
      vec3 newpos = position + basis*vec3(0, 0, 1000);
      position  -= basis*(spherecenter*voxel) -spherecenter*voxel;
      vec4 color = CastRay(position , newpos);
     // vec4 color = vec4(1.0f, (gl_GlobalInvocationID.y/SCR_HEIGHT-0.5f), 1.0f, 1.0f);
      imageStore(img_output, ivec2(gl_GlobalInvocationID.xy), color);
}




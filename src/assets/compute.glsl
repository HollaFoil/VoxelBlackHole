#version 430 core

layout(local_size_x = 32, local_size_y = 32) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

#define SMOOTH 0

uniform float time;

const float width = 1536.0f;
const float height = 864.0f;
const vec2 scr_size = vec2(width, height);
const vec3 centerOfSphere = vec3(0.0005f);
float grid_size = 1.0f;

float sphere_size = sin(time)*5.0f + 7.5f;

vec3 castRay(vec3 origin, vec3 direction) {
    float sign_x = direction.x > 0 ? 1 : direction.x == 0 ? 0 : -1;
    float sign_y = direction.y > 0 ? 1 : direction.y == 0 ? 0 : -1;
    float sign_z = direction.z > 0 ? 1 : direction.z == 0 ? 0 : -1;

    vec3 pos = origin;
    vec3 tile_coords = (floor(pos)/grid_size);

    float dist = distance(tile_coords, centerOfSphere);
    int step = 0;
    int max_steps = 85;
    int lastdir = -1;
    while (dist > sphere_size && step < max_steps) {
        step++;
        float dtx = ((tile_coords.x + sign_x*grid_size) - pos.x) / direction.x;
        float dty = ((tile_coords.y + sign_y*grid_size) - pos.y) / direction.y;
        float dtz = ((tile_coords.z + sign_z*grid_size) - pos.z) / direction.z;
        if (dtx < dty && dtx < dtz && sign_x != 0) {
            pos += direction*dtx;
            tile_coords.x += sign_x*grid_size;
            lastdir = 0;
        }
        else if (dty < dtz && sign_y != 0) {
            pos += direction*dty;
            tile_coords.y += sign_y*grid_size;
            lastdir = 1;
        }
        else {
            pos += direction*dtz;
            tile_coords.z += sign_z*grid_size;
            lastdir = 2;
        }

        dist = distance(floor(pos)/grid_size, centerOfSphere);
    }
    vec3 color;
    if (step == max_steps) color = vec3(0,0,0);
    else if (lastdir == 0) color = vec3(255,0,0);
    else if (lastdir == 1) color = vec3(0,255,0);
    else color = vec3(0,0,255);
    return color;
}

mat3 calcLookAtMatrix(vec3 origin, vec3 target, float roll) {
    vec3 rr = vec3(sin(roll), cos(roll), 0.0);
    vec3 ww = normalize(target - origin);
    vec3 uu = normalize(cross(ww, rr));
    vec3 vv = normalize(cross(uu, ww));

    return mat3(uu, vv, ww);
}

void main()
{
    vec3 origin = vec3(sin(time/4)*-40.0f, 0.0f, cos(time/4)*-40.0f);

    vec3 direction = vec3(float(gl_GlobalInvocationID.x), float(gl_GlobalInvocationID.y), width);
    direction.xy = direction.xy*2.0f - scr_size;
    direction = normalize(direction);
    direction += 0.00001f;

    mat3 lookAt = calcLookAtMatrix(origin, centerOfSphere, 0);
    direction = direction*lookAt;
    //direction.x = ((direction.x)-0.5f)*2.0f;
    //direction.y = ((direction.y)-0.5f)*2.0f;


    vec3 color = castRay(origin, direction);

    imageStore(img_output, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1.0));
}


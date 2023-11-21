#version 430 core
precision highp float;


layout(local_size_x = 32, local_size_y = 32) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

#define SMOOTH 0

uniform float time;

const float width = 1536.0f;
const float height = 864.0f;
const vec2 scr_size = vec2(width, height);
const vec3 centerOfSphere = vec3(0.5f);
const float grid_size = 0.5f;

float sphere_size = sin(time/2)*4.0f + 8.4f;

vec3 castRay(vec3 origin, vec3 direction) {
    const float sign_x = direction.x > 0 ? 1 : -1;
    const float sign_y = direction.y > 0 ? 1 : -1;
    const float sign_z = direction.z > 0 ? 1 : -1;
    const float offset_x = direction.x > 0 ? 0 : 1;
    const float offset_y = direction.y > 0 ? 0 : 1;
    const float offset_z = direction.z > 0 ? 0 : 1;
    const int max_steps = 200;

    vec3 pos = origin;
    vec3 tile_coords = floor(pos/grid_size); // Maybe needs to be * instead, idk

    float dist = distance(tile_coords, centerOfSphere);
    int step = 0;

    int lastdir = -1;
    while (dist > sphere_size && step < max_steps) {
        step++;
        float dtx = ((tile_coords.x + sign_x + offset_x)*grid_size - pos.x) / direction.x;
        float dty = ((tile_coords.y + sign_y + offset_y)*grid_size - pos.y) / direction.y;
        float dtz = ((tile_coords.z + sign_z + offset_z)*grid_size - pos.z) / direction.z;
        if (dtx < dty && dtx < dtz) {
            pos += direction*dtx;
            tile_coords.x += sign_x;
            lastdir = 0;
        }
        else if (dty < dtz) {
            pos += direction*dty;
            tile_coords.y += sign_y;
            lastdir = 1;
        }
        else {
            pos += direction*dtz;
            tile_coords.z += sign_z;
            lastdir = 2;
        }

        dist = distance(tile_coords*grid_size, centerOfSphere);
    }
    vec3 color;
    if (step == max_steps) color = vec3(0,0,0);
    else if (lastdir == 0) color = vec3(255,0,0);
    else if (lastdir == 1) color = vec3(0,255,0);
    else color = vec3(0,0,255);
    return color;
}

mat4 LookAt(vec3 eye, vec3 at, vec3 up)
{
    vec3 zaxis = normalize(at - eye);
    vec3 xaxis = normalize(cross(zaxis, up));
    vec3 yaxis = cross(xaxis, zaxis);

    zaxis *= -1.0f;

    mat4 viewMatrix = mat4(
    vec4(xaxis.x, xaxis.y, xaxis.z, -dot(xaxis, eye)),
    vec4(yaxis.x, yaxis.y, yaxis.z, -dot(yaxis, eye)),
    vec4(zaxis.x, zaxis.y, zaxis.z, -dot(zaxis, eye)),
    vec4(0.0f, 0.0f, 0.0f, 1.0f)
    );

    return viewMatrix;
}

void main()
{
    vec3 origin = vec3 (sin(time/2.5f), sin(time/3.0f), cos(time/2.5f));
    origin = normalize(origin) * 40.0;

    vec2 pixel_coord = vec2(gl_GlobalInvocationID.xy);
    pixel_coord = pixel_coord*2.0f - scr_size;
    vec3 direction = normalize(vec3(pixel_coord, width));

    mat4 lookAt = LookAt(origin, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    vec4 dir  = lookAt*vec4(direction, 1.0f);
    dir /= dir.w;
    direction = dir.xyz;
    direction += 0.00001f;

    vec3 color = castRay(origin, normalize(direction));
    imageStore(img_output, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1.0));
}


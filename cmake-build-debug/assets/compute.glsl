#version 430 core
precision highp float;


layout(local_size_x = 512, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

#define SMOOTH 0

uniform float time;
uniform samplerCube cubeMap;

const float width = 1536.0f;
const float height = 864.0f;
const vec2 scr_size = vec2(width, height);
const vec3 centerOfSphere = vec3(0.5f);
const float grid_size = 0.10f;

float sphere_size = 300.0f - sin(time/3)*200;

float distSquared( vec3 A, vec3 B )
{
    vec3 C = A - B;
    return dot( C, C );

}

vec4 castRay(vec3 origin, vec3 direction) {
    const float sign_x = direction.x > 0 ? 1 : -1;
    const float sign_y = direction.y > 0 ? 1 : -1;
    const float sign_z = direction.z > 0 ? 1 : -1;
    const float offset_x = direction.x > 0 ? 1 : 0;
    const float offset_y = direction.y > 0 ? 1 : 0;
    const float offset_z = direction.z > 0 ? 1 : 0;
    const int max_steps = 650;


    vec3 sign = vec3(sign_x, sign_y, sign_z);
    vec3 offset = vec3(offset_x, offset_y, offset_z);
    vec3 inv = 1/direction;


    vec3 pos = origin;
    vec3 tile_coords = floor(pos/grid_size);
    vec3 tile_coords_scaled = tile_coords*grid_size;// Maybe needs to be * instead, idk
    vec3 sign_scaled = sign*grid_size;
    vec3 offset_scaled = offset*grid_size;



    float dist = distSquared(tile_coords_scaled, centerOfSphere);
    int step = 0;

    int lastdir = -1;
    while (step < max_steps && dist > sphere_size) {
        step++;
        vec3 dt = (tile_coords_scaled+offset_scaled - pos)*inv;
        if (dt.x < dt.y && dt.x < dt.z) {
            pos += direction*dt.x;
            tile_coords.x += sign_x;
            tile_coords_scaled.x += sign_scaled.x;
            lastdir = 0;
        }
        else if (dt.y < dt.z) {
            pos += direction*dt.y;
            tile_coords.y += sign_y;
            tile_coords_scaled.y += sign_scaled.y;
            lastdir = 1;
        }
        else {
            pos += direction*dt.z;
            tile_coords.z += sign_z;
            tile_coords_scaled.z += sign_scaled.z;
            lastdir = 2;
        }

        dist = distSquared(tile_coords_scaled, centerOfSphere);
    }
    vec4 color;
    if (step == max_steps) color = texture(cubeMap, -normalize(direction));
    else if (lastdir == 0) color = vec4(1,0,0,1) / (sqrt(sphere_size) + 2 - abs(tile_coords_scaled.x));
    else if (lastdir == 1) color = vec4(0,1,0,1) * 0;
    else color = vec4(0,0,1,1) * 0;
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
    vec3 direction = normalize(vec3(pixel_coord, width*1.4));

    mat4 lookAt = LookAt(origin, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    vec4 dir  = lookAt*vec4(direction, 1.0f);
    dir /= dir.w;
    direction = dir.xyz;
    direction += 0.00001f;

    vec4 color = castRay(origin, normalize(direction));
    imageStore(img_output, ivec2(gl_GlobalInvocationID.xy), color);
}


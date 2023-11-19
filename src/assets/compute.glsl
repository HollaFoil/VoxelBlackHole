#version 430 core

layout(local_size_x = 32, local_size_y = 32) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

#define SMOOTH 0


vec3 rainbow(float level)
{
    float r = float(level <= 2.0) + float(level > 4.0) * 0.5;
    float g = max(1.0 - abs(level - 2.0) * 0.5, 0.0);
    float b = (1.0 - (level - 4.0) * 0.5) * float(level >= 4.0);
    return vec3(r, g, b);
}

vec3 smoothRainbow (float x)
{
    float level1 = floor(x*6.0);
    float level2 = min(6.0, floor(x*6.0) + 1.0);

    vec3 a = rainbow(level1);
    vec3 b = rainbow(level2);

    return mix(a, b, fract(x*6.0));
}

void main()
{
    float pixel_coords = float(gl_GlobalInvocationID.x)/1536.0f;

    #if SMOOTH
    vec3 color = smoothRainbow(pixel_coords);
    #else
    vec3 color = rainbow(floor(pixel_coords*6.0));
    #endif

    imageStore(img_output, ivec2(gl_GlobalInvocationID.xy), vec4(color, 1.0));
}


#version 330 core
#define SMOOTH 0

out vec4 FragColor;
in vec3 pos;

vec3 rainbow(float level)
{
   /*
       Target colors
       =============

       L  x   color
       0  0.0 vec4(1.0, 0.0, 0.0, 1.0);
       1  0.2 vec4(1.0, 0.5, 0.0, 1.0);
       2  0.4 vec4(1.0, 1.0, 0.0, 1.0);
       3  0.6 vec4(0.0, 0.5, 0.0, 1.0);
       4  0.8 vec4(0.0, 0.0, 1.0, 1.0);
       5  1.0 vec4(0.5, 0.0, 0.5, 1.0);
   */

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
   vec3 norm = (pos + 1.0f) / 2.0f;

   #if SMOOTH
   vec3 color = smoothRainbow(norm.x);
   #else
   vec3 color = rainbow(floor(norm.x*6.0));
   #endif

   FragColor = vec4(color, 1.0f);
}
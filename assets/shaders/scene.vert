#version 430 core

layout(location = 0) in vec2 aPos;    // vec2 position
layout(location = 1) in vec2 aUV;     // vec2 UV
layout(location = 2) in vec4 aColor;  // vec4 color

out vec2 UV;
out vec4 Color;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(aPos, 0.0, 1.0); // z=0
    UV = aUV;
    Color = aColor;
}
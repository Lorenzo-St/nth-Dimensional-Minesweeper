#version 450

layout(location = 0) in vec4 pos;
layout(location = 4) in vec2 texcoord;

layout(location = 0)out vec2 texPos;

uniform mat4 objectMatrix;
uniform mat4 screenMatrix;

void main()
{
    gl_Position = screenMatrix * objectMatrix * pos;
    texPos = texcoord;
}

#version 450

layout(location = 0) in vec2 texPos;
uniform sampler2D tex;
uniform int textured;
uniform vec4 color;
out vec4 diffuseColor;

void main()
{
    if(textured == 1)
        diffuseColor = texture(tex, texPos);
    else
        diffuseColor = color;
}

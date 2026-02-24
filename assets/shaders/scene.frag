#version 430 core

in vec2 UV;
in vec4 Color;

out vec4 FragColor;
uniform sampler2D atlasTexture;

void main()
{
    FragColor = Color * texture(atlasTexture, UV); // combine vertex color with texture
}
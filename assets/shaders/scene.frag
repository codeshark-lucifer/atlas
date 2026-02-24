#version 430 core

in vec2 UV;
in vec4 Color;

out vec4 FragColor;
uniform sampler2D atlasTexture;

void main()
{
    vec4 texColor = texture(atlasTexture, UV);
    if (texColor.a < 0.1) // discard transparent pixels
        discard;
    FragColor = Color * texColor; // combine vertex color with texture
}
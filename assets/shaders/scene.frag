#version 330 core

in vec2 vUV;
in vec4 vColor;

uniform sampler2D atlasTexture;
uniform bool isText;

out vec4 FragColor;

void main()
{
    if (isText)
    {
        // Font atlas is GL_RED
        float alpha = texture(atlasTexture, vUV).r;
        FragColor = vec4(vColor.rgb, vColor.a * alpha);
    }
    else
    {
        // Normal sprite texture (RGBA)
        vec4 texColor = texture(atlasTexture, vUV);
        if (texColor.a < 0.1)
            discard;
        FragColor = texColor * vColor;
    }
}
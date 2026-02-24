#include <glad/glad.h>
#include <platform/win32.h>

#include <engine/input.h>
#include <engine/shader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

struct Vertex
{
    vec2 pos;
    vec2 uv;
    vec4 color;
};

struct Batch
{
    u32 tex{0};
    i32 w, h, chs;
    array<Vertex> vertices;
};

array<Batch> batches;

int main()
{
    InitPlatform();
    CreateWindowPlatform("atlas - engine", 956, 540);

    u32 program = CreateShaderProgram(
        "assets/shaders/scene.vert",
        "assets/shaders/scene.frag");

    mat4 proj = mat4::Ortho(0, input->screen.x, 0, input->screen.y, -1, 1);

    glUseProgram(program);
    SetUniform(program, "projection", proj);
    SetUniform(program, "atlasTexture", 0);
    batches.push_back(Batch{});

    vec2 pos = vec2(0.0f, 0.0f);
    vec2 size = vec2(100.0f, 100.0f);

    auto &b = batches[0];
    glGenTextures(1, &b.tex);
    glBindTexture(GL_TEXTURE_2D, b.tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char *data = stbi_load("assets/textures/wall.jpg", &b.w, &b.h, &b.chs, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, b.w, b.h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);

    b.vertices.push_back(Vertex{.pos = vec2(pos.x, pos.y), .uv = vec2(0.0f, 0.0f), .color = vec4(1.0f)});          // bottom-left
    b.vertices.push_back(Vertex{.pos = vec2(pos.x + size.x, pos.y), .uv = vec2(1.0f, 0.0f), .color = vec4(1.0f)}); // bottom-right
    b.vertices.push_back(Vertex{.pos = vec2(pos.x, pos.y + size.y), .uv = vec2(0.0f, 1.0f), .color = vec4(1.0f)}); // top-left

    b.vertices.push_back(Vertex{.pos = vec2(pos.x, pos.y + size.y), .uv = vec2(0.0f, 1.0f), .color = vec4(1.0f)});          // top-left
    b.vertices.push_back(Vertex{.pos = vec2(pos.x + size.x, pos.y + size.y), .uv = vec2(1.0f, 1.0f), .color = vec4(1.0f)}); // top-right
    b.vertices.push_back(Vertex{.pos = vec2(pos.x + size.x, pos.y), .uv = vec2(1.0f, 0.0f), .color = vec4(1.0f)});          // bottom-right

    u32 vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));

    glBindVertexArray(0);

    while (!ShouldClose())
    {
        Event event;
        PollEvent(&event);

        for (auto &b : batches)
        {
            glBindTexture(GL_TEXTURE0, b.tex);
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * b.vertices.size(), b.vertices.data(), GL_DYNAMIC_DRAW);

            glUseProgram(program);
            glDrawArrays(GL_TRIANGLES, 0, b.vertices.size());
            glBindVertexArray(0);
        }

        SwapBuffersWindow();
    }

    DestroyPlatform();
}

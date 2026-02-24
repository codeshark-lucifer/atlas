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
    u32 vao{}, vbo{}, ebo{};
    u32 tex{0};
    i32 w{}, h{}, chs{};
    array<Vertex> vertices;
    array<u32> indices;

    void reserve(size_t quadCount)
    {
        vertices.reserve(quadCount * 4); // 4 vertices per quad
        indices.reserve(quadCount * 6);  // 6 indices per quad
    }
};

array<Batch> batches;

int main()
{
    InitPlatform();
    CreateWindowPlatform("atlas - engine", 956, 540);

    u32 program = CreateShaderProgram(
        "assets/shaders/scene.vert",
        "assets/shaders/scene.frag");

    glUseProgram(program);
    SetUniform(program, "atlasTexture", 0);

    // create one batch
    batches.push_back(Batch{});
    auto &b = batches[0];
    b.reserve(1000); // reserve space for 1000 quads
    auto emit = [](vec2 pos, vec2 size, ivec2 ioffset, ivec2 isize, vec4 color = vec4(1.0f), bool flipX = false, bool flipY = false)
    {
        auto &b = batches[0];
        u32 startIndex = b.vertices.size();

        // 1. Calculate atlas texture coordinates
        float left = float(ioffset.x);
        float top = float(ioffset.y);
        float right = float(ioffset.x + isize.x);
        float bottom = float(ioffset.y + isize.y);

        // Handle flipping
        if (flipX)
            std::swap(left, right);
        if (flipY)
            std::swap(top, bottom);

        // Normalize by texture size
        float w = float(b.w);
        float h = float(b.h);

        vec2 uvBL = vec2(left, top) / vec2(w, h);     // bottom-left
        vec2 uvBR = vec2(right, top) / vec2(w, h);    // bottom-right
        vec2 uvTR = vec2(right, bottom) / vec2(w, h); // top-right
        vec2 uvTL = vec2(left, bottom) / vec2(w, h);  // top-left

        // 2. Push vertices
        b.vertices.push_back(Vertex{.pos = vec2(pos.x, pos.y), .uv = uvBL, .color = color});                   // bottom-left
        b.vertices.push_back(Vertex{.pos = vec2(pos.x + size.x, pos.y), .uv = uvBR, .color = color});          // bottom-right
        b.vertices.push_back(Vertex{.pos = vec2(pos.x + size.x, pos.y + size.y), .uv = uvTR, .color = color}); // top-right
        b.vertices.push_back(Vertex{.pos = vec2(pos.x, pos.y + size.y), .uv = uvTL, .color = color});          // top-left

        // 3. Push indices
        b.indices.push_back(startIndex + 0);
        b.indices.push_back(startIndex + 1);
        b.indices.push_back(startIndex + 2);

        b.indices.push_back(startIndex + 2);
        b.indices.push_back(startIndex + 3);
        b.indices.push_back(startIndex + 0);
    };
    // load texture
    glGenTextures(1, &b.tex);
    glBindTexture(GL_TEXTURE_2D, b.tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    unsigned char *data = stbi_load("assets/sprites/sprite.png", &b.w, &b.h, &b.chs, 0);
    if (data)
    {
        GLenum format = GL_RGB;
        if (b.chs == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, b.w, b.h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);

    // create VAO, VBO, EBO
    glGenVertexArrays(1, &b.vao);
    glGenBuffers(1, &b.vbo);
    glGenBuffers(1, &b.ebo);

    glBindVertexArray(b.vao);

    glBindBuffer(GL_ARRAY_BUFFER, b.vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, uv));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b.ebo);

    glBindVertexArray(0);

    while (!ShouldClose())
    {
        Event event;
        PollEvent(&event);

        // update batch
        emit(vec2(0.0f), vec2(100.0f), ivec2(16, 0), ivec2(16));

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, input->screen.x, input->screen.y);

        glUseProgram(program);
        mat4 proj = mat4::Ortho(0, input->screen.x, input->screen.y, 0, -1, 1);
        SetUniform(program, "projection", proj);

        for (auto &b : batches)
        {
            glBindTexture(GL_TEXTURE_2D, b.tex);
            glBindVertexArray(b.vao);

            // update VBO/EBO
            glBindBuffer(GL_ARRAY_BUFFER, b.vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * b.vertices.size(), b.vertices.data(), GL_DYNAMIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b.ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * b.indices.size(), b.indices.data(), GL_DYNAMIC_DRAW);

            glDrawElements(GL_TRIANGLES, b.indices.size(), GL_UNSIGNED_INT, 0);

            b.vertices.clear();
            b.indices.clear();

            glBindVertexArray(0);
        }

        SwapBuffersWindow();
    }

    DestroyPlatform();
}
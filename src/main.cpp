#include <glad/glad.h>
#include <platform/win32.h>
#include <engine/input.h>
#include <engine/shader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>
#include <algorithm>

struct Vertex
{
    vec2 pos;
    vec2 uv;
    vec4 color;
};
struct Glyph
{
    ivec2 size;
    ivec2 bearing;
    u32 advance;
    ivec2 atlasPos;
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

std::map<char, Glyph> glyphs;
bool LoadFont(int width, str filepath = "assets/fonts/arial.ttf")
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("Failed to init FreeType\n");
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, filepath, 0, &face))
    {
        printf("Failed to load font\n");
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, 48); // height = 48px

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    int x = 0;
    int y = 0;
    int rowHeight = 0;
    int padding = 2;

    for (unsigned char c = 32; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            continue;

        if (x + face->glyph->bitmap.width + padding >= width)
        {
            x = 0;
            y += rowHeight;
            rowHeight = 0;
        }

        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            x,
            y,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer);

        Glyph g;
        g.size = ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        g.bearing = ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        g.advance = face->glyph->advance.x;
        g.atlasPos = ivec2(x, y);

        glyphs.insert({c, g});

        rowHeight = std::max(rowHeight,
                             static_cast<int>(face->glyph->bitmap.rows) + padding);
        x += face->glyph->bitmap.width + padding;
    }

    return true;
}

array<Batch> batches;

void DrawQuad(Batch &b, vec2 pos, vec2 size, ivec2 ioffset, ivec2 isize, vec4 color = vec4(1.0f), bool flipX = false, bool flipY = false)
{
    u32 startIndex = b.vertices.size();

    // 1. Calculate atlas pixel coordinates
    float x0 = float(ioffset.x);
    float y0 = float(ioffset.y);
    float x1 = float(ioffset.x + isize.x);
    float y1 = float(ioffset.y + isize.y);

    // Handle flipping (useful for sprites, usually false for text)
    if (flipX)
        std::swap(x0, x1);
    if (flipY)
        std::swap(y0, y1);

    // Normalize UVs (0 to 1 range)
    float w = float(b.w);
    float h = float(b.h);

    vec2 uvTL = vec2(x0 / w, y0 / h);
    vec2 uvTR = vec2(x1 / w, y0 / h);
    vec2 uvBR = vec2(x1 / w, y1 / h);
    vec2 uvBL = vec2(x0 / w, y1 / h);

    // 2. Push vertices (Counter-Clockwise order)
    // Using your Ortho: (0,0) is bottom-left, (screen.x, screen.y) is top-right
    b.vertices.push_back(Vertex{.pos = vec2(pos.x, pos.y), .uv = uvBL, .color = color});                   // Bottom-Left
    b.vertices.push_back(Vertex{.pos = vec2(pos.x + size.x, pos.y), .uv = uvBR, .color = color});          // Bottom-Right
    b.vertices.push_back(Vertex{.pos = vec2(pos.x + size.x, pos.y + size.y), .uv = uvTR, .color = color}); // Top-Right
    b.vertices.push_back(Vertex{.pos = vec2(pos.x, pos.y + size.y), .uv = uvTL, .color = color});          // Top-Left

    // 3. Push indices
    b.indices.push_back(startIndex + 0);
    b.indices.push_back(startIndex + 1);
    b.indices.push_back(startIndex + 2);
    b.indices.push_back(startIndex + 2);
    b.indices.push_back(startIndex + 3);
    b.indices.push_back(startIndex + 0);
};

void DrawRect(vec2 pos, vec2 size, ivec2 ioffset, ivec2 isize, vec4 color = vec4(1.0f), bool flipX = false, bool flipY = false)
{
    DrawQuad(
        batches[0], pos,
        size, ioffset,
        isize, color,
        flipX, flipY);
}

void DrawCircle(vec2 pos, float radius, int segment = 8, vec4 color = vec4(1.0f))
{
    array<Vertex>& vertices = batches[0].vertices;
    array<u32>& indices = batches[0].indices;
    u32 startIndex = vertices.size();

    // Center vertex
    vertices.push_back(
        Vertex{
            .pos = pos,
            .uv = vec2(0.0f),
            .color = color
        }
    ); 
    
    // Outer circle vertices
    for(int i = 0; i < segment; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(segment);
        float x = radius * cosf(theta);
        float y = radius * sinf(theta);
        
        vertices.push_back(
            Vertex{
                .pos = pos + vec2(x, y),   // important: offset by center
                .uv = vec2(0.0f),
                .color = color
            }
        ); 
    }
    
    // Indices (triangle fan)
    for(int i = 0; i < segment; i++) {
        u32 current = startIndex + 1 + i;
        u32 next    = startIndex + 1 + ((i + 1) % segment);

        indices.push_back(startIndex);  // center
        indices.push_back(current);
        indices.push_back(next);
    }
}

void RenderText(std::string text, float x, float y, float scale, vec4 color)
{
    for (char c : text)
    {
        Glyph g = glyphs[c];

        float xpos = x + g.bearing.x * scale;
        float ypos = y - (g.size.y - g.bearing.y) * scale;

        float w = g.size.x * scale;
        float h = g.size.y * scale;

        DrawQuad(
            batches[1],
            vec2(xpos, ypos),
            vec2(w, h),
            g.atlasPos,
            g.size,
            color);

        x += (g.advance >> 6) * scale;
    }
}

int main()
{
    InitPlatform();
    CreateWindowPlatform("atlas - engine", 956, 540);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    u32 program = CreateShaderProgram(
        "assets/shaders/scene.vert",
        "assets/shaders/scene.frag");

    glUseProgram(program);
    SetUniform(program, "atlasTexture", 0);

    // create one batch
    batches.push_back(Batch{}); // world / scene
    batches.push_back(Batch{}); // ui

    auto &world = batches[0];
    auto &ui = batches[1];
    world.reserve(1000); // reserve space for 1000 quads
    ui.reserve(1000);    // reserve space for 1000 quads
    {
        // load texture
        // stbi_set_flip_vertically_on_load(true);

        glGenTextures(1, &world.tex);
        glBindTexture(GL_TEXTURE_2D, world.tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        unsigned char *data = stbi_load("assets/sprites/sprite.png", &world.w, &world.h, &world.chs, 0);
        if (data)
        {
            GLenum format = GL_RGB;
            if (world.chs == 4)
                format = GL_RGBA;

            glTexImage2D(GL_TEXTURE_2D, 0, format, world.w, world.h, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            ui.w = world.w;
            ui.h = world.h;
        }
        stbi_image_free(data);
    }

    {
        // ui
        glGenTextures(1, &ui.tex);
        glBindTexture(GL_TEXTURE_2D, ui.tex);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            ui.w,
            ui.h,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    LoadFont(ui.w, "assets/fonts/arial.ttf");
    // create VAO, VBO, EBO
    for (auto &b : batches)
    {
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
    }

    while (!ShouldClose())
    {
        Event event;
        PollEvent(&event);

        // update batch
        DrawRect(vec2(0.0f), vec2(100.0f), ivec2(16, 0), ivec2(16), vec4(1.0f, 0.0f, 1.0f, 1.0f), true, true);
        DrawCircle(vec2(100.0f), 10.0f);

        RenderText("Hello, World!", 0, 0, 1.0f, vec4(1.0f));

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, input->screen.x, input->screen.y);

        glUseProgram(program);
        mat4 proj = mat4::Ortho(0, input->screen.x, 0, input->screen.y, -1, 1);
        SetUniform(program, "projection", proj);
        // render a world
        {
            glEnable(GL_DEPTH_TEST);
            SetUniform(program, "isText", false);
            auto &b = world;
            glBindVertexArray(b.vao);
            glBindTexture(GL_TEXTURE_2D, b.tex);

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
        // render ui
        {
            glDisable(GL_DEPTH_TEST);
            SetUniform(program, "isText", true);
            auto &b = ui;
            glBindVertexArray(b.vao);
            glBindTexture(GL_TEXTURE_2D, b.tex);

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

    for (auto &b : batches)
    {
        glDeleteBuffers(1, &b.vbo);
        glDeleteBuffers(1, &b.ebo);
        glDeleteVertexArrays(1, &b.vao);
    }
    glDeleteShader(program);
    DestroyPlatform();
}
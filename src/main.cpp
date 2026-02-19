#include <glad/glad.h>
#include <platform/win32.h>

#include <engine/input.h>
#include <engine/shader.h>
#include <stb/stb_image.h>

struct alignas(16) Transform
{
    ivec2 ioffset;     // 8 bytes
    ivec2 isize;       // 8 bytes
    vec2 pos;          // 8 bytes
    vec2 size;         // 8 bytes
    vec4 color;        // 16 bytes
    int renderOptions; // 4 bytes
    float layer;       // 4 bytes
    int _padding[2];   // padding for std430 alignment
};

struct Camera2D
{
    vec2 pos;
    vec2 dimensions;   // logical world size
    vec2 framebuffer;  // actual pixel framebuffer

    mat4 matrix()
    {
        return Mat4::Ortho(0.0f, dimensions.x, dimensions.y, 0.0f, -1.0f, 1.0f);
    }
};

int main()
{
    // --- Initialize window and GL ---
    InitPlatform();
    CreateWindowPlatform("atlas - engine", 956, 540);

    u32 shader = CreateShaderProgram("assets/shaders/scene.vert",
                                     "assets/shaders/scene.frag");
    glUseProgram(shader);

    // --- Camera ---
    Camera2D camera;
    camera.dimensions = vec2(input->screen.x, input->screen.y);
    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, camera.matrix().m);

    // --- Load texture atlas ---
    i32 texWidth, texHeight, texChannels;
    u8* imageData = stbi_load("assets/sprites/sample.png", &texWidth, &texHeight, &texChannels, 4);
    Assert(imageData, "Failed to load texture");

    u32 atlasTex;
    glGenTextures(1, &atlasTex);
    glBindTexture(GL_TEXTURE_2D, atlasTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    stbi_image_free(imageData);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader, "textureAtlas"), 0);
    glUniform2f(glGetUniformLocation(shader, "atlasSize"), (float)texWidth, (float)texHeight);
    glUniform1i(glGetUniformLocation(shader, "isFont"), 0);

    // --- Transform instances ---
    Array<Transform> transforms = {};
    transforms.push_back(Transform{
        .ioffset = ivec2(32, 0),
        .isize = ivec2(32),
        .pos = vec2(0.0f),
        .size = vec2(100.0f),
        .color = vec4(1.0f),
        .renderOptions = 0,
        .layer = 0.0f,
        ._padding = {0, 0}
    });

    // --- VAO (unit quad handled via gl_VertexID in shader) ---
    u32 vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindVertexArray(0);

    // --- SSBO ---
    u32 ssbo;
    glGenBuffers(1, &ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 sizeof(Transform) * transforms.size(),
                 transforms.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // --- GL states ---
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // --- Main loop ---
    while (!ShouldClose())
    {
        Event event;
        PollEvent(&event);
        if (IsKeyPressed(KEY_ESCAPE))
            break;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update SSBO if transforms changed
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Transform) * transforms.size(), transforms.data());

        glUseProgram(shader);
        glBindVertexArray(vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, atlasTex);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, (GLsizei)transforms.size());
        glBindVertexArray(0);

        SwapBuffersWindow();
    }

    // --- Cleanup ---
    glDeleteBuffers(1, &ssbo);
    glDeleteTextures(1, &atlasTex);
    glDeleteProgram(shader);

    DestroyPlatform();
}

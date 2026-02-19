#include <platform/win32.h>
#include <engine/input.h>
#include <glad/glad.h>
#include <engine/shader.h>

int main() {
    InitPlatform();
    CreateWindowPlatform("atlas - engine", 956, 450);

    u32 shader = CreateShaderProgram("assets/shaders/scene.vert", "assets/shaders/scene.frag");
    u32 vao;
    glGenVertexArrays(1, &vao);

    while (!ShouldClose()) 
    {
        Event event;
        PollEvent(&event);
        
        glViewport(0, 0, 956, 540);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        if(IsKeyPressed(KEY_ESCAPE)) break;

        SwapBuffersWindow();
    }
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shader);

    DestoryPlatform();
}
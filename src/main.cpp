#include <glad/glad.h>
#include <platform/win32.h>

#include <engine/input.h>
#include <engine/gl_renderer.hpp>

void Update(float dt);
int main()
{
    // --- Initialize window and GL ---
    InitPlatform();
    CreateWindowPlatform("atlas - engine", 956, 540);
    glInitRenderer();
    Array<Vertex> vertices;

    vertices.push_back({
        {-0.5f, -0.5f, 0.0f},    // bottom left
        {1.0f, 0.0f, 0.0f, 1.0f} // red
    });

    vertices.push_back({
        {0.5f, -0.5f, 0.0f},     // bottom right
        {0.0f, 1.0f, 0.0f, 1.0f} // green
    });

    vertices.push_back({
        {0.0f, 0.5f, 0.0f},      // top
        {0.0f, 0.0f, 1.0f, 1.0f} // blue
    });

    glEmit(vertices);

    // --- Main loop ---
    while (!ShouldClose())
    {
        Event event;
        PollEvent(&event);
        Update(event.deltaTime);
        glRender();
        SwapBuffersWindow();
    }

    // --- Cleanup ---
    glClearContext(&gl);
    DestroyPlatform();
}

void step();
void simulate();
void render();

const float FIXED_DELTA = 1.0f / 60.0f;
float accumulator = 0.0f;

void Update(float dt)
{
    accumulator += dt;

    while (accumulator >= FIXED_DELTA)
    {
        step();     // input
        simulate(); // physics
        accumulator -= FIXED_DELTA;
    }

    render(); // render once per frame
}

void step()
{
}

void simulate()
{
}

void render()
{
}
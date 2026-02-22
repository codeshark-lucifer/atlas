#pragma once

#include <engine/shader.h>
#include <stb/stb_image.h>
#include <string>

struct Vertex
{
    vec3 position;
    vec4 color;
};

struct GLContext
{
    u32 vao, vbo;
    u32 shader;

    Array<Vertex> vertices;
    Array<u32> textures;
};

static GLContext gl;

bool glInitRenderer();
void glRender();

void glEmit(Array<Vertex> vertices);

void glClearContext(GLContext *context);
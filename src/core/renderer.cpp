#include <engine/gl_renderer.hpp>
#include <glad/glad.h>

#include <freetype/freetype.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <platform/win32.h>

bool glInitRenderer()
{
    gl.shader = CreateShaderProgram(
        "assets/shaders/scene.vert",
        "assets/shaders/scene.frag");

    glGenVertexArrays(1, &gl.vao);
    glGenBuffers(1, &gl.vbo);

    glBindVertexArray(gl.vao);
    glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);
    glBufferData(GL_ARRAY_BUFFER, gl.vertices.size() * sizeof(Vertex), gl.vertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    return true;
}

void glEmit(Array<Vertex> vertices)
{
    gl.vertices.insert(gl.vertices.end(), vertices.begin(), vertices.end());
    glBindBuffer(GL_ARRAY_BUFFER, gl.vbo);
    glBufferData(GL_ARRAY_BUFFER, gl.vertices.size() * sizeof(Vertex), gl.vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void glRender()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(gl.shader);
    glBindVertexArray(gl.vao);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)gl.vertices.size());
    glBindVertexArray(0);
}

void glClearContext(GLContext *context)
{
    glDeleteBuffers(1, &gl.vbo);
    glDeleteVertexArrays(1, &gl.vao);
}

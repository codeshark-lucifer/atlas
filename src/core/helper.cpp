#include <engine/utils.h>
#include <string.h>
#include <stdarg.h>
#include <glad/glad.h>
#include <iostream>

void print(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
}

char *read_file(str path)
{
    FILE *f = fopen(path, "rb");
    if (!f)
        return nullptr;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = (char *)malloc(size + 1);
    if (!buffer)
    {
        fclose(f);
        return nullptr;
    }

    if (fread(buffer, 1, size, f) != (size_t)size)
    {
        free(buffer);
        fclose(f);
        return nullptr;
    }

    buffer[size] = '\0'; // null-terminate
    fclose(f);
    return buffer;
}

int write_file(str path, const char *buffer, u64 size)
{
    FILE *f = fopen(path, "wb");
    if (!f)
        return 0;

    if (fwrite(buffer, 1, size, f) != (size_t)size)
    {
        fclose(f);
        return 0;
    }

    fclose(f);
    return 1;
}

BumpAllocator MakeAllocator(size_t size)
{
    BumpAllocator alloc{};
    alloc.memory = (char *)malloc(size);
    alloc.capacity = size;
    alloc.used = 0;
    return alloc;
}

inline size_t AlignForward(size_t ptr, size_t align)
{
    size_t mod = ptr & (align - 1);
    if (mod)
        ptr += (align - mod);
    return ptr;
}

void *BumpAllocAligned(BumpAllocator *alloc, size_t size, size_t align)
{
    size_t current = (size_t)alloc->memory + alloc->used;
    size_t aligned = AlignForward(current, align);
    size_t newUsed = aligned - (size_t)alloc->memory + size;

    if (newUsed > alloc->capacity)
        return nullptr;

    alloc->used = newUsed;
    return (void *)aligned;
}

void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

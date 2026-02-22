
#pragma once

class Entity;
struct Component
{
    Entity *entity = nullptr;
    ~Component() { Clean(); }
    virtual void Clean() {}
    virtual void Start() {}
    virtual void Update(float deltaTime) {}
};

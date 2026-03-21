#pragma once
#include <engine/utils.h>
#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>
#include <stdexcept>

using Entity = u32;

// ------------------------------------------------------------
// Base interface ONLY for storage polymorphism (not components)
// ------------------------------------------------------------
struct IComponentArray
{
    virtual ~IComponentArray() = default;
    virtual void Remove(Entity entity) = 0;
};

// ------------------------------------------------------------
// Component storage per type
// ------------------------------------------------------------
template <typename T>
class ComponentArray : public IComponentArray
{
public:
    void Insert(Entity entity, const T& component)
    {
        components[entity] = component;
    }

    void Remove(Entity entity) override
    {
        components.erase(entity);
    }

    T* Get(Entity entity)
    {
        auto it = components.find(entity);
        if (it != components.end())
            return &it->second;
        return nullptr;
    }

    std::unordered_map<Entity, T>& GetAll()
    {
        return components;
    }

private:
    std::unordered_map<Entity, T> components;
};
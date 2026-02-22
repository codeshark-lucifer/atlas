#pragma once
#include "component.h"
#include <engine/utils.h>
#include <atomic>
#include <unordered_map>

#include <memory>
#include <typeindex>
#include <typeinfo>
#include <stdexcept>

using EntityID = u64;

struct Transform : public Component
{
    vec3 position = vec3(0.0f);
    quat rotation = quat::Identity();
    vec3 scale = vec3(1.0f);
};

class Entity
{
public:
    EntityID id = 0;
    std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
    Entity() : id(Generate()) {
        AddComponent<Transform>();
    }

public:
    template <typename T, typename... Args>
    void AddComponent(Args &&...args)
    {
        components[typeid(T)] = std::make_unique<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    T *GetComponent()
    {
        auto it = components.find(typeid(T));
        if (it != components.end())
            return static_cast<T *>(it->second.get());
        return nullptr;
    }

    template <typename T>
    bool HasComponent() const
    {
        return components.find(typeid(T)) != components.end();
    }

private:
    static EntityID Generate()
    {
        return s_Counter.fetch_add(1, std::memory_order_relaxed);
    };

private:
    static std::atomic<EntityID> s_Counter;
};

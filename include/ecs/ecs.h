#pragma once
#include "world.h"

class ECS
{
public:
    // ------------------------------------------------------------
    // Entity management
    // ------------------------------------------------------------
    Entity CreateEntity()
    {
        Entity entity = nextID++;
        aliveEntities.push_back(entity);
        return entity;
    }

    void DestroyEntity(Entity entity)
    {
        for (auto& [type, store] : componentStores)
        {
            store->Remove(entity);
        }
    }

    // ------------------------------------------------------------
    // Component management
    // ------------------------------------------------------------
    template<typename T>
    void AddComponent(Entity entity, const T& component)
    {
        GetOrCreateStore<T>()->Insert(entity, component);
    }

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        GetOrCreateStore<T>()->Remove(entity);
    }

    template<typename T>
    T* GetComponent(Entity entity)
    {
        return GetOrCreateStore<T>()->Get(entity);
    }

    template<typename T>
    bool HasComponent(Entity entity)
    {
        return GetComponent<T>(entity) != nullptr;
    }

    template<typename T>
    std::unordered_map<Entity, T>& GetAllComponents()
    {
        return GetOrCreateStore<T>()->GetAll();
    }

private:
    Entity nextID = 0;
    std::vector<Entity> aliveEntities;

    std::unordered_map<std::type_index, std::unique_ptr<IComponentArray>> componentStores;

    template<typename T>
    ComponentArray<T>* GetOrCreateStore()
    {
        std::type_index type = std::type_index(typeid(T));

        auto it = componentStores.find(type);
        if (it == componentStores.end())
        {
            auto store = std::make_unique<ComponentArray<T>>();
            ComponentArray<T>* ptr = store.get();
            componentStores[type] = std::move(store);
            return ptr;
        }

        return static_cast<ComponentArray<T>*>(componentStores[type].get());
    }
};
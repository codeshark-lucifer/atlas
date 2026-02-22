#pragma once
#include "entity.h"
#include <unordered_map>
#include <memory>

using EntityMap = std::unordered_map<EntityID, std::unique_ptr<Entity>>;


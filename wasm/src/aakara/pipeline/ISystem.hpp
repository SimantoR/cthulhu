#ifndef ISYSTEM_HPP
#define ISYSTEM_HPP

#include <entt/entity/registry.hpp>

class ISystem
{
public:
    virtual void Execute(entt::registry* entities) = 0;
};

#endif

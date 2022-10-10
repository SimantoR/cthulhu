#ifndef RENDERPIPELINE_HPP
#define RENDERPIPELINE_HPP

#include <utils.h>
#include <entt/entity/registry.hpp>

class Scene;
class ISystem;

/**
 * @brief A pipeline that renders objects at the end
 *
 */
class RenderPipeline
{
public:
    RenderPipeline( const Array<Ptr<ISystem>>& systems );

    void Run( entt::registry* entities );

private:
    std::vector<Ptr<ISystem>> m_systems;
};

#endif

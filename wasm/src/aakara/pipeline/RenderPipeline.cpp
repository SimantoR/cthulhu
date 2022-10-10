#include "RenderPipeline.hpp"
#include "ISystem.hpp"
#include <aakara/Scene.hpp>

#include <entt/entt.hpp>

RenderPipeline::RenderPipeline( const Array<Ptr<ISystem>>& systems )
    : m_systems( systems )
{
}

void RenderPipeline::Run( entt::registry* entities )
{
    for ( Ptr<ISystem> system : m_systems )
        system->Execute( entities );
}

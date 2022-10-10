#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include "ISystem.hpp"
#include <utils.h>

class Camera;
class Light;

class RenderSystem : public ISystem
{
public:
    RenderSystem( Ptr<Camera> cam, Ptr<Light> light );

    void Execute( entt::registry* entities ) override;
};

#endif

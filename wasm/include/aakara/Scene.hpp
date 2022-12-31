#ifndef SCENE_HPP
#define SCENE_HPP

#include <iostream>
#include <utils.h>
#include <entt/entt.hpp>
#include <entt/entity/registry.hpp>
#include <map>

class Camera;
class Mesh;

class Scene
{
public:
    Scene();
    ~Scene();

    /**
     * @brief Load a scene from file.
     *
     * @param filepath Path to file in filesystem.
     */
    static void Load( const string& filepath );

private:
    entt::registry         m_entityRegistery;
    std::map<string, Mesh> m_meshRegistry;
};

#endif

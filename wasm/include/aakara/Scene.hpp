#ifndef SCENE_HPP
#define SCENE_HPP

#include "SceneObject.hpp"
#include "Camera.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"

#include <utils.h>
#include <entt/entity/registry.hpp>

struct Node
{
    string id;
    Node*  parent = nullptr;

    Node()
    {
    }

    Node( string id, Node* parent )
    {
        this->id     = id;
        this->parent = parent;
    }
};

class Scene
{
public:
    Scene();
    static Ptr<Scene> LoadScene( std::string filepath );

    void removeNode( Node* node );

private:
    Ptr<SceneObject>    m_rootNode = nullptr;
    Array<Ptr<Camera>>  m_cameras;
    Array<Ptr<Mesh>>    m_meshes;
    Array<Ptr<Texture>> m_textures;
    entt::registry      m_registry;
};

#endif

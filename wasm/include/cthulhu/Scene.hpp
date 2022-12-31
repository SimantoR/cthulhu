#ifndef SCENE_HPP
#define SCENE_HPP

#include "../utils.h"
#include "../aakara/Mesh.hpp"
#include "../aakara/Texture.hpp"
#include "../aakara/Transform.hpp"
#include "../aakara/Skybox.hpp"
#include "../aakara/Shader.hpp"

struct Node_t;

class Scene
{
public:
    Vector<Ptr<Mesh>>    m_Meshes;
    Vector<Ptr<Texture>> m_Textures;
    Vector<Ptr<Skybox>>  m_Skyboxes;
    Vector<Node_t*>      m_RootNode;
    Shader*              m_Shader   = nullptr;

    ~Scene();

    static Scene LoadFromFile( const string& filepath );
};

#endif

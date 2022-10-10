#ifndef MESH_HPP
#define MESH_HPP

#include <utils.h>
#include <glm/glm.hpp>
#include <emscripten/val.h>

template <typename T> using Array = std::vector<T>;

class Shader;

class Mesh
{
public:
    std::vector<glm::vec3> Positions;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec2> UVMap;
    std::vector<u16>       Indices;

    Mesh();
    Mesh( Array<glm::vec3>& pos, Array<glm::vec3>& norm, Array<u16>& indices, Array<glm::vec2>& uvmap );
    ~Mesh();

    bool Bind( Shader* shader );
    void Unbind();

    bool update( Shader* shader );

    void Draw();

    static void                   LoadFromURL( const std::string& url, emscripten::val onLoad );
    static std::vector<Ptr<Mesh>> LoadFromFile( Shader* shader, const std::string& url );
    static Ptr<Mesh>              LoadFromMemory( const char* data, u32 size );

    static Ptr<Mesh> Create( Shader* shader, const Array<glm::vec3>& pos, const Array<glm::vec3>& norm,
        const Array<u16> indices, const Array<glm::vec2>& uvmap );

    u32 VBO = 0, NBO = 0, IBO = 0, CBO = 0;

private:
    /**
     * @brief Reduce polycount of mesh.
     * @param ratio Ratio of compression. Value between 0.0 - 1.0
     */
    void Optimize( f32 ratio = 0.5f );

    bool m_isOptimized = false;
};

#endif

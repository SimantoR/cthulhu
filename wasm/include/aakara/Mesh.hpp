#ifndef MESH_HPP
#define MESH_HPP

#include <array>
#include <utils.h>
#include <glm/glm.hpp>
#include <emscripten/val.h>

class Shader;
class Vertex;

class Mesh
{
public:
    Vector<glm::vec3> Vertices;
    Vector<glm::vec3> Normals;
    Vector<glm::vec2> UVMap;
    Vector<u16>       Indices;
    Vector<glm::vec4> Tangents;

    Mesh();
    Mesh( Vector<glm::vec3>& verts, Vector<glm::vec3>& norm, Vector<u16>& indices, Vector<glm::vec2>& uvmap );
    ~Mesh();

    bool Bind( Shader* shader );
    void Unbind();

    bool update( Shader* shader );

    void Draw();

    static void              LoadFromURL( const std::string& url, emscripten::val onLoad );
    static Vector<Ptr<Mesh>> LoadFromFile( Shader* shader, const std::string& url );
    static Ptr<Mesh>         LoadFromMemory( const char* data, u32 size );

    /**
     * @brief Create a mesh.
     *
     * @param shader Shader to bind the mesh to.
     * @param pos Vertices of mesh.
     * @param norm Normals of triangles
     * @param indices Index array defining triangles in vertex array.
     * @param uvmap 2D map indicating which vertex is mapped to which point on a texture.
     * @return Ptr<Mesh> A pointer to created mesh
     */
    static Ptr<Mesh> Create( Shader* shader, const Vector<glm::vec3>& pos, const Vector<glm::vec3>& norm,
        const Vector<u16> indices, const Vector<glm::vec2>& uvmap );

    u32 VBO = 0, NBO = 0, IBO = 0, CBO = 0;

private:
    /**
     * @brief Reduce polycount of mesh.
     * @param ratio Ratio of compression. Value between 0.0 - 1.0
     */
    void Optimize( f32 ratio = 0.5f );

    void computeTangent();

    bool m_isOptimized = false;
};

#endif

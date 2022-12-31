#ifndef MESH_SIMPLIFIER_HPP
#define MESH_SIMPLIFIER_HPP

#include "../utils.h"
#include <glm/vec3.hpp>

class Mesh;
class Triangle;
class Vertex;
struct Ref;

class MeshSimplifier
{
public:
    u32 m_MaxIterations     = 100;
    f32 m_Aggressiveness    = 7;
    u8  m_TriangleEdgeCount = 3;
    u8  m_UVChannelCount    = 3;

    MeshSimplifier();
    Ptr<Mesh> Simplify( const Mesh& mesh, f32 quality = 0.5f );

private:
    Vector<Triangle> m_triangles;
    Vector<Vertex>   m_vertices;
    Vector<Ref>      m_refs;

    bool      flipped( glm::vec3 p, int i0, int i1, Vertex& v0, Vertex& v1, std::vector<u32>& deleted );
    void      compact_mesh();
    void      update_triangles( u32 i0, Vertex& v, std::vector<u32>& deleted, u32& deleted_triangles );
    Ptr<Mesh> generate_mesh();
    void      transform_mesh( const Mesh* mesh );
};

#endif

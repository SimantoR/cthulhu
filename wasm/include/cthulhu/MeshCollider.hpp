#ifndef MESH_COLLIDER_HPP
#define MESH_COLLIDER_HPP

#include "Ray.hpp"
#include "../utils.h"
#include "../aakara/Mesh.hpp"
#include <glm/vec3.hpp>

class Triangle;
class Vertex;
struct Ref;

struct MeshOptimizationOptions
{
    u32 MaxIterations     = 100;
    f32 Aggressiveness    = 7;
    u8  TriangleEdgeCount = 3;
    u8  UVChannelCount    = 3;
    f32 Quality           = 0.5f;
};

class MeshCollider
{
public:
    MeshCollider( Ptr<Mesh> originMesh, MeshOptimizationOptions options = {} );

    bool Intersects( const Ray* ray ) const;

    Ptr<Mesh> generate_mesh() const;

private:
    Vector<Triangle>        m_triangles;
    Vector<Vertex>          m_vertices;
    Vector<Ref>             m_refs;
    MeshOptimizationOptions m_options;

    bool flipped( glm::vec3 p, int i0, int i1, Vertex& v0, Vertex& v1, std::vector<u32>& deleted );
    void compact_mesh();
    void update_triangles( u32 i0, Vertex& v, std::vector<u32>& deleted, u32& deleted_triangles );
    void transform_mesh( const Mesh* mesh );
};

#endif

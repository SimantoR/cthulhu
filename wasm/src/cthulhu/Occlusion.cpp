#include <climits>
#include <aakara/Transform.hpp>
#include <cthulhu/Occlusion.hpp>
#include <cthulhu/MeshCollider.hpp>
#include <cthulhu/Ray.hpp>
#include <uuid.hpp>

using namespace Octree;

glm::vec3 getCenter( Mesh* mesh );

namespace Octree
{
    node_t* Generate( const std::map<MeshCollider, const Transform> scene )
    {
        // TODO: Find the overall scene bounds

        glm::vec3 top_corner( 0 );
        glm::vec3 bottom_corner( 0 );

        /* ------------------- Find the overall size of the scene ------------------- */
        f32 highX = static_cast<f32>( INT_MIN ), lowX = static_cast<f32>( INT_MAX );
        f32 highY = static_cast<f32>( INT_MIN ), lowY = static_cast<f32>( INT_MAX );
        f32 highZ = static_cast<f32>( INT_MIN ), lowZ = static_cast<f32>( INT_MAX );

        bounds_t* sceneBounds = new bounds_t();
        for ( const auto& [collider, transform] : scene )
        {
            Ptr<Mesh> collisionMesh = collider.generate_mesh();
            // collisionM<
            // glm::vec3 center        = getCenter( collisionMesh.get() );

            for ( glm::vec3& vertex : collisionMesh->Vertices )
            {
                if ( vertex.x > highX )
                    highX = vertex.x;
                if ( vertex.x < lowX )
                    lowX = vertex.x;

                if ( vertex.y > highY )
                    highY = vertex.y;
                if ( vertex.y < lowY )
                    lowY = vertex.y;

                if ( vertex.z > highZ )
                    highZ = vertex.z;
                if ( vertex.y < lowX )
                    lowZ = vertex.z;
            }
        }

        node_t*   rootNode   = new node_t();
        bounds_t* meshBounds = rootNode->bounds;

        // meshBounds.center = center;
        meshBounds->width  = highX - lowX;
        meshBounds->height = highY - lowY;
        meshBounds->depth  = highZ - lowZ;

        return nullptr;
    }

    node_t* Query( const Ray& ray, node_t* treeNode )
    {
        return nullptr;
    }

    void ClearTree( node_t* root )
    {
    }
}

glm::vec3 getCenter( Mesh* mesh )
{
    glm::vec3 centroid;
    f32       volume = 0.0f;

    auto& vertices = mesh->Vertices;
    auto& indices  = mesh->Indices;

    for ( u32 i = 0; i < mesh->Indices.size(); i += 3 )
    {
        glm::vec3 v1 = vertices[indices[i + 0]];
        glm::vec3 v2 = vertices[indices[i + 1]];
        glm::vec3 v3 = vertices[indices[i + 2]];

        f32 tetrahedronVolume = glm::dot( v1, glm::cross( v2, v3 ) ) / 6.0f;
        centroid += tetrahedronVolume * ( v1 + v2 + v3 ) / 4.0f;
        volume += tetrahedronVolume;
    }

    return centroid / volume;
}

Array<bounds_t, 8> subdivide( const Octree::bounds_t& bounds )
{
    Array<bounds_t, 8> children;

    for ( u8 i = 0; i < 8; i++ )
    {
        bounds_t& child = children[i];
        child.width     = bounds.width / 2;
        child.height    = bounds.height / 2;
        child.depth     = bounds.depth / 2;
    }

    return children;
}

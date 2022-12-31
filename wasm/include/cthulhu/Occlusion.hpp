#ifndef OCCLUSION
#define OCCLUSION

#include <utils.h>
#include <glm/vec3.hpp>

class MeshCollider;
class Transform;
class Ray;

namespace Octree
{
    struct bounds_t
    {
        glm::vec3 center = glm::vec3( 0 );
        f32       width  = 0.0f;
        f32       height = 0.0f;
        f32       depth  = 0.0f;
    };

    struct node_t
    {
        u16               level  = 0;
        bounds_t*         bounds = nullptr;
        Array<node_t*, 8> children;
    };

    node_t* Generate( const std::map<MeshCollider, const Transform> scene );

    /**
     * @brief Query occlusion tree find leaf node where ray hits
     *
     * @param ray Ray to check intersection against
     * @param treeNode Root node of octree to check against
     * @return node_t* Pointer to node that was hit. Null pointer if none.
     */
    node_t* Query( const Ray& ray, node_t* treeNode );

    void ClearTree( node_t* root );
}

#endif

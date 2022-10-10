#ifndef PART_HPP
#define PART_HPP

#include <utils.h>

class Mesh;
class Texture;
class Transform;

struct Part
{
    string         id;
    Ptr<Mesh>      mesh;
    Ptr<Texture>   texture;
    Ptr<Transform> transform;

    Part();
    Part( const Part& other );
    Part( const string& id, Ptr<Mesh> mesh, Ptr<Texture> texture, Ptr<Transform> transform );
};

#endif

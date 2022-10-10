#ifndef SCENEOBJECT_HPP
#define SCENEOBJECT_HPP

#include <utils.h>
#include "Transform.hpp"
#include "Part.hpp"

class SceneObject
{
public:
    SceneObject() {};
    SceneObject( std::string name );
    ~SceneObject();

    std::string GetID();
    std::string GetName();

    std::string             id;
    std::string             name;
    Ptr<Part>               part      = nullptr;
    Ptr<Transform>          transform = nullptr;
    Array<Ptr<SceneObject>> children;

private:
    std::string m_id;
};

#endif

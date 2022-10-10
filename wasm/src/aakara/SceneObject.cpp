#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <aakara/SceneObject.hpp>
#include <uuid.hpp>

SceneObject::SceneObject( std::string name )
    : m_id( uuid::generate() )
    , name( name )
    , children()
{
    this->transform = std::make_shared<Transform>();
}

SceneObject::~SceneObject()
{
    this->children.clear();
}

// emscripten::val getChildren( Ptr<SceneObject> sceneObj )
// {
//     Array<SceneObject> children;

//     for ( size_t i = 0; i < children.size(); i++ )
//         children.push_back( *sceneObj->children[i].get() );

//     return emscripten::val::array( children );
// }

EMSCRIPTEN_BINDINGS( SCENEOBJECT_HPP )
{
    emscripten::value_object<SceneObject>( "SceneObject" )
        .field( "id", &SceneObject::id )
        // .field( "children", &getChildren )
        .field( "name", &SceneObject::name );
}

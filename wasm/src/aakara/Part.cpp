#include <emscripten/bind.h>
#include <aakara/Part.hpp>

Part::Part()
{
}

Part::Part( const Part& other )
    : id( other.id )
    , mesh( other.mesh )
    , texture( other.texture )
    , transform( other.transform )
{
}

Part::Part( const string& id, Ptr<Mesh> mesh, Ptr<Texture> texture, Ptr<Transform> transform )
    : id( id )
    , mesh( mesh )
    , texture( texture )
    , transform( transform )
{
}

/* -------------------------------- Bindings -------------------------------- */
string getId( Ptr<Part> part )
{
    return part->id;
}

Ptr<Transform> getTransform( Ptr<Part> part )
{
    return part->transform;
}

EMSCRIPTEN_BINDINGS( PART_HPP )
{
    emscripten::class_<Part>( "Part" )
        .smart_ptr_constructor( "Part", &std::make_shared<Part> )
        .function( "getId", &getId )
        .function( "getTransform", &getTransform );
}

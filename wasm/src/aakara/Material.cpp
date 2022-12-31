/*

*/

#include <aakara/Material.hpp>
#include <aakara/Shader.hpp>
#include <webgl/webgl2.h>
#include <emscripten/val.h>

void applyMaterial( emscripten::val updateFn, Shader* shader )
{
    updateFn( shader );
}

#include <utils.h>
#include <emscripten/bind.h>
#include <emscripten.h>

#include <cthulhu/App.hpp>

#pragma region FunctionSignatures

void init();
void drawcall();

#pragma endregion

#pragma region Globals
App*           g_app = nullptr;
#pragma endregion

int main()
{
    emscripten_set_main_loop( drawcall, 30, 1 );
}

void init( string canvas_id, int width, int height )
{
    if ( g_app != nullptr )
        delete g_app;

    g_app = new App( canvas_id, width, height );
}

void drawcall()
{
}

EMSCRIPTEN_BINDINGS(PROGRAM)
{
    
}

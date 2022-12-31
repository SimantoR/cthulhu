#include <queue>
#include <aakara/fetch.hpp>

std::map<std::string, std::function<void( emscripten_fetch_t* )>> cbs;
std::queue<string>                                                fetchQueue;

void onResponse( emscripten_fetch_t* fetch );

struct Header
{
    string Key;
    string Value;
};

struct FetchOptions
{
    std::vector<Header>                              headers;
    std::function<void( emscripten_fetch_t* )> callback;
};

namespace HTTP
{
    void GET( const std::string& url, std::function<void( emscripten_fetch_t* )> cb )
    {
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init( &attr );
        std::strcpy( attr.requestMethod, "GET" );
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;

        attr.onsuccess = onResponse;
        attr.onerror   = onResponse;

        cbs[url] = cb;

        // emscripten_console_log( "Starting fetch..." );
        emscripten_fetch( &attr, url.c_str() );
    }

    void POST( const std::string& url, FetchOptions options )
    {
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init( &attr );
        std::strcpy(attr.requestMethod, "POST");
        
        // attach request headers
        
    }
}

void onResponse( emscripten_fetch_t* fetch )
{
    std::string url( fetch->url );

    auto func = cbs[url];
    func( fetch );

    cbs.erase( url );
}

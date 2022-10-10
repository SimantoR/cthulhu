#ifndef FETCH_H
#define FETCH_H

#include <utils.h>
#include <functional>
#include <emscripten/fetch.h>

namespace HTTP
{
    /**
     * @brief Async GET HTTP request given the URL.
     *
     * @param url URL to submit HTTP request to.
     * @param cb Callback that returns the response of the HTTP call.
     */
    void GET( const string& url, std::function<void( emscripten_fetch_t* )> cb );
}

#endif

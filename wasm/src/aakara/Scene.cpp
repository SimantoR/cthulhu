#include <fstream>
#include <memory>
#include <filesystem>
#include <future>
#include <zlib.h>
#include <zstr.hpp>
#include <aakara/Scene.hpp>
#include <aakara/Mesh.hpp>
#include <aakara/Camera.hpp>
#include <uuid.hpp>

namespace fs = std::filesystem;

#define BUFFERSIZE 1024

/**
 * @brief Extract tar content to a specified directory
 *
 * @param tar_path
 * @param extract_dir
 */
void extract_tar( const fs::path& tar_path, const fs::path& extract_dir );

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::Load( const string& filepath )
{
    string         file_id  = uuid::generate();
    const fs::path temp_dir = fs::path( "/tmp" ) / file_id;
    const fs::path tar_path = temp_dir / "container.tar";

    if ( !fs::exists( temp_dir ) )
        fs::create_directories( temp_dir );

    zstr::ifstream unzip_stream( filepath );
    std::ofstream  tar_stream( tar_path );

    char* buffer      = new char[BUFFERSIZE];
    u16   buffer_size = BUFFERSIZE;
    u64   byte_loaded = 0;

    while ( unzip_stream.eof() == false )
    {
        // if remaining stream is less than buffer size
        if ( unzip_stream.gcount() - byte_loaded < 1024 )
            buffer_size = unzip_stream.gcount() - byte_loaded;

        // transfer unzipped_stream to tar stream
        unzip_stream.read( buffer, buffer_size );
        tar_stream.write( buffer, buffer_size );
        byte_loaded += buffer_size;
    }
    delete[] buffer;

    unzip_stream.close();
    tar_stream.close();

    // TODO: extract tar content to filesystem
    extract_tar( tar_path, temp_dir );
    fs::remove( tar_path );
}

void extract_tar( const fs::path& tar_path, const fs::path& extract_dir )
{
    if ( !fs::exists( extract_dir ) )
        fs::create_directories( extract_dir );
}

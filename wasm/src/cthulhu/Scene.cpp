#include <cthulhu/Scene.hpp>
#include <filesystem>
#include <fstream>
#include <rapidjson/document.h>

namespace fs = std::filesystem;

using JsonNode = rapidjson::Value;

struct Node_t
{
    string          name;
    string          meshID;
    string          textureID;
    Transform       transform;
    Vector<Node_t*> children;
};

rapidjson::Document read_json( const fs::path path );

Scene Scene::LoadFromFile( const string& filepath )
{
    if ( !fs::exists( filepath ) )
        throw std::runtime_error( "Scene file does not exist in path: " + filepath );

    Scene scene;

    std::map<string, Ptr<Mesh>>    meshes;
    std::map<string, Ptr<Texture>> textures;

    // TODO: Unarchive tar file and dump content in a temporary folder
    fs::path archive_path( "" );

    /* --------------- Read the unarchived files into Scene object -------------- */
    fs::path scenefile_path = archive_path / "scene.json";

    for ( const fs::directory_entry& entry : fs::directory_iterator( archive_path / "meshes" ) )
    {
        if ( entry.is_directory() )
            continue;

        // Get filename without extension
        string mesh_uuid = entry.path().stem();

        // Load all mesh
        for ( auto mesh : Mesh::LoadFromFile( scene.m_Shader, entry.path().string() ) )
            meshes[mesh_uuid] = mesh;
    }

    for ( const fs::directory_entry& entry : fs::directory_iterator( archive_path / "textures" ) )
    {
        if ( entry.is_directory() )
            continue;

        fs::path entry_path    = entry.path();
        string   texture_uuid  = entry_path.stem();
        textures[texture_uuid] = Texture::LoadFromFile( entry_path.string() );
    }

    /* ------------------------- Read scene information ------------------------- */
    rapidjson::Document json = read_json( archive_path / "scene.json" );

    assert( json["nodes"].IsArray() );
    auto nodes = json["nodes"].GetArray();

    std::function<Node_t*( JsonNode& )> parseNode = [&parseNode]( JsonNode& node )
    {
        Node_t* node_data = new Node_t();

        string&    node_id    = node_data->name;
        string&    mesh_id    = node_data->meshID;
        string&    texture_id = node_data->textureID;
        Transform& transform  = node_data->transform;

        node_id = node["name"].GetString();

        if ( node.HasMember( "mesh" ) )
            mesh_id = node["mesh"].GetString();

        if ( node.HasMember( "texture" ) )
            texture_id = node["texture"].GetString();

        if ( node.HasMember( "transform" ) )
        {
            transform.position.x = node["transform"]["position"]["x"].GetFloat();
            transform.position.y = node["transform"]["position"]["y"].GetFloat();
            transform.position.z = node["transform"]["position"]["z"].GetFloat();

            transform.rotation.x = node["transform"]["rotation"]["x"].GetFloat();
            transform.rotation.y = node["transform"]["rotation"]["y"].GetFloat();
            transform.rotation.z = node["transform"]["rotation"]["z"].GetFloat();

            transform.scale.x = node["transform"]["scale"]["x"].GetFloat();
            transform.scale.y = node["transform"]["scale"]["y"].GetFloat();
            transform.scale.z = node["transform"]["scale"]["z"].GetFloat();
        }

        if ( node.HasMember( "children" ) )
        {
            for ( JsonNode& child : node["children"].GetArray() )
                node_data->children.push_back( parseNode( child ) );
        }

        return node_data;
    };

    for ( JsonNode& node : nodes )
        scene.m_RootNode.push_back( parseNode( node ) );
}

rapidjson::Document read_json( const fs::path path )
{
    assert( fs::exists( path ) );

    std::ifstream json_stream( path );
    assert( json_stream.is_open() );

    u64  stream_size = json_stream.tellg();
    char buffer[stream_size];

    json_stream.read( buffer, stream_size );

    rapidjson::Document document;
    document.Parse( buffer, stream_size );

    return std::move( document );
}

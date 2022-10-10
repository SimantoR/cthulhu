#include <aakara/Mesh.hpp>
#include <aakara/Shader.hpp>
#include <aakara/fetch.hpp>
#include <sstream>
#include <webgl/webgl1.h>
#include <emscripten/fetch.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/vec3.hpp>

void updateNormals( Array<glm::vec3>& pos, Array<glm::vec3>& norms, Array<u16> indices );

Mesh::Mesh()
    : Positions()
    , Normals()
    , Indices()
    , UVMap()
{
}

Mesh::Mesh( Array<glm::vec3>& pos, Array<glm::vec3>& norm, Array<u16>& indices, Array<glm::vec2>& uvmap )
    : Positions( pos )
    , Normals( norm )
    , Indices( indices )
    , UVMap( uvmap )
{
}

Mesh::~Mesh()
{
    u32 bufferIds[] = { VBO, NBO, IBO };
    glDeleteBuffers( 1, bufferIds );

    VBO = 0, IBO = 0;
}

bool Mesh::Bind( Shader* shader )
{
    u32 shaderId       = shader->GetID();
    int attribLocation = 0;

    // emscripten_console_logf( "VBO: %u", m_VBO );
    if ( !VBO || !IBO || !NBO )
        return false;

    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    attribLocation = glGetAttribLocation( shaderId, "v_position" );
    glEnableVertexAttribArray( attribLocation );

    glVertexAttribPointer( attribLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), nullptr );

    // Activate normal buffer
    glBindBuffer( GL_ARRAY_BUFFER, NBO );
    attribLocation = glGetAttribLocation( shaderId, "v_normal" );
    glEnableVertexAttribArray( attribLocation );

    glVertexAttribPointer( attribLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( float ), nullptr );

    // Activate uv buffer
    glBindBuffer( GL_ARRAY_BUFFER, CBO );
    attribLocation = glGetAttribLocation( shaderId, "v_uv" );
    glEnableVertexAttribArray( attribLocation );

    glVertexAttribPointer( attribLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof( float ), nullptr );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, IBO );

    return true;
}

void Mesh::Unbind()
{
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void Mesh::Draw()
{
    glDrawElements( GL_TRIANGLES, Indices.size(), GL_UNSIGNED_SHORT, 0 );
}

Ptr<Mesh> Mesh::LoadFromMemory( const char* data, u32 size )
{
    const u32 import_flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_SortByPType
                             | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_OptimizeMeshes
                             | aiProcess_ValidateDataStructure;

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFileFromMemory( data, (size_t)size, import_flags );

    if ( !scene->HasMeshes() )
        throw std::runtime_error( importer.GetErrorString() );

    aiMesh* loadedMesh = scene->mMeshes[0];

    u32 vertexCount = loadedMesh->mNumVertices;
    u32 faceCount   = loadedMesh->mNumFaces;

    std::vector<glm::vec3> positions( vertexCount );
    std::vector<glm::vec3> normals( vertexCount );
    std::vector<glm::vec2> uvmap( vertexCount );
    std::vector<u16>       indices;

    for ( u32 j = 0; j < vertexCount; j++ )
    {
        positions[j] = { loadedMesh->mVertices[j].x, loadedMesh->mVertices[j].y, loadedMesh->mVertices[j].z };
        normals[j]   = { loadedMesh->mNormals[j].x, loadedMesh->mNormals[j].y, loadedMesh->mNormals[j].z };

        uvmap[j] = { loadedMesh->mTextureCoords[0][j].x, loadedMesh->mTextureCoords[0][j].y };
    }

    for ( u32 j = 0; j < loadedMesh->mNumFaces; j++ )
    {
        aiFace& face = loadedMesh->mFaces[j];
        for ( u32 k = 0; k < face.mNumIndices; k++ )
            indices.push_back( (u16)face.mIndices[k] );
    }

    return std::make_shared<Mesh>( positions, normals, indices, uvmap );
}

Ptr<Mesh> Mesh::Create( Shader* shader, const Array<glm::vec3>& pos, const Array<glm::vec3>& norm,
    const Array<u16> indices, const Array<glm::vec2>& uvmap )
{
    if ( !pos.size() || !norm.size() || !indices.size() )
        throw std::runtime_error( "Mesh is invalid" );

    Ptr<Mesh> mesh = std::make_shared<Mesh>();

    mesh->Positions = pos;
    mesh->Normals   = norm;
    mesh->Indices   = indices;
    mesh->UVMap     = uvmap;

    std::vector<float> vertexBuffer;
    std::vector<float> normalBuffer;
    std::vector<float> uvBuffer;

    for ( u32 i = 0; i < pos.size(); i++ )
    {
        vertexBuffer.push_back( pos[i].x );
        vertexBuffer.push_back( pos[i].y );
        vertexBuffer.push_back( pos[i].z );

        normalBuffer.push_back( norm[i].x );
        normalBuffer.push_back( norm[i].y );
        normalBuffer.push_back( norm[i].z );

        uvBuffer.push_back( uvmap[i].x );
        uvBuffer.push_back( uvmap[i].y );

        // float x = uvmap[i].x;
        // float y = uvmap[i].y;

        // if ( x > 1.0f )
        //     while ( x > 1.0f )
        //         x--;
        // else if ( x < 0.0f )
        //     while ( x < 0.0f )
        //         x++;

        // if ( y > 1.0f )
        //     while ( y > 1.0f )
        //         y--;
        // else if ( y < 0.0f )
        //     while ( y < 0.0f )
        //         y++;

        // uvBuffer.push_back( x );
        // uvBuffer.push_back( y );
    }

    u32 attribLocation = 0;

    u32 vbo = 0, nbo = 0, ibo = 0, cbo = 0;

    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData(
        GL_ARRAY_BUFFER, sizeof( float ) * vertexBuffer.size(), vertexBuffer.data(), GL_STATIC_DRAW );

    glGenBuffers( 1, &ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( u16 ) * indices.size(), indices.data(), GL_STATIC_DRAW );

    attribLocation = glGetAttribLocation( shader->GetID(), "v_position" );
    glEnableVertexAttribArray( attribLocation );
    glBindAttribLocation( shader->GetID(), attribLocation, "v_position" );

    glGenBuffers( 1, &nbo );
    glBindBuffer( GL_ARRAY_BUFFER, nbo );
    glBufferData(
        GL_ARRAY_BUFFER, sizeof( float ) * normalBuffer.size(), normalBuffer.data(), GL_STATIC_DRAW );

    attribLocation = glGetAttribLocation( shader->GetID(), "v_normal" );
    glEnableVertexAttribArray( attribLocation );
    glBindAttribLocation( shader->GetID(), attribLocation, "v_normal" );

    glGenBuffers( 1, &cbo );
    glBindBuffer( GL_ARRAY_BUFFER, cbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * uvBuffer.size(), uvBuffer.data(), GL_STATIC_DRAW );

    attribLocation = glGetAttribLocation( shader->GetID(), "v_uv" );
    glEnableVertexAttribArray( attribLocation );
    glBindAttribLocation( shader->GetID(), attribLocation, "v_uv" );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    mesh->VBO = vbo;
    mesh->NBO = nbo;
    mesh->IBO = ibo;
    mesh->CBO = cbo;

    return mesh;
}

// TODO: Release all mesh data from memory
bool Mesh::update( Shader* shader )
{
    std::vector<float> vertexBuffer;
    std::vector<float> normalBuffer;
    std::vector<float> uvBuffer;

    if ( Positions.size() == 0 || Normals.size() == 0 || UVMap.size() == 0 )
        return false;

    for ( u32 i = 0; i < Positions.size(); i++ )
    {
        vertexBuffer.push_back( Positions[i].x );
        vertexBuffer.push_back( Positions[i].y );
        vertexBuffer.push_back( Positions[i].z );

        normalBuffer.push_back( Normals[i].x );
        normalBuffer.push_back( Normals[i].y );
        normalBuffer.push_back( Normals[i].z );

        uvBuffer.push_back( UVMap[i].x );
        uvBuffer.push_back( UVMap[i].y );
    }

    u32 attribLocation = 0;

    u32 vbo = 0, nbo = 0, ibo = 0, cbo = 0;

    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData(
        GL_ARRAY_BUFFER, sizeof( float ) * vertexBuffer.size(), vertexBuffer.data(), GL_STATIC_DRAW );

    glGenBuffers( 1, &ibo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( u16 ) * Indices.size(), Indices.data(), GL_STATIC_DRAW );

    attribLocation = glGetAttribLocation( shader->GetID(), "v_position" );
    glEnableVertexAttribArray( attribLocation );
    glBindAttribLocation( shader->GetID(), attribLocation, "v_position" );

    glGenBuffers( 1, &nbo );
    glBindBuffer( GL_ARRAY_BUFFER, nbo );
    glBufferData(
        GL_ARRAY_BUFFER, sizeof( float ) * normalBuffer.size(), normalBuffer.data(), GL_STATIC_DRAW );

    attribLocation = glGetAttribLocation( shader->GetID(), "v_normal" );
    glEnableVertexAttribArray( attribLocation );
    glBindAttribLocation( shader->GetID(), attribLocation, "v_normal" );

    glGenBuffers( 1, &cbo );
    glBindBuffer( GL_ARRAY_BUFFER, cbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * uvBuffer.size(), uvBuffer.data(), GL_STATIC_DRAW );

    attribLocation = glGetAttribLocation( shader->GetID(), "v_uv" );
    glEnableVertexAttribArray( attribLocation );
    glBindAttribLocation( shader->GetID(), attribLocation, "v_uv" );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    this->VBO = vbo;
    this->NBO = nbo;
    this->IBO = ibo;
    this->CBO = cbo;

    return true;
}

void updateNormals( std::vector<glm::vec3>& pos, std::vector<glm::vec3>& norms, std::vector<u32> indices )
{
    if ( norms.size() > 0 )
        norms.clear();
    else
        norms.reserve( pos.size() );

    for ( size_t i = 0; i < indices.size(); i += 3 )
    {
        glm::vec3& p1 = pos[indices[i + 0]];
        glm::vec3& p2 = pos[indices[i + 1]];
        glm::vec3& p3 = pos[indices[i + 2]];

        /** REVIEW: Pseudocode
         * Begin Function CalculateSurfaceNormal (Input Triangle) Returns Vector
         *     Set Vector U to (Triangle.p2 minus Triangle.p1)
         *     Set Vector V to (Triangle.p3 minus Triangle.p1)
         *
         *     Set Normal.x to (multiply U.y by V.z) minus (multiply U.z by V.y)
         *     Set Normal.y to (multiply U.z by V.x) minus (multiply U.x by V.z)
         *     Set Normal.z to (multiply U.x by V.y) minus (multiply U.y by V.x)
         *
         *     Returning Normal
         *
         * End Function
         */
        glm::vec3 U = p2 - p1;
        glm::vec3 V = p3 - p1;

        glm::vec3 normal {
            U.y * V.z - U.z * V.y,
            U.z * V.x - U.x * V.z,
            U.x * V.y - U.y * V.x,
        };

        norms.emplace_back( U.y * V.z - U.z * V.y, U.z * V.x - U.x * V.z, U.x * V.y - U.y * V.x );
    }
}

// TODO: Compress mess to reduce polycount
// LINK: https://sites.stat.washington.edu/wxs/Siggraph-93/siggraph93.pdf
void Mesh::Optimize( f32 ratio )
{
}

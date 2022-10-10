#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <entt/entt.hpp>
#include <emscripten/bind.h>

#include <aakara/Scene.hpp>
#include <aakara/Texture.hpp>

Scene::Scene()
{
}

Ptr<Scene> Scene::LoadScene( std::string filepath )
{
    const u32 import_flags = aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_SortByPType
                             | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_OptimizeMeshes
                             | aiProcess_ValidateDataStructure;

    Assimp::Importer importer;

    const aiScene* loadedScene = importer.ReadFile( filepath, import_flags );

    Array<Ptr<Camera>> cameraList( loadedScene->mNumCameras );

    for ( size_t i = 0; i < loadedScene->mNumCameras; i++ )
    {
        // SECTION
        // REVIEW: This should load all needed information for camera
        // TODO: Copy over the aiMatrix4x4::Decompose(...) function to see how 4x4 matrix is transformed back
        // SECTION
        aiCamera* loadedCam = loadedScene->mCameras[i];

        Ptr<Camera> camera = std::make_shared<Camera>();
        camera->FOV        = loadedCam->mHorizontalFOV;
        camera->ZNear      = loadedCam->mClipPlaneNear;
        camera->ZFar       = loadedCam->mClipPlaneFar;
        // camera->Position   = glm::normalize(
        //       glm::vec3( loadedCam->mPosition.x, loadedCam->mPosition.y, loadedCam->mPosition.z ) );

        // camera->Up = glm::normalize( glm::vec3( loadedCam->mUp.x, loadedCam->mUp.y, loadedCam->mUp.z ) );
        // camera->Forward
        //     = glm::normalize( glm::vec3( loadedCam->mLookAt.x, loadedCam->mLookAt.y, loadedCam->mLookAt.z )
        //     );

        cameraList[i] = camera;
    }

    Array<Ptr<Texture>> textureList;

    /* ------------------------------ Load textures ----------------------------- */
    for ( size_t i = 0; i < loadedScene->mNumMaterials; i++ )
    {
        aiMaterial* material = loadedScene->mMaterials[i];
        if ( material->GetTextureCount( aiTextureType_DIFFUSE ) == 1 )
        {
            aiString file_path;
            material->GetTexture( aiTextureType_DIFFUSE, 0, &file_path );

            const aiTexture* loadedTexture = loadedScene->GetEmbeddedTexture( file_path.C_Str() );

            size_t    pixelCount = loadedTexture->mWidth * loadedTexture->mHeight;
            Array<u8> pixels( pixelCount * 4 );

            // REVIEW: Could this be parallalized?
            for ( size_t i = 0; i < pixelCount; i++ )
            {
                aiTexel* loadedPixel = loadedTexture->pcData;
                size_t   pixl_idx    = i * 4;

                pixels[pixl_idx + 0] = reinterpret_cast<u8>( loadedPixel->r );
                pixels[pixl_idx + 1] = reinterpret_cast<u8>( loadedPixel->g );
                pixels[pixl_idx + 2] = reinterpret_cast<u8>( loadedPixel->b );
                pixels[pixl_idx + 3] = reinterpret_cast<u8>( loadedPixel->a );
            }

            Ptr<Texture> texture = std::make_shared<Texture>(
                pixels, loadedTexture->mWidth, loadedTexture->mHeight, Texture::PixelType::RGBA );

            // load texture to OpenGL
            texture->update();

            textureList.push_back( texture );
        }
    }

    /* ------------------------------- Load meshes ------------------------------ */
    if ( !loadedScene->HasMeshes() )
        throw std::runtime_error( importer.GetErrorString() );

    Array<Ptr<Mesh>> meshList( loadedScene->mNumMeshes );
    for ( size_t i = 0; i < loadedScene->mNumMeshes; i++ )
    {
        aiMesh* loadedMesh = loadedScene->mMeshes[i];

        u32 vertexCount = loadedMesh->mNumVertices;
        u32 faceCount   = loadedMesh->mNumFaces;

        std::vector<glm::vec3> positions( vertexCount );
        std::vector<glm::vec3> normals( vertexCount );
        std::vector<glm::vec2> uvmap( vertexCount );
        std::vector<u16>       indices;

        for ( u32 j = 0; j < vertexCount; j++ )
        {
            positions[j]
                = { loadedMesh->mVertices[j].x, loadedMesh->mVertices[j].y, loadedMesh->mVertices[j].z };
            normals[j] = { loadedMesh->mNormals[j].x, loadedMesh->mNormals[j].y, loadedMesh->mNormals[j].z };

            uvmap[j] = { loadedMesh->mTextureCoords[0][j].x, loadedMesh->mTextureCoords[0][j].y };
        }

        for ( u32 j = 0; j < loadedMesh->mNumFaces; j++ )
        {
            aiFace& face = loadedMesh->mFaces[j];
            for ( u32 k = 0; k < face.mNumIndices; k++ )
                indices.push_back( (u16)face.mIndices[k] );
        }

        meshList[i] = std::make_shared<Mesh>( positions, normals, indices, uvmap );
    }

    entt::registry registry;

    /* ---------------------------- Create scene tree --------------------------- */
    std::function<Ptr<SceneObject>( aiNode* )> traverseTree = [&traverseTree, &meshList]( aiNode* node )
    {
        std::string name = node->mParent ? "ROOT" : std::string( node->mName.C_Str() );

        Ptr<SceneObject> sceneNode = std::make_shared<SceneObject>( name );

        aiVector3D pos, rot, scale;
        node->mTransformation.Decompose( scale, rot, pos );

        sceneNode->transform->position = glm::vec3( pos.x, pos.y, pos.z );
        sceneNode->transform->rotation = glm::vec3( rot.x, rot.y, rot.z );
        sceneNode->transform->scale    = glm::vec3( scale.x, scale.y, scale.z );

        if ( node->mNumMeshes > 0 )
            Ptr<Mesh> mesh = meshList[*node->mMeshes];

        for ( size_t i = 0; i < node->mNumChildren; i++ )
            sceneNode->children.push_back( traverseTree( node->mChildren[i] ) );

        return sceneNode;
    };

    std::function<void( aiNode*, Node* )> traverse
        = [&meshList, &traverse, &registry]( aiNode* sceneNode, Node* parent )
    {
        entt::entity entity = registry.create();
        Node&        node   = registry.emplace<Node>( entity, "", parent );

        for ( size_t i = 0; i < sceneNode->mNumChildren; i++ )
            traverse( sceneNode->mChildren[i], &node );
    };

    traverse( loadedScene->mRootNode, nullptr );

    Ptr<SceneObject> rootNode = traverseTree( loadedScene->mRootNode );

    /* -------------------- Create scene from all loaded data ------------------- */
    Ptr<Scene> scene = std::make_shared<Scene>();

    scene->m_rootNode = rootNode;
    scene->m_meshes   = meshList;
    scene->m_cameras  = cameraList;
    scene->m_textures = textureList;

    return scene;
}

EMSCRIPTEN_BINDINGS( SCENE_HPP )
{
    emscripten::class_<Scene>( "Scene" ) //
        .smart_ptr_constructor( "Scene", &Scene::LoadScene );
}

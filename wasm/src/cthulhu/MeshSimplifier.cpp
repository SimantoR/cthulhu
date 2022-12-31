/**
 * @file MeshSimplifier.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-10-30
 *
 * @copyright MIT license
 * @link https://github.com/sp4cerat/Fast-Quadric-Mesh-Simplification/blob/master/src.gl/Simplify.h @endlink
 */

#include <cthulhu/MeshSimplifier.hpp>
#include <algorithm>
#include <aakara/Mesh.hpp>

#define MAX_ITERATION 100
#define AGRESSIVENESS 7
#define TRIANGLE_EDGE_COUNT 3
#define DOUBLE_EPSILON 1.0E-3
#define DENOM_EPSILON 0.00000001
#define UVCHANNEL_COUNT 3

#pragma region Private Classes

class SymetricMatrix
{

public:
    // Constructor

    SymetricMatrix( double c = 0 )
    {
        for ( int i = 0; i < 10; i++ )
            m[i] = c;
    }

    SymetricMatrix( double m11, double m12, double m13, double m14, double m22, double m23, double m24,
        double m33, double m34, double m44 )
    {
        m[0] = m11;
        m[1] = m12;
        m[2] = m13;
        m[3] = m14;
        m[4] = m22;
        m[5] = m23;
        m[6] = m24;
        m[7] = m33;
        m[8] = m34;
        m[9] = m44;
    }

    // Make plane

    SymetricMatrix( double a, double b, double c, double d )
    {
        m[0] = a * a;
        m[1] = a * b;
        m[2] = a * c;
        m[3] = a * d;
        m[4] = b * b;
        m[5] = b * c;
        m[6] = b * d;
        m[7] = c * c;
        m[8] = c * d;
        m[9] = d * d;
    }

    double operator[]( int c ) const
    {
        return m[c];
    }

    // Determinant

    double det( int a11, int a12, int a13, int a21, int a22, int a23, int a31, int a32, int a33 )
    {
        double det = m[a11] * m[a22] * m[a33] + m[a13] * m[a21] * m[a32] + m[a12] * m[a23] * m[a31]
                     - m[a13] * m[a22] * m[a31] - m[a11] * m[a23] * m[a32] - m[a12] * m[a21] * m[a33];
        return det;
    }

    const SymetricMatrix operator+( const SymetricMatrix& n ) const
    {
        return SymetricMatrix( m[0] + n[0], m[1] + n[1], m[2] + n[2], m[3] + n[3], m[4] + n[4], m[5] + n[5],
            m[6] + n[6], m[7] + n[7], m[8] + n[8], m[9] + n[9] );
    }

    SymetricMatrix& operator+=( const SymetricMatrix& n )
    {
        m[0] += n[0];
        m[1] += n[1];
        m[2] += n[2];
        m[3] += n[3];
        m[4] += n[4];
        m[5] += n[5];
        m[6] += n[6];
        m[7] += n[7];
        m[8] += n[8];
        m[9] += n[9];
        return *this;
    }

    double m[10];
};

class Triangle
{
public:
    u32                index;
    std::array<u32, 3> vertices;
    u32                subMeshIndex;
    u32                va0, va1, va2;

    double err0 = 0;
    double err1 = 0;
    double err2 = 0;
    double err3 = 0;

    bool      deleted = false;
    bool      dirty   = false;
    glm::vec3 n;

    Triangle()
    {
    }

    Triangle( u32 index, u32 v0, u32 v1, u32 v2, u32 subMeshIndex )
        : index( index )
        , vertices( { v0, v1, v2 } )
        , va0( v0 )
        , va1( v1 )
        , va2( v2 )
        , subMeshIndex( subMeshIndex )
        , n( 0.0f )
    {
    }

    bool operator==( const Triangle& t )
    {
        return this->index == t.index;
    }

    void GetAttributeIndices( std::array<u32, 3>& attributeIndices )
    {
        attributeIndices[0] = va0;
        attributeIndices[1] = va1;
        attributeIndices[2] = va2;
    }

    void GetErrors( std::array<double, 3>& err )
    {
        err[0] = err0;
        err[1] = err1;
        err[2] = err2;
    }
};

class Vertex
{
public:
    u32            index  = 0;
    glm::vec3      p      = glm::vec3( 0.0f );
    u32            tstart = 0;
    u32            tcount = 0;
    SymetricMatrix q;
    bool           borderEdge     = true;
    bool           uvSeamEdge     = false;
    bool           uvFoldoverEdge = false;

    Vertex()
    {
    }

    Vertex( u32 index, glm::vec3 point )
        : index( index )
        , p( point )
    {
    }

    bool operator==( const Vertex& v )
    {
        return this->index == v.index;
    }
};

struct Ref
{
    int tid, tvertex;
};

#pragma endregion

#pragma region Private Functions

double                                       calculate_error( Vertex& v1, Vertex& v2, glm::vec3& p_result );
double                                       vertex_error( SymetricMatrix q, double x, double y, double z );
std::tuple<Vector<Triangle>, Vector<Vertex>> transform_mesh( const Mesh* mesh );

#pragma endregion

Ptr<Mesh> MeshSimplifier::Simplify( const Mesh& mesh, f32 quality )
{
    quality = std::clamp( quality, 0.000001f, 1.0f );

    // Transform mesh information to internal data structures
    transform_mesh( &mesh );

    u32 triangle_count        = mesh.Indices.size() / 3;
    u32 target_triangle_count = std::roundf( triangle_count * quality );

    std::vector<Triangle> triangles( triangle_count );
    std::vector<Vertex>   vertices( mesh.Indices.size() );

    // Set all triangles to specify not deleted
    std::for_each( triangles.begin(), triangles.end(), []( Triangle& t ) { t.deleted = false; } );

    u32              deleted_triangles = 0;
    std::vector<u32> deleted0, deleted1;

    for ( u32 iteration = 0; iteration < MAX_ITERATION; iteration++ )
    {
        if ( triangle_count - deleted_triangles <= target_triangle_count )
            break;

        std::for_each( triangles.begin(), triangles.end(), []( Triangle& t ) { t.dirty = false; } );

        // All triangles with edges below the threshold will be removed
        //
        // The following numbers works well for most models.
        // If it does not, try to adjust the 3 parameters.
        //
        double threshold = 0.000000001 * std::pow( double( iteration + 3 ), AGRESSIVENESS );

        for ( u32 i = 0; i < triangles.size(); i++ )
        {
            Triangle& t = triangles[i];
            if ( t.err3 > threshold )
                continue;
            if ( t.deleted )
                continue;
            if ( t.dirty )
                continue;

            double errs[3] = { t.err0, t.err1, t.err2 };

            for ( u32 j = 0; j < TRIANGLE_EDGE_COUNT; j++ )
            {
                if ( errs[j] > threshold )
                    continue;

                u32     i0 = t.vertices[j];
                Vertex& v0 = vertices[i0];

                u32     i1 = t.vertices[( j + 1 ) % 3];
                Vertex& v1 = vertices[i1];

                // Border check
                if ( v0.borderEdge != v1.borderEdge )
                    continue;

                glm::vec3 p;
                calculate_error( v0, v1, p );

                deleted0.resize( v0.tcount ); // normals temporarily
                deleted1.resize( v1.tcount ); // normals temporarily

                // don't remove if flipped
                if ( flipped( p, i0, i1, v0, v1, deleted0 ) )
                    continue;
                if ( flipped( p, i1, i0, v1, v0, deleted1 ) )
                    continue;

                v0.p       = p;
                v0.q       = v1.q + v0.q;
                u32 tstart = m_refs.size();

                update_triangles( i0, v0, deleted0, deleted_triangles );
                update_triangles( i0, v1, deleted1, deleted_triangles );

                u32 tcount = m_refs.size() - tstart;

                if ( tcount <= v0.tcount )
                {
                    // save ram
                    if ( tcount )
                        std::memcpy( &m_refs[v0.tstart], &m_refs[tstart], tcount * sizeof( Ref ) );
                }
                else
                {
                    // append
                    v0.tstart = tstart;
                }

                v0.tcount = tcount;
                break;
            }
            // done?
            if ( triangle_count - deleted_triangles <= target_triangle_count )
                break;
        }
    }

    compact_mesh();
    return generate_mesh();
}

// Check if a triangle flips when this edge is removed
bool MeshSimplifier::flipped( glm::vec3 p, int i0, int i1, Vertex& v0, Vertex& v1, std::vector<u32>& deleted )
{
    int bordercount = 0;

    for ( int k = 0; k < v0.tcount; k++ )
    {
        Triangle& t = m_triangles[m_refs[v0.tstart + k].tid];
        if ( t.deleted )
            continue;

        int s   = m_refs[v0.tstart + k].tvertex;
        u32 id1 = t.vertices[( s + 1 ) % 3];
        u32 id2 = t.vertices[( s + 2 ) % 3];

        if ( id1 == i1 || id2 == i1 ) // delete ?
        {
            bordercount++;
            deleted[k] = 1;
            continue;
        }
        glm::vec3 d1 = glm::normalize( m_vertices[id1].p - p );
        glm::vec3 d2 = glm::normalize( m_vertices[id2].p - p );
        if ( fabs( glm::dot( d1, d2 ) ) > 0.999f )
            return true;
        glm::vec3 n = glm::normalize( glm::cross( d1, d2 ) );
        deleted[k]  = 0;
        if ( glm::dot( n, t.n ) < 0.2 )
            return true;
    }
    return false;
}

// Compact mesh before exiting
void MeshSimplifier::compact_mesh()
{
    int dst = 0;

    // loopi( 0, vertices.size() )
    for ( int i = 0; i < m_vertices.size(); i++ )
        m_vertices[i].tcount = 0;

    // loopi( 0, triangles.size() )
    for ( int i = 0; i < m_triangles.size(); i++ )
    {
        if ( !m_triangles[i].deleted )
        {
            Triangle& t        = m_triangles[i];
            m_triangles[dst++] = t;

            // loopj( 0, 3 ) vertices[t.v[j]].tcount = 1;
            for ( int j = 0; j < TRIANGLE_EDGE_COUNT; j++ )
                m_vertices[t.vertices[j]].tcount = 1;
        }
    }
    m_triangles.resize( dst );
    dst = 0;
    // loopi( 0, vertices.size() )
    for ( int i = 0; i < m_vertices.size(); i++ )
    {
        if ( m_vertices[i].tcount )
        {
            m_vertices[i].tstart = dst;
            m_vertices[dst].p    = m_vertices[i].p;
            dst++;
        }
    }

    // loopi( 0, triangles.size() )
    for ( int i = 0; i < m_triangles.size(); i++ )
    {
        Triangle& t = m_triangles[i];
        // loopj( 0, 3 ) t.v[j] = vertices[t.v[j]].tstart;

        for ( int j = 0; j < TRIANGLE_EDGE_COUNT; j++ )
            t.vertices[j] = m_vertices[t.vertices[j]].tstart;
    }
    m_vertices.resize( dst );
}

// Update triangle connections and edge error after a edge is collapsed
void MeshSimplifier::update_triangles( u32 i0, Vertex& v, std::vector<u32>& deleted, u32& deleted_triangles )
{
    glm::vec3 p;
    // loopk( 0, v.tcount )
    for ( int k = 0; k < v.tcount; k++ )
    {
        Ref&      r = m_refs[v.tstart + k];
        Triangle& t = m_triangles[r.tid];
        if ( t.deleted )
            continue;
        if ( deleted[k] )
        {
            t.deleted = 1;
            deleted_triangles++;
            continue;
        }
        t.vertices[r.tvertex] = i0;
        t.dirty               = 1;
        t.err0                = calculate_error( m_vertices[t.vertices[0]], m_vertices[t.vertices[1]], p );
        t.err1                = calculate_error( m_vertices[t.vertices[1]], m_vertices[t.vertices[2]], p );
        t.err2                = calculate_error( m_vertices[t.vertices[2]], m_vertices[t.vertices[0]], p );
        t.err3                = std::min( t.err0, std::min( t.err1, t.err2 ) );
        m_refs.push_back( r );
    }
}

Ptr<Mesh> MeshSimplifier::generate_mesh()
{
    Vector<glm::vec3> vertices( m_vertices.size() );
    Vector<glm::vec3> normals( m_triangles.size() );
    Vector<u16>       indices( std::round( m_triangles.size() * 3 ) );
    Vector<glm::vec2> uvmap;

    // Copy over vertex positions
    for ( u32 i = 0; i < m_vertices.size(); i++ )
        vertices[i] = m_vertices[i].p;

    // Copy over indices and calculate normal per triangle
    for ( u32 i = 0; i < m_triangles.size(); i++ )
    {
        Triangle& t            = m_triangles[i];
        indices[( i * 3 ) + 0] = static_cast<u16>( t.vertices[0] );
        indices[( i * 3 ) + 1] = static_cast<u16>( t.vertices[1] );
        indices[( i * 3 ) + 2] = static_cast<u16>( t.vertices[2] );

        glm::vec3& n = normals[i];

        glm::vec3 u = m_vertices[t.vertices[1]].p - m_vertices[t.vertices[0]].p;
        glm::vec3 v = m_vertices[t.vertices[2]].p - m_vertices[t.vertices[1]].p;

        n.x = ( u.y * v.z ) - ( u.z * v.y );
        n.y = ( u.z * v.x ) - ( u.x * v.z );
        n.z = ( u.x * v.y ) - ( u.y * v.x );
    }

    return std::make_shared<Mesh>( vertices, normals, indices, uvmap );
}

void MeshSimplifier::transform_mesh( const Mesh* mesh )
{
    m_triangles.resize( mesh->Indices.size() / 3 );
    m_vertices.resize( mesh->Vertices.size() );

    for ( u32 i = 0; i < m_vertices.size(); i++ )
    {
        Vertex& v = m_vertices[i];
        v.p       = mesh->Vertices[i];
    }

    for ( u32 i = 0; i < mesh->Indices.size() / 3; i++ )
    {
        Triangle& t   = m_triangles[i];
        t.vertices[0] = mesh->Indices[i + 0];
        t.vertices[1] = mesh->Indices[i + 1];
        t.vertices[2] = mesh->Indices[i + 2];
    }
}

// Error between vertex and Quadric
double vertex_error( SymetricMatrix q, double x, double y, double z )
{
    return q[0] * x * x + 2 * q[1] * x * y + 2 * q[2] * x * z + 2 * q[3] * x + q[4] * y * y + 2 * q[5] * y * z
           + 2 * q[6] * y + q[7] * z * z + 2 * q[8] * z + q[9];
}

// Error for one edge
double calculate_error( Vertex& v1, Vertex& v2, glm::vec3& p_result )
{
    SymetricMatrix q      = v1.q + v2.q;
    bool           border = v1.borderEdge && v2.borderEdge;
    double         error  = 0;
    double         det    = q.det( 0, 1, 2, 1, 4, 5, 2, 5, 7 );

    if ( det != 0 && !border )
    {
        // q_delta is invertible
        p_result.x = -1 / det * ( q.det( 1, 2, 3, 4, 5, 6, 5, 7, 8 ) ); // vx = A41/det(q_delta)
        p_result.y = 1 / det * ( q.det( 0, 2, 3, 1, 5, 6, 2, 7, 8 ) );  // vy = A42/det(q_delta)
        p_result.z = -1 / det * ( q.det( 0, 1, 3, 1, 4, 6, 2, 5, 8 ) ); // vz = A43/det(q_delta)
        error      = vertex_error( q, p_result.x, p_result.y, p_result.z );
    }
    else
    {
        // det = 0 -> try to find best result
        glm::vec3 p1     = v1.p;
        glm::vec3 p2     = v2.p;
        glm::vec3 p3     = ( p1 + p2 ) / 2.0f;
        double    error1 = vertex_error( q, p1.x, p1.y, p1.z );
        double    error2 = vertex_error( q, p2.x, p2.y, p2.z );
        double    error3 = vertex_error( q, p3.x, p3.y, p3.z );
        error            = std::min( error1, std::min( error2, error3 ) );
        if ( error1 == error )
            p_result = p1;
        if ( error2 == error )
            p_result = p2;
        if ( error3 == error )
            p_result = p3;
    }

    return error;
}

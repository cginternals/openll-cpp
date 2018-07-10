
#pragma once


#include <memory>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <openll/openll_api.h>


namespace globjects
{
    class Texture;
    class Buffer;
    class VertexArray;
}


namespace openll
{


/**
*  @brief
*    Vertex array that describes each glyph to be rendered on the screen
*/
class OPENLL_API GlyphVertexCloud
{
public:
    /**
    *  @brief
    *    Data for a single vertex (glyph) on the screen
    */
    struct Vertex
    {
        glm::vec3 origin;    ///< Position of the glyph in normalized device coordinates
        glm::vec3 vtan;      ///< Tangent vector
        glm::vec3 vbitan;    ///< Bitangent vector
        glm::vec4 uvRect;    ///< Source image rect of the glyph in the glyph texter (uv-coordinates)
        glm::vec4 textColor; ///< Text of the glyph
    };


public:
    /**
    *  @brief
    *    Constructor
    *
    *  @remarks
    *    This allocates OpenGL objects, so an OpenGL context must be current when creating this object.
    */
    GlyphVertexCloud();

    /**
    *  @brief
    *    Move constructor
    */
    GlyphVertexCloud(GlyphVertexCloud && other);

    /**
    *  @brief
    *    Destructor
    *
    *  @remarks
    *    This allocates OpenGL objects, so an OpenGL context must be current when deleting this object.
    */
    virtual ~GlyphVertexCloud();

    // Forbid copying
    GlyphVertexCloud & operator=(const GlyphVertexCloud &) = delete;

    // Allow moving
    GlyphVertexCloud & operator=(GlyphVertexCloud && other);

    /**
    *  @brief
    *    Get vertices (in CPU memory)
    *
    *  @return
    *    Vertex list
    */
    const std::vector<Vertex> & vertices() const;

    /**
    *  @brief
    *    Get vertices (in CPU memory)
    *
    *  @return
    *    Vertex list
    */
    std::vector<Vertex> & vertices();

    /**
    *  @brief
    *    Get vertex array
    *
    *  @return
    *    Vertex array
    */
    const globjects::VertexArray * vao() const;

    /**
    *  @brief
    *    Get vertex array
    *
    *  @return
    *    Vertex array
    */
    globjects::VertexArray * vao();

    /**
    *  @brief
    *    Get glyph texture for which the text has been layouted
    *
    *  @return
    *    Glyph texture
    */
    const globjects::Texture * texture() const;

    /**
    *  @brief
    *    Set glyph texture for which the text has been layouted
    *
    *  @param[in] texture
    *    Glyph texture
    */
    void setTexture(globjects::Texture * texture);

    /**
    *  @brief
    *    Update VAO
    *
    *    Uploads the contents of the vertex list (see vao())
    *    onto the VAO on the GPU.
    *
    *  @param[in] texture
    *    Glyph texture
    */
    void update();

    /**
    *  @brief
    *    Update VAO
    *
    *    Uploads the contents of the vertex list (see vao())
    *    onto the VAO on the GPU.
    *
    *  @param[in] vertices
    *    Vertex list
    */
    void update(const std::vector<Vertex> & vertices);

    /**
    *  @brief
    *    Draw glyph vertex array
    *
    *    This function draws the vertex array by means of OpenGL geometry.
    *    It only emits the geometry, but it doesn't setup the rendering
    *    process for text rendering, such as binding the glyph texture
    *    or shader programs. Thus, the rendering has to be setup before
    *    calling this function (see GlyphRenderer).
    */
    void draw() const;


protected:
    std::vector<Vertex>                       m_vertices; ///< Vertex list (CPU memory)
    std::unique_ptr<globjects::Buffer>        m_buffer;   ///< Vertex buffer (GPU memory)
    std::unique_ptr<globjects::VertexArray>   m_vao;      ///< Vertex array object
    globjects::Texture                      * m_texture;  ///< Glyph texture
};


} // namespace openll

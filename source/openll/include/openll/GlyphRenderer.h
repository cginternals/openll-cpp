
#pragma once


#include <memory>

#include <glm/fwd.hpp>

#include <openll/openll_api.h>


namespace globjects
{
    class AbstractStringSource;
    class Program;
}


namespace openll
{


class GlyphVertexCloud;


/**
*  @brief
*    Executes the OpenGL code to render layouted text to the screen
*/
class OPENLL_API GlyphRenderer
{
public:
    /**
    *  @brief
    *    Get standard vertex shader for text rendering
    *
    *  @return
    *    Vertex shader source
    */
    static std::unique_ptr<globjects::AbstractStringSource> vertexShaderSource();

    /**
    *  @brief
    *    Get standard geometry shader for text rendering
    *
    *  @return
    *    Vertex shader source
    */
    static std::unique_ptr<globjects::AbstractStringSource> geometryShaderSource();

    /**
    *  @brief
    *    Get standard geometry shader for text rendering
    *
    *  @return
    *    Vertex shader source
    */
    static std::unique_ptr<globjects::AbstractStringSource> fragmentShaderSource();


public:
    /**
    *  @brief
    *    Constructor
    *
    *  @param[in] program
    *    Shader program used for rendering
    *
    *  @remarks
    *    This initializes OpenGL objects, so an OpenGL context must be current when creating this object.
    */
    explicit GlyphRenderer(globjects::Program * program);

    /**
    *  @brief
    *    Destructor
    */
    virtual ~GlyphRenderer();

    // Forbid copying
    GlyphRenderer & operator=(const GlyphRenderer &) = delete;

    /**
    *  @brief
    *    Get shader program
    *
    *  @return
    *    Shader program used for rendering
    */
    const globjects::Program * program() const;

    /**
    *  @brief
    *    Get shader program
    *
    *  @return
    *    Shader program used for rendering
    */
    globjects::Program * program();

    /**
    *  @brief
    *    Render text to screen in 2D space
    *
    *  @param[in] vertexCloud
    *    Glyph vertex array
    */
    void render(const GlyphVertexCloud & vertexCloud) const;

    /**
    *  @brief
    *    Render text to screen in 3D world space
    *
    *  @param[in] vertexCloud
    *    Glyph vertex array
    *  @param[in] viewProjectionMatrix
    *    View-projection matrix of the current camera
    */
    void renderInWorld(const GlyphVertexCloud & vertexCloud, const glm::mat4 & viewProjectionMatrix) const;


protected:
    globjects::Program * m_program; ///< Program used for rendering
};


} // namespace openll


#include <openll/GlyphRenderer.h>

#include <glm/mat4x4.hpp>

#include <globjects/base/File.h>
#include <globjects/Shader.h>
#include <globjects/Program.h>
#include <globjects/Texture.h>

#include <openll/openll.h>
#include <openll/GlyphVertexCloud.h>


namespace openll
{


std::unique_ptr<globjects::AbstractStringSource> GlyphRenderer::vertexShaderSource()
{
    return globjects::Shader::sourceFromFile(openll::dataPath() + "/openll/shaders/glyph.vert");
}

std::unique_ptr<globjects::AbstractStringSource> GlyphRenderer::geometryShaderSource()
{
    return globjects::Shader::sourceFromFile(openll::dataPath() + "/openll/shaders/glyph.geom");
}

std::unique_ptr<globjects::AbstractStringSource> GlyphRenderer::fragmentShaderSource()
{
    return globjects::Shader::sourceFromFile(openll::dataPath() + "/openll/shaders/glyph.frag");
}

GlyphRenderer::GlyphRenderer(globjects::Program * program)
: m_program(program)
{
    // Initialize uniform values
    m_program->setUniform<gl::GLint>("glyphs", 0);
    m_program->setUniform<glm::mat4>("viewProjectionMatrix", glm::mat4());
}

GlyphRenderer::~GlyphRenderer()
{
}

const globjects::Program * GlyphRenderer::program() const
{
    return m_program;
}

globjects::Program * GlyphRenderer::program()
{
    return m_program;
}

void GlyphRenderer::render(const GlyphVertexCloud & vertexCloud) const
{
    // Abort if vertex array is empty
    if (vertexCloud.vertices().empty()) {
        return;
    }

    // Update uniform values
    m_program->setUniform("viewProjectionMatrix", glm::mat4());

    // Bind shader program and texture
    m_program->use();
    vertexCloud.texture()->bindActive(0);

    // Draw vertex array
    vertexCloud.draw();

    // Release shader program and texture
    vertexCloud.texture()->unbindActive(0);
    m_program->release();
}

void GlyphRenderer::renderInWorld(const GlyphVertexCloud & vertexCloud, const glm::mat4 & viewProjectionMatrix) const
{
    // Abort if vertex array is empty
    if (vertexCloud.vertices().empty())
    {
        return;
    }

    // Update uniform values
    m_program->setUniform("viewProjectionMatrix", viewProjectionMatrix);

    // Bind shader program and texture
    m_program->use();
    vertexCloud.texture()->bindActive(0);

    // Draw vertex array
    vertexCloud.draw();

    // Release shader program and texture
    vertexCloud.texture()->unbindActive(0);
    m_program->release();
}


} // namespace openll

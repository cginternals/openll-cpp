
#include <openll/GlyphRenderer.h>

#include <glm/mat4x4.hpp>

#include <glbinding/gl/enum.h>

#include <globjects/base/File.h>
#include <globjects/Shader.h>
#include <globjects/Program.h>
#include <globjects/Texture.h>

#include <openll/openll.h>
#include <openll/GlyphVertexCloud.h>


namespace openll
{


GlyphRenderer::GlyphRenderer(globjects::Program * program)
: m_program(program)
{
    m_program->setUniform<gl::GLint>("glyphs", 0);
    m_program->setUniform<glm::mat4>("viewProjectionMatrix", glm::mat4());
}

GlyphRenderer::~GlyphRenderer()
{
}

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

void GlyphRenderer::render(const GlyphVertexCloud & vertexCloud) const
{
    if (vertexCloud.vertices().empty())
    {
        return;
    }

    m_program->setUniform("viewProjectionMatrix", glm::mat4());

    m_program->use();

    vertexCloud.texture()->bindActive(0);
    vertexCloud.draw();
    vertexCloud.texture()->unbindActive(0);

    m_program->release();
}

void GlyphRenderer::renderInWorld(const GlyphVertexCloud & vertexCloud, const glm::mat4 & viewProjectionMatrix) const
{
    if (vertexCloud.vertices().empty())
    {
        return;
    }

    m_program->setUniform("viewProjectionMatrix", viewProjectionMatrix);

    m_program->use();

    vertexCloud.texture()->bindActive(0);
    vertexCloud.draw();
    vertexCloud.texture()->unbindActive(0);

    m_program->release();
}

globjects::Program * GlyphRenderer::program()
{
    return m_program;
}

const globjects::Program * GlyphRenderer::program() const
{
    return m_program;
}


} // namespace openll

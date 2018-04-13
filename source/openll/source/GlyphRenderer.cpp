
#include <openll/GlyphRenderer.h>

#include <glm/mat4x4.hpp>

#include <glbinding/gl/gl.h>

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

GlyphRenderer::GlyphRenderer()
{
    // Get shader sources
    m_vertexShaderSource   = vertexShaderSource();
    m_geometryShaderSource = geometryShaderSource();
    m_fragmentShaderSource = fragmentShaderSource();

    // Create shader programs
    m_vertexShader   = std::unique_ptr<globjects::Shader>(new globjects::Shader(gl::GL_VERTEX_SHADER,   m_vertexShaderSource.get()));
    m_geometryShader = std::unique_ptr<globjects::Shader>(new globjects::Shader(gl::GL_GEOMETRY_SHADER, m_geometryShaderSource.get()));
    m_fragmentShader = std::unique_ptr<globjects::Shader>(new globjects::Shader(gl::GL_FRAGMENT_SHADER, m_fragmentShaderSource.get()));

    // Create program
    m_program = std::unique_ptr<globjects::Program>(new globjects::Program);
    m_program->attach(m_vertexShader.get());
    m_program->attach(m_geometryShader.get());
    m_program->attach(m_fragmentShader.get());

    // Initialize uniform values
    m_program->setUniform<gl::GLint>("glyphs", 0);
    m_program->setUniform<glm::mat4>("viewProjectionMatrix", glm::mat4());
}

GlyphRenderer::~GlyphRenderer()
{
}

const globjects::Program * GlyphRenderer::program() const
{
    return m_program.get();
}

globjects::Program * GlyphRenderer::program()
{
    return m_program.get();
}

void GlyphRenderer::setProgram(std::unique_ptr<globjects::Program> && program)
{
    m_program = std::move(program);
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

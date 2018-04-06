
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>

#include <glm/glm.hpp>

#include <GLFW/glfw3.h>

#include <cppassist/memory/make_unique.h>
#include <cppassist/string/conversion.h>

#include <glbinding/gl/gl.h>
#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>

#include <globjects/globjects.h>
#include <globjects/base/File.h>
#include <globjects/logging.h>
#include <globjects/Texture.h>

#include <openll/openll.h>
#include <openll/Alignment.h>
#include <openll/LineAnchor.h>
#include <openll/FontFace.h>
#include <openll/FontLoader.h>
#include <openll/GlyphSequence.h>
#include <openll/GlyphVertexCloud.h>
#include <openll/GlyphRenderer.h>
#include <openll/Typesetter.h>


using namespace gl;
using namespace openll;


namespace
{
    auto g_viewport = glm::uvec2{};

    const auto lorem =
    R"(Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.)";

    // Text demo
    std::string g_fontFilename(openll::dataPath() + "/openll/fonts/opensansr36.fnt");
    std::string g_string(lorem);
    uint32_t    g_numChars(0);
    float       g_fontSize(1.0f);
    glm::vec2   g_origin(0.0f, 0.5f);
    glm::vec4   g_margins(0.0f, 0.0f, 0.0f, 0.0f);
    float       g_pixelPerInch(72.0f);
    bool        g_wordWrap(true);
    float       g_lineWidth(400.0f);
    Alignment   g_alignment(Alignment::Centered);
    LineAnchor  g_lineAnchor(LineAnchor::Baseline);
    bool        g_optimized(true);

    FontFace  * g_fontFace;
    std::vector<GlyphSequence> g_sequences;
    GlyphVertexCloud * g_vertexCloud;

    std::unique_ptr<globjects::AbstractStringSource> g_vSource;
    std::unique_ptr<globjects::AbstractStringSource> g_gSource;
    std::unique_ptr<globjects::AbstractStringSource> g_fSource;

    std::unique_ptr<globjects::Shader> g_vertexShader;
    std::unique_ptr<globjects::Shader> g_geometryShader;
    std::unique_ptr<globjects::Shader> g_fragmentShader;

    std::unique_ptr<globjects::Program> g_program;

    std::unique_ptr<GlyphRenderer> g_renderer;
}

void loadFont(const std::string & filename)
{
    FontLoader fontLoader;
    g_fontFace = fontLoader.load(filename);
}

void createSequence()
{
    const auto scaledFontSize = g_fontSize * 16.0f;
    const auto scaledLineWidth = g_lineWidth * 160.0f;

    g_sequences.resize(1);

    g_sequences.front().setString(cppassist::string::encode(g_string, cppassist::Encoding::UTF8));
    g_sequences.front().setWordWrap(g_wordWrap);
    g_sequences.front().setLineWidth(scaledLineWidth, scaledFontSize, *g_fontFace);
    g_sequences.front().setAlignment(g_alignment);
    g_sequences.front().setLineAnchor(g_lineAnchor);

    g_sequences.front().setTransform(g_origin, scaledFontSize, *g_fontFace, g_viewport, g_pixelPerInch, g_margins);
}

void prepare()
{
    // get total number of glyphs
    auto numGlyphs = std::size_t{0};
    for (const auto & sequence : g_sequences)
    {
        numGlyphs += sequence.size(*g_fontFace);
    }

    g_vertexCloud = new GlyphVertexCloud;

    // prepare vertex cloud storage
    g_vertexCloud->vertices().clear();
    g_vertexCloud->vertices().resize(numGlyphs);

    // typeset and transform all sequences
    assert(g_fontFace != nullptr);

    auto vertexItr = g_vertexCloud->vertices().begin();
    for (const auto & sequence : g_sequences)
    {
        auto extent = Typesetter::typeset(sequence, *g_fontFace, vertexItr);
        vertexItr += sequence.size(*g_fontFace);
    }

    if (g_optimized)
    {
        g_vertexCloud->optimize(g_sequences, *g_fontFace); // optimize and update drawable
    }
    else
    {
        g_vertexCloud->update(); // update drawable
    }

    g_vertexCloud->setTexture(g_fontFace->glyphTexture());
}

void prepareRendering()
{
    g_vSource = GlyphRenderer::vertexShaderSource();
    g_gSource = GlyphRenderer::geometryShaderSource();
    g_fSource = GlyphRenderer::fragmentShaderSource();

    g_vertexShader   = cppassist::make_unique<globjects::Shader>(gl::GL_VERTEX_SHADER,   g_vSource.get());
    g_geometryShader = cppassist::make_unique<globjects::Shader>(gl::GL_GEOMETRY_SHADER, g_gSource.get());
    g_fragmentShader = cppassist::make_unique<globjects::Shader>(gl::GL_FRAGMENT_SHADER, g_fSource.get());

    g_program = cppassist::make_unique<globjects::Program>();
    g_program->attach(g_vertexShader.get());
    g_program->attach(g_geometryShader.get());
    g_program->attach(g_fragmentShader.get());

    g_renderer = cppassist::make_unique<GlyphRenderer>(g_program.get());
}

void render()
{
    gl::glViewport(0, 0, g_viewport.x, g_viewport.y);

    gl::glDepthMask(gl::GL_FALSE);
    gl::glEnable(gl::GL_CULL_FACE);
    gl::glEnable(gl::GL_BLEND);
    gl::glBlendFunc(gl::GL_SRC_ALPHA, gl::GL_ONE_MINUS_SRC_ALPHA);

    g_renderer->render(*g_vertexCloud);

    gl::glDepthMask(gl::GL_TRUE);
    gl::glDisable(gl::GL_CULL_FACE);
    gl::glBlendFunc(gl::GL_ONE, gl::GL_ZERO);
    gl::glDisable(gl::GL_BLEND);
}

void initialize()
{
    std::cout << g_fontFilename << std::endl;

    // Load font file
    loadFont(g_fontFilename);
    createSequence();
    prepare();
    prepareRendering();
}

void deinitialize()
{
    /*
    g_program.reset(nullptr);
    g_vertexShaderSource.reset(nullptr);
    g_vertexShaderTemplate.reset(nullptr);
    g_vertexShader.reset(nullptr);
    g_fragmentShaderSource.reset(nullptr);
    g_fragmentShaderTemplate.reset(nullptr);
    g_fragmentShader.reset(nullptr);
    */
}

void draw()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render();
}


void error(int errnum, const char * errmsg)
{
    globjects::critical() << errnum << ": " << errmsg << std::endl;
}

void framebuffer_size_callback(GLFWwindow * /*window*/, int width, int height)
{
    g_viewport = glm::uvec2{ width, height };
}

void key_callback(GLFWwindow * window, int key, int /*scancode*/, int action, int /*modes*/)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        glfwSetWindowShouldClose(window, true);
}


int main(int /*argc*/, char * /*argv*/[])
{
    // Initialize GLFW
    if (!glfwInit())
        return 1;

    glfwSetErrorCallback(error);
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

    // Create a context and, if valid, make it current
    GLFWwindow * window = glfwCreateWindow(640, 480, "globjects Texture", NULL, NULL);
    if (window == nullptr)
    {
        globjects::critical() << "Context creation failed. Terminate execution.";

        glfwTerminate();
        return -1;
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);

    // Initialize globjects (internally initializes glbinding, and registers the current context)
    globjects::init();

    std::cout << std::endl
        << "OpenGL Version:  " << glbinding::ContextInfo::version() << std::endl
        << "OpenGL Vendor:   " << glbinding::ContextInfo::vendor() << std::endl
        << "OpenGL Renderer: " << glbinding::ContextInfo::renderer() << std::endl << std::endl;

    globjects::info() << "Press F5 to reload shaders." << std::endl << std::endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    g_viewport = glm::uvec2(width, height);
    initialize();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        draw();
        glfwSwapBuffers(window);
    }
    deinitialize();

    // Properly shutdown GLFW
    glfwTerminate();

    return 0;
}

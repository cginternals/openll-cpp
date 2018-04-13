
#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include <chrono>

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
#include <openll/Label.h>
#include <openll/GlyphVertexCloud.h>
#include <openll/GlyphRenderer.h>
#include <openll/Typesetter.h>


using namespace gl;
using namespace openll;


namespace
{
    // Text that is displayed
    const auto s_text =
    R"(Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Ste)";

    // Configuration of text rendering
    std::string    g_fontFilename("opensansr36.fnt");   ///< Font file
    std::u32string g_text;                              ///< Text to display
    float          g_fontSize(16.0f);                   ///< Font size (in pt)
    glm::vec2      g_origin(-1.0f, 1.0f);               ///< Origin position ( (-1, -1)..(1, 1), relative to the defined viewport )
    glm::vec4      g_margins(8.0f, 8.0f, 0.0f, 8.0f);   ///< Margins (top/right/bottom/left, in pt)
    float          g_pixelPerInch(72.0f);               ///< Number of pixels per inch
    bool           g_wordWrap(true);                    ///< Wrap words at the end of a line?
    float          g_lineWidth(0.0f);                   ///< Width of a line (in pt)
    Alignment      g_alignment(Alignment::LeftAligned); ///< Horizontal text alignment
    LineAnchor     g_lineAnchor(LineAnchor::Ascent);    ///< Vertical line anchor
    bool           g_optimized(true);                   ///< Optimize rendering performance?
    glm::uvec2     g_size;                              ///< Viewport size (in pixels)
    glm::vec4      g_textColor(0.0f, 0.0f, 0.0f, 1.0f); ///< Text color

    // Text rendering objects
    Label                             g_label;       ///< Label that is displayed
    std::unique_ptr<FontFace>         g_fontFace;    ///< Used font
    std::unique_ptr<GlyphVertexCloud> g_vertexCloud; ///< GPU geometry of the label
    std::unique_ptr<GlyphRenderer>    g_renderer;    ///< Label renderer

    // OpenGL objects
    std::unique_ptr<globjects::AbstractStringSource> g_vertexShaderSource;   ///< Shader source for the vertex shader
    std::unique_ptr<globjects::Shader>               g_vertexShader;         ///< Vertex shader
    std::unique_ptr<globjects::AbstractStringSource> g_geometryShaderSource; ///< Shader source for the geometry shader
    std::unique_ptr<globjects::Shader>               g_geometryShader;       ///< Geometry shader
    std::unique_ptr<globjects::AbstractStringSource> g_fragmentShaderSource; ///< Shader source for the fragment shader
    std::unique_ptr<globjects::Shader>               g_fragmentShader;       ///< Fragment shader
    std::unique_ptr<globjects::Program>              g_program;              ///< Shader program
}

void prepareText(size_t size)
{
    // Prepare text snippet
    auto snippet = cppassist::string::encode(std::string(s_text), cppassist::Encoding::UTF8);

    // Prepare large text
    for (unsigned int i=0; i<size; i++) {
        g_text += snippet;
        g_text += snippet;
    }
}

void loadFont(const std::string & filename)
{
    g_fontFace = FontLoader::load(filename);
}

void createLabel()
{
    g_lineWidth = g_size.x / g_pixelPerInch * 72.0f - (g_margins[0] + g_margins[1]);

    const auto scaledFontSize = g_fontSize; // * 16.0f;
    const auto scaledLineWidth = g_lineWidth; // * 160.0f;

    g_label.setText(g_text);
    g_label.setFontFace(*g_fontFace);
    g_label.setFontSize(scaledFontSize);
    g_label.setWordWrap(g_wordWrap);
    g_label.setLineWidth(scaledLineWidth);
    g_label.setAlignment(g_alignment);
    g_label.setLineAnchor(g_lineAnchor);
    g_label.setTextColor(g_textColor);
    g_label.setMargins(g_margins);

    g_label.setTransform2D(g_origin, g_size, g_pixelPerInch);
}

void prepare()
{
    assert(g_fontFace.get() != nullptr);

    // Prepare vertex cloud
    g_vertexCloud = std::unique_ptr<GlyphVertexCloud>(new GlyphVertexCloud);

    // Typeset and transform all labels
    auto extent = Typesetter::typeset(*g_vertexCloud, g_label, g_optimized);
    std::cout << "extent: (" << extent.x << ", " << extent.y << ")" << std::endl;
}

void prepareRendering()
{
    g_vertexShaderSource = GlyphRenderer::vertexShaderSource();
    g_vertexShader = cppassist::make_unique<globjects::Shader>(gl::GL_VERTEX_SHADER, g_vertexShaderSource.get());

    g_geometryShaderSource = GlyphRenderer::geometryShaderSource();
    g_geometryShader = cppassist::make_unique<globjects::Shader>(gl::GL_GEOMETRY_SHADER, g_geometryShaderSource.get());

    g_fragmentShaderSource = GlyphRenderer::fragmentShaderSource();
    g_fragmentShader = cppassist::make_unique<globjects::Shader>(gl::GL_FRAGMENT_SHADER, g_fragmentShaderSource.get());

    g_program = cppassist::make_unique<globjects::Program>();
    g_program->attach(g_vertexShader.get());
    g_program->attach(g_geometryShader.get());
    g_program->attach(g_fragmentShader.get());

    g_renderer = cppassist::make_unique<GlyphRenderer>(g_program.get());
}

void initialize()
{
    // Load font
    loadFont(openll::dataPath() + "/openll/fonts/" + g_fontFilename);

    createLabel();

    auto start = std::chrono::high_resolution_clock::now();

    prepare();

    auto end = std::chrono::high_resolution_clock::now();
    auto diff = end - start;
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(diff).count();
    std::cout << "Preparation: " << ns << " ns." << std::endl;

    prepareRendering();
}

void deinitialize()
{
    // Release OpenGL objects
    g_vertexShaderSource.reset(nullptr);
    g_vertexShader.reset(nullptr);
    g_geometryShaderSource.reset(nullptr);
    g_geometryShader.reset(nullptr);
    g_fragmentShaderSource.reset(nullptr);
    g_fragmentShader.reset(nullptr);
    g_program.reset(nullptr);
}

void resize()
{
    g_lineWidth = g_size.x / g_pixelPerInch * 72.0f - (g_margins[0] + g_margins[1]);

    const auto scaledLineWidth = g_lineWidth;

    g_label.setTransform2D(g_origin, g_size, g_pixelPerInch);
    g_label.setLineWidth(scaledLineWidth);

    // Typeset and transform all labels
    auto extent = Typesetter::typeset(*g_vertexCloud, g_label, g_optimized);
    std::cout << "extent: (" << extent.x << ", " << extent.y << ")" << std::endl;
}

void draw()
{
    // Clear screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set viewport
    gl::glViewport(0, 0, g_size.x, g_size.y);

    // Set rendering states
    gl::glDepthMask(gl::GL_FALSE);
    gl::glEnable(gl::GL_CULL_FACE);
    gl::glEnable(gl::GL_BLEND);
    gl::glBlendFunc(gl::GL_SRC_ALPHA, gl::GL_ONE_MINUS_SRC_ALPHA);

    // Render text
    g_renderer->render(*g_vertexCloud);

    // Reset rendering states
    gl::glDepthMask(gl::GL_TRUE);
    gl::glDisable(gl::GL_CULL_FACE);
    gl::glBlendFunc(gl::GL_ONE, gl::GL_ZERO);
    gl::glDisable(gl::GL_BLEND);
}

void onError(int errnum, const char * errmsg)
{
    globjects::critical() << errnum << ": " << errmsg << std::endl;
}

void onResize(GLFWwindow *, int width, int height)
{
    g_size = glm::uvec2(width, height);
    resize();
}

void onKeyEvent(GLFWwindow * window, int key, int, int action, int)
{
    // Escape: Close window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(int, char *[])
{
    // Initialize GLFW
    if (!glfwInit()) {
        // Abort on error
        return 1;
    }

    // Set window options
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);

    // Create window and context
    GLFWwindow * window = glfwCreateWindow(640, 480, "globjects Texture", NULL, NULL);
    if (!window)
    {
        // Abort if window creation failed
        globjects::critical() << "Context creation failed. Terminate execution.";
        glfwTerminate();
        return -1;
    }

    // Register callbacks
    glfwSetErrorCallback(onError);
    glfwSetFramebufferSizeCallback(window, onResize);
    glfwSetKeyCallback(window, onKeyEvent);

    // Make context current
    glfwMakeContextCurrent(window);

    // Initialize globjects (internally initializes glbinding and registers the current context)
    globjects::init();

    // Output OpenGL version information
    std::cout << std::endl
        << "OpenGL Version:  " << glbinding::ContextInfo::version() << std::endl
        << "OpenGL Vendor:   " << glbinding::ContextInfo::vendor() << std::endl
        << "OpenGL Renderer: " << glbinding::ContextInfo::renderer() << std::endl << std::endl;

    // Get framebuffer size
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    g_size = glm::uvec2(width, height);

    // Prepare text
    prepareText(1);    // 1 kB
//    prepareText(1024); // 1 MB
    std::cout << "Text size: " << g_text.size() << std::endl;

    // Initialize OpenGL objects in context
    initialize();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        draw();
        glfwSwapBuffers(window);
    }

    // Release OpenGL objects
    deinitialize();

    // Shutdown GLFW
    glfwTerminate();

    return 0;
}

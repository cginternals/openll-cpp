
#include <iostream>
#include <chrono>

#include <glm/glm.hpp>

#include <GLFW/glfw3.h>

#include <cppassist/string/conversion.h>

#include <glbinding/gl/gl.h>
#include <glbinding-aux/ContextInfo.h>
#include <glbinding/Version.h>
#include <glbinding-aux/types_to_string.h>

#include <globjects/globjects.h>

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
    const auto s_text = R"(Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum.)";

    // Configuration of text rendering
    std::string    g_fontFilename("opensansr36.fnt");    ///< Font file
    float          g_fontSize(36.0f);                    ///< Font size (in pt)
    glm::ivec2     g_pos(0, 0);                          ///< Text position (in px)
    glm::ivec2     g_size(250, 50);                      ///< Text size (in px)
    glm::ivec4     g_margins(10, 40, 0, 40);             ///< Margins (top/right/bottom/left, in px)
    float          g_pixelPerInch(72.0f);                ///< Number of pixels per inch
    bool           g_wordWrap(true);                     ///< Wrap words at the end of a line?
    Alignment      g_alignment(Alignment::LeftAligned);  ///< Horizontal text alignment
    LineAnchor     g_lineAnchor(LineAnchor::Ascent);     ///< Vertical line anchor
    bool           g_optimized(true);                    ///< Optimize rendering performance?
    glm::uvec2     g_screenSize;                         ///< Screen size (in pixels)
    glm::vec4      g_textColor(0.0f, 0.0f, 0.0f, 1.0f);  ///< Text color

    // Text rendering objects
    Label                             g_label;       ///< Label that is displayed
    std::unique_ptr<FontFace>         g_fontFace;    ///< Used font
    std::unique_ptr<GlyphVertexCloud> g_vertexCloud; ///< GPU geometry of the label
    std::unique_ptr<GlyphRenderer>    g_renderer;    ///< Label renderer
}

void initialize()
{
    auto text = std::string(s_text);

    for (auto i = 0; i < 0; ++i)
    {
         text += "\n" + text;
    }

    std::cout << "Using a text with " << text.size() << " characters (" << static_cast<float>(text.size() / 1024.0 / 1024.0) << "MB)" << std::endl;

    // Set text
    const auto encodedText = cppassist::string::encode(std::string(text), cppassist::Encoding::UTF8);

    // Load font
    g_fontFace = FontLoader::load(openll::dataPath() + "/openll/fonts/" + g_fontFilename);

    // Create label
    g_label.setText(std::move(encodedText));
    g_label.setFontFace(*g_fontFace);
    g_label.setFontSize(g_fontSize);
    g_label.setWordWrap(g_wordWrap);
    g_label.setAlignment(g_alignment);
    g_label.setLineAnchor(g_lineAnchor);
    g_label.setTextColor(g_textColor);

    // Create vertex cloud
    g_vertexCloud = std::unique_ptr<GlyphVertexCloud>(new GlyphVertexCloud);

    // Create glyph renderer
    g_renderer = std::unique_ptr<GlyphRenderer>(new GlyphRenderer);
}

void deinitialize()
{
    // Release OpenGL objects
    g_renderer.reset(nullptr);
    g_vertexCloud.reset(nullptr);
    g_fontFace.reset(nullptr);
}

void update()
{
    // Update label size to match the screen size
    g_size = g_screenSize;

    // Calculate reference position according to alignment
    int x = 0;
         if (g_alignment == Alignment::LeftAligned)  x = g_pos.x;
    else if (g_alignment == Alignment::RightAligned) x = g_pos.x + g_size.x - 1;
    else if (g_alignment == Alignment::Centered)     x = g_pos.x + g_size.x / 2;

    // Calculate label position in NDC
    glm::vec2 origin(
        2.0f * (float)x / (g_screenSize.x - 1) - 1.0f,
        2.0f * (float)(g_screenSize.y - g_pos.y) / (g_screenSize.y - 1) - 1.0f
    );

    // Calculate margins in pt
    glm::vec4 margins = g_margins;
    margins *= 72.0f / (float)g_pixelPerInch;

    // Calculate line width in pt
    float lineWidth = g_size.x / g_pixelPerInch * 72.0f - (margins[1] + margins[3]);

    // Update label
    g_label.setLineWidth(lineWidth);
    g_label.setMargins(margins);
    g_label.setTransform2D(origin, g_screenSize, g_pixelPerInch);

    const auto start = std::chrono::high_resolution_clock::now();

    // Execute typesetter
    auto extent = Typesetter::typeset(*g_vertexCloud, g_label, g_optimized);

    const auto end = std::chrono::high_resolution_clock::now();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

    (void)extent;
}

void draw()
{
    // Clear screen
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set viewport
    gl::glViewport(0, 0, g_screenSize.x, g_screenSize.y);

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
    // Set new screen size
    g_screenSize = glm::uvec2(width, height);

    // Update label
    update();
}

void onKeyEvent(GLFWwindow * window, int key, int, int action, int)
{
    // Escape: Close window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(int, char *[])
{
    // Initialize GLFW
    if (!glfwInit())
    {
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
    globjects::init(glfwGetProcAddress);

    // Output OpenGL version information
    std::cout << std::endl
        << "OpenGL Version:  " << glbinding::aux::ContextInfo::version() << std::endl
        << "OpenGL Vendor:   " << glbinding::aux::ContextInfo::vendor() << std::endl
        << "OpenGL Renderer: " << glbinding::aux::ContextInfo::renderer() << std::endl << std::endl;

    // Get framebuffer size
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    g_screenSize = glm::uvec2(width, height);

    // Initialize OpenGL objects in context
    initialize();

    // Update label
    update();

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

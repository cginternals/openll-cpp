
#include <iostream>
#include <algorithm>

#include <glm/vec2.hpp>

#include <glbinding/gl/gl.h>
#include <glbinding-aux/ContextInfo.h>
#include <glbinding/Version.h>
#include <glbinding-aux/types_to_string.h>

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include <globjects/globjects.h>
#include <globjects/logging.h>
#include <globjects/base/File.h>

#include <globjects/Uniform.h>
#include <globjects/Program.h>
#include <globjects/Shader.h>
#include <globjects/Buffer.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/Texture.h>

#include <openll/openll.h>
#include <openll/Alignment.h>
#include <openll/FontFace.h>
#include <openll/FontLoader.h>
#include <openll/GlyphRenderer.h>
#include <openll/Label.h>
#include <openll/Text.h>
#include <openll/GlyphVertexCloud.h>
#include <openll/LineAnchor.h>
#include <openll/Typesetter.h>

#include "PointDrawable.h"


using namespace gl;


namespace
{
	auto g_frame = 0u; //unused
	auto g_size = glm::ivec2{};
	auto g_size_changed = true;

	std::vector<Point> g_points;
	PointDrawable * g_pointDrawable = nullptr;

	float g_time = 0.f;

    std::vector<openll::GlyphVertexCloud> g_vertexClouds;
    std::unique_ptr<openll::FontFace> g_font;

	//different configs we will use in this demo
    std::vector<openll::Label> g_labelConfigs;

    void setFromConfig(openll::Label & target, const openll::Label & config) {
        target.setWordWrap(config.wordWrap());
        target.setLineWidth(config.lineWidth());
        target.setAlignment(config.alignment());
        target.setLineAnchor(config.lineAnchor());
        target.setFontFace(config.fontFace());
        target.setFontSize(config.fontSize());
        target.setTextColor(config.textColor());
        target.setMargins(config.margins());
    }
}

void prepareGlyphSequences(openll::GlyphVertexCloud & cloud, const glm::vec2 origin, std::string string, openll::Label config)
{
    openll::Label sequence;
    sequence.setText(std::u32string(string.begin(), string.end()));

	// set wordWrap, lineWidth, alignment, lineAnchor, fontFace, fontSize, fontColor
    setFromConfig(sequence, config);

    sequence.setTransform2D(origin, g_size);

    openll::Typesetter::typeset(cloud, sequence);
}

void preparePoint(openll::GlyphVertexCloud & cloud, const glm::vec2 origin, openll::FontFace * font) {
	const std::string pointText = R"(.)";

    std::vector<openll::Label> sequences;

    openll::Label sequence;
	std::u32string unicode_string(pointText.begin(), pointText.end());
    sequence.setText(unicode_string);

	sequence.setWordWrap(true);
	sequence.setLineWidth(500.f);
    sequence.setAlignment(openll::Alignment::Centered);
    sequence.setLineAnchor(openll::LineAnchor::Baseline);
	sequence.setFontFace(font);
	sequence.setFontSize(64.f);
    sequence.setMargins({ 0.f, 0.f, 0.f, 0.f });

    sequence.setTransform2D(origin, g_size);

	sequences.push_back(sequence);

    openll::Typesetter::typeset(cloud, sequence);
}


void annotatePoint(openll::GlyphVertexCloud & cloud, const glm::vec2 origin, const glm::vec2 offset, std::string string, openll::Label config)
{
    prepareGlyphSequences(cloud, origin + offset, string, config);
}


void initialize()
{
    const auto dataPath = openll::dataPath();

    g_font = openll::FontLoader::load(dataPath + "/fonts/opensansr36/opensansr36.fnt");
	
	g_pointDrawable = new PointDrawable{ dataPath };

	//initialize all used configs. There is no need to initialize them and never change them. It's just this example.
    auto defaultConfig = openll::Label();
    defaultConfig.setFontFace(g_font.get());
    g_labelConfigs.push_back(defaultConfig);

    auto configBigRed = openll::Label();
    configBigRed.setFontFace(g_font.get());
    configBigRed.setTextColor(glm::vec4(1.f, 0.f, 0.f, 1.f));
	configBigRed.setFontSize(32.f);
    g_labelConfigs.push_back(configBigRed);

    auto configVertical = openll::Label();
    configVertical.setFontFace(g_font.get());
    configVertical.setAlignment(openll::Alignment::Centered);
    configVertical.setLineAnchor(openll::LineAnchor::Center);
	configVertical.setWordWrap(true);
	configVertical.setLineWidth(0);
    g_labelConfigs.push_back(configVertical);


	//this is so bad hard-coded
	std::vector<glm::vec2> points{glm::vec2(-0.2, 0.f), glm::vec2(0.2, -0.2), glm::vec2(-0.2, -0.6)
		, glm::vec2(-0.8, 0.8), glm::vec2(-0.7, 0.4), glm::vec2(0.8, 0.8) };

	for (auto point : points) {
		g_points.push_back({
			point,
			glm::vec3(0.f, 0.f, 1.f), //color: blue
			10.f * g_size.x / 1000 //size
		});
	}
	g_pointDrawable->initialize(g_points);

	glClearColor(1.f, 1.f, 1.f, 1.f);
}

void deinitialize()
{
	globjects::detachAllObjects();
}

void draw(openll::GlyphRenderer &renderer)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (g_size_changed)
	{
		glViewport(0, 0, g_size.x, g_size.y);

        g_vertexClouds.resize(6);

        annotatePoint(g_vertexClouds[0], glm::vec2(-0.2f, 0.f), glm::vec2(0.f, 0.1f), "annotated with default config", g_labelConfigs.at(0));
        annotatePoint(g_vertexClouds[1], glm::vec2(0.2f, -0.2f), glm::vec2(0.f, -0.1f), "this one also with default config", g_labelConfigs.at(0));
        annotatePoint(g_vertexClouds[2], glm::vec2(-0.2f, -0.6f), glm::vec2(0.1, 0.f), "is that point clear? hehe point.", g_labelConfigs.at(0));

        annotatePoint(g_vertexClouds[3], glm::vec2(-0.8f, 0.8f), glm::vec2(0.1, -0.05f), "annotated with big red config", g_labelConfigs.at(1));
        annotatePoint(g_vertexClouds[4], glm::vec2(-0.7f, 0.4f), glm::vec2(0.1, -0.05f), "still big and red!", g_labelConfigs.at(1));

        annotatePoint(g_vertexClouds[5], glm::vec2(0.8f, 0.8f), glm::vec2(0.05f, 0.f), "one line vertical", g_labelConfigs.at(2));
	}
	
	gl::glDepthMask(gl::GL_FALSE);
	gl::glEnable(gl::GL_CULL_FACE);
	gl::glEnable(gl::GL_BLEND);
	gl::glBlendFunc(gl::GL_SRC_ALPHA, gl::GL_ONE_MINUS_SRC_ALPHA);

	glm::vec3 myRotationAxis(0, 1, 0);
	auto m = glm::mat4();
	auto rotatedM = glm::rotate(m, 0.2f, myRotationAxis);
	g_time+=0.004;

	g_pointDrawable->render();

	for (size_t i = 0; i < g_vertexClouds.size(); i++) {
		glm::vec3 myRotationAxis(0, 1, 0);
		auto m = glm::mat4();
		auto rotatedM = glm::rotate(m, g_time, myRotationAxis);
		renderer.renderInWorld(g_vertexClouds[i],rotatedM);
	}

	gl::glDepthMask(gl::GL_TRUE);
	gl::glDisable(gl::GL_CULL_FACE);
	gl::glDisable(gl::GL_BLEND);

	g_size_changed = false;
}


void error(int errnum, const char * errmsg)
{
	globjects::critical() << errnum << ": " << errmsg << std::endl;
}

void framebuffer_size_callback(GLFWwindow * /*window*/, int width, int height)
{
	g_size = glm::ivec2{ width, height };

	g_size_changed = true;
}

void key_callback(GLFWwindow * window, int key, int /*scancode*/, int action, int /*mods*/)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, 1);

	if (key == GLFW_KEY_F5 && action == GLFW_RELEASE)
    {
        // globjects::File::reload();
    }
}

int main()
{
#ifdef SYSTEM_DARWIN
	globjects::critical() << "macOS does currently not support compute shader (OpenGL 4.3. required).";
	return 0;
#endif

	// Initialize GLFW
	if (!glfwInit())
		return 1;

	glfwSetErrorCallback(error);
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a context and, if valid, make it current
	GLFWwindow * window = glfwCreateWindow(1000, 1000, "ll-opengl | labeling-at-point", nullptr, nullptr);

	if (!window)
	{
		globjects::critical() << "Context creation failed. Terminate execution.";

		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwMakeContextCurrent(window);

	// Initialize globjects (internally initializes glbinding, and registers the current context)
    globjects::init(glfwGetProcAddress);

	std::cout << std::endl
        << "OpenGL Version:  " << glbinding::aux::ContextInfo::version() << std::endl
        << "OpenGL Vendor:   " << glbinding::aux::ContextInfo::vendor() << std::endl
        << "OpenGL Renderer: " << glbinding::aux::ContextInfo::renderer() << std::endl << std::endl;

	globjects::DebugMessage::enable();

	// globjects::info() << "Press F5 to reload compute shader." << std::endl << std::endl;

	glfwGetFramebufferSize(window, &g_size[0], &g_size[1]);
	initialize();

    openll::GlyphRenderer renderer;

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		draw(renderer);
		glfwSwapBuffers(window);
	}
	deinitialize();

	// Properly shutdown GLFW
	glfwTerminate();

	return 0;
}

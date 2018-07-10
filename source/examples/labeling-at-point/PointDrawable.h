#pragma once

#include <vector>
#include <string>
#include <memory>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace globjects
{
    class VertexArray;
    class Program;
    class Shader;
    class AbstractStringSource;
}

struct Point
{
    glm::vec2 coords;
    glm::vec3 color;
    float size;
};

class PointDrawable
{
public:
    PointDrawable(const std::string & dataPath);
    ~PointDrawable();

    void initialize(const std::vector<Point> & points);
    void render();

private:
    std::unique_ptr<globjects::VertexArray> m_vao;
    std::unique_ptr<globjects::AbstractStringSource> m_vertexShaderSource;
    std::unique_ptr<globjects::AbstractStringSource> m_fragmentShaderSource;
    std::unique_ptr<globjects::Program> m_program;
    std::unique_ptr<globjects::Shader> m_vertexShader;
    std::unique_ptr<globjects::Shader> m_fragmentShader;
    int m_count;
};

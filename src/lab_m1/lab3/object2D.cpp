#include "object2D.h"

#include <vector>

#include "core/engine.h"
#include "utils/gl_utils.h"

Mesh* object2D::CreateTriangle(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length * 0.5f, length / 1.75f * sqrt(3), 0), color),        // equilateral triangle
    };

    Mesh* triangle = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2 };

    if (!fill) {
        triangle->SetDrawMode(GL_LINE_LOOP);
    }
    //else {
    //    // Draw 1 triangle. Add the remaining 2 indices
    //    indices.push_back(0);
    //    //indices.push_back(2);
    //}

    triangle->InitFromData(vertices, indices);
    return triangle;
}

Mesh* object2D::CreateSquare(
    const std::string &name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    } else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

Mesh* object2D::CreatePentagon(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length * 0.65f, 0), color),
        VertexFormat(corner + glm::vec3(length * 0.5f, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length * 0.65f, 0), color),
    };

    Mesh* pentagon = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3, 4};

    if (!fill) {
        pentagon->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices = { 0, 1, 2,
                    0, 2, 3,
                    0, 3, 4 };
    }

    pentagon->InitFromData(vertices, indices);
    return pentagon;
}

Mesh* object2D::CreateShield(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length * 0.25f, -length * 0.15f, 0), color),
        VertexFormat(corner + glm::vec3(length * 0.75f, -length * 0.15f, 0), color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length * 0.65f, 0), color),
        VertexFormat(corner + glm::vec3(length * 0.5f, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length * 0.65f, 0), color),

    };

    Mesh* pentagon = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3, 4, 5, 6 };

    if (!fill) {
        pentagon->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        // Draw 2 triangles. Add the remaining 2 indices
        indices = { 0, 1, 2,
                    0, 2, 3,
                    0, 3, 4,
                    0, 4, 5,
                    0, 5, 6 };
    }

    pentagon->InitFromData(vertices, indices);
    return pentagon;
}
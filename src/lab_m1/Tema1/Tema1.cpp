#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema1::Tema1()
{
}


Tema1::~Tema1()
{
}


void Tema1::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 100;

    // TODO(student): Compute coordinates of a square's center, and store
    // then in the `cx` and `cy` class variables (see the header). Use
    // `corner` and `squareSide`. These two class variables will be used
    // in the `Update()` function. Think about it, why do you need them?

    cx = 50;
    cy = 50;

    // Initialize tx and ty (the translation steps)
    translateX = 0;
    translateY = 0;

    // Initialize sx and sy (the scale factors)
    scaleX = 1;
    scaleY = 1;

    // Initialize car coordonates
    carX = 50;
    carY = 50;
    rotationAngle = 0;  // rad

    // Initialize angularStep
    angularStep = 0;

    Mesh* square1 = object2D::CreateSquare("square1", corner, squareSide, glm::vec3(1, 0, 0), true);
    AddMeshToList(square1);

    Mesh* square2 = object2D::CreateSquare("square2", corner, squareSide, glm::vec3(0, 1, 0));
    AddMeshToList(square2);

    Mesh* square3 = object2D::CreateSquare("square3", corner, squareSide, glm::vec3(0, 0, 1));
    AddMeshToList(square3);

    Mesh* square4 = object2D::CreateSquare("square4", corner, squareSide, glm::vec3(0, 1, 1), true);
    AddMeshToList(square4);

    Mesh* square5 = object2D::CreateSquare("square5", corner, squareSide, glm::vec3(1, 0, 1), true);
    AddMeshToList(square5);
}


void Tema1::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Tema1::Update(float deltaTimeSeconds)
{
    // TODO(student): Update steps for translation, rotation and scale,
    // in order to create animations. Use the class variables in the
    // class header, and if you need more of them to complete the task,
    // add them over there!

    //int tx = cx, ty = cy;
    if (translateY < 310 && ascending) {
        translateX += 100 * deltaTimeSeconds;
        translateY += 100 * deltaTimeSeconds;
    }
    else
        ascending = false;
    if (translateY > 190 && !ascending) {
        translateX -= 100 * deltaTimeSeconds;
        translateY -= 100 * deltaTimeSeconds;
    }
    else
        ascending = true;

    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(translateY, translateX);
    // TODO(student): Create animations by multiplying the current
    // transform matrix with the matrices you just implemented.
    // Remember, the last matrix in the chain will take effect first!

    RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);

    if (scaleY < 1.5f && growing) {
        scaleX += 1 * deltaTimeSeconds;
        scaleY += 1 * deltaTimeSeconds;
    }
    else
        growing = false;
    if (scaleY > 1 && !growing) {
        scaleY -= 1 * deltaTimeSeconds;
        scaleX -= 1 * deltaTimeSeconds;
    }
    else
        growing = true;

    modelMatrix *= transform2D::Translate(500, 300);
    modelMatrix *= transform2D::Translate(cx, cy);
    modelMatrix *= transform2D::Scale(scaleX, scaleY);
    modelMatrix *= transform2D::Translate(-cx, -cy);
    // TODO(student): Create animations by multiplying the current
    // transform matrix with the matrices you just implemented
    // Remember, the last matrix in the chain will take effect first!

    RenderMesh2D(meshes["square2"], shaders["VertexColor"], modelMatrix);

    modelMatrix = glm::mat3(1);
    angularStep += 3 * deltaTimeSeconds;

    modelMatrix *= transform2D::Translate(800, 250);
    modelMatrix *= transform2D::Translate(cx, cy);
    modelMatrix *= transform2D::Rotate(angularStep);
    modelMatrix *= transform2D::Translate(-cx, -cy);

    // TODO(student): Create animations by multiplying the current
    // transform matrix with the matrices you just implemented
    // Remember, the last matrix in the chain will take effect first!

    RenderMesh2D(meshes["square3"], shaders["VertexColor"], modelMatrix);


    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(carX, carY);
    modelMatrix *= transform2D::Translate(cx, cy);
    modelMatrix *= transform2D::Rotate(rotationAngle);
    modelMatrix *= transform2D::Translate(-cx, -cy);

    modelMatrix *= transform2D::Translate(cx, cy);
    modelMatrix *= transform2D::Scale(0.8f, 0.5f);
    modelMatrix *= transform2D::Translate(-cx, -cy);
    RenderMesh2D(meshes["square4"], shaders["VertexColor"], modelMatrix);

    // just keep using the car's modelMatrix to track the transform
    modelMatrix *= transform2D::Translate(0, cy);
    modelMatrix *= transform2D::Scale(0.3f, 1.2f);
    modelMatrix *= transform2D::Translate(0, -cy);
    RenderMesh2D(meshes["square5"], shaders["VertexColor"], modelMatrix);
}


void Tema1::FrameEnd()
{
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
    //float speed = 300 * deltaTime;
    //float rotationSpeed = 6 * deltaTime;

    //if (window->KeyHold(GLFW_KEY_W)) {
    //    //if (rotationAngle < glm::radians(90.0f))
    //    carX += cos(rotationAngle) * speed;
    //    carY += sin(rotationAngle) * speed;

    //    //carY += cos(rotationAngle) * speed;
    //    //if (rotationAngle < glm::radians(90)) {
    //    //    rotationAngle += 30 * deltaTime;
    //    //    rotationAngle = glm::radians(rotationAngle);
    //    //}

    //    //rotationAngle = 90;
    //    //rotationAngle = glm::radians(rotationAngle);
    //}

    //if (window->KeyHold(GLFW_KEY_S)) {
    //    carX -= cos(rotationAngle) * speed;
    //    carY -= sin(rotationAngle) * speed;
    //}
    //if (window->KeyHold(GLFW_KEY_D)) {
    //    rotationAngle -= rotationSpeed;
    //}
    //if (window->KeyHold(GLFW_KEY_A)) {
    //    rotationAngle += rotationSpeed;
    //}
}


    void Tema1::OnKeyPress(int key, int mods)
    {
        // Add key press event
    }


    void Tema1::OnKeyRelease(int key, int mods)
    {
        // Add key release event
    }


    void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
    {
        // Add mouse move event
    }


    void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
    {
        // Add mouse button press event
    }


    void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
    {
        // Add mouse button release event
    }


    void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
    {
    }


    void Tema1::OnWindowResize(int width, int height)
    {
    }
#include "lab_m1/lab1/lab1.h"

#include <vector>
#include <iostream>
#include <cmath>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab1::Lab1()
{
    // TODO(student): Never forget to initialize class variables!

}


Lab1::~Lab1()
{
}


void Lab1::Init()
{
    // Load a mesh from file into GPU memory. We only need to do it once,
    // no matter how many times we want to draw this mesh.
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;

        Mesh* bunnyMesh = new Mesh("bunny");
        bunnyMesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "monke.obj");
        meshes[bunnyMesh->GetMeshID()] = bunnyMesh;
    }

    // TODO(student): Load some more meshes. The value of RESOURCE_PATH::MODELS
    // is actually a path on disk, go there and you will find more meshes.

}


void Lab1::FrameStart()
{
}


void Lab1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->props.resolution;

    // Sets the clear color for the color buffer

    // TODO(student): Generalize the arguments of `glClearColor`.
    // You can, for example, declare three variables in the class header,
    // that will store the color components (red, green, blue).
    glClearColor(envColor.x, envColor.y, envColor.z, 1);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

    // Render the object
    RenderMesh(meshes["box"], glm::vec3(1, 0.5f, 0), glm::vec3(0.5f));

    if (changingState == 0)
        meshes["bunny"]->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "monke.obj");
    if (changingState == 1)
        meshes["bunny"]->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
    if (changingState == 2)
        meshes["bunny"]->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "animals"), "bunny.obj");

    // MY GAME JUMPINGS
    //if (!isJumping && window->KeyHold(GLFW_KEY_SPACE)) {
    //    isJumping = true;
    //    targetX = pos.x + 2;
    //    initY = pos.y;
    //    maxH = 1;
    //}
    //else 
    //{
    //    if (isJumping) {
    //        //pos.x += 2 * deltaTimeSeconds;
    //        if (pos.y < jumpRadius && !isFalling) {
    //            pos.y += 2 * deltaTimeSeconds;
    //            // velocity = (jumpRadius - pos.y) / deltaTimeSeconds; // ???
    //        }
    //        else {
    //            isFalling = true;
    //            if (pos.y > initY) {
    //                pos.y -= 2 * deltaTimeSeconds;
    //            }
    //            else {
    //                isJumping = false;
    //                isFalling = false;
    //            }
    //        }
    //        //pos.y = sqrt(jumpRadius * jumpRadius - pos.x * pos.x) * 0.001f * deltaTimeSeconds;
    //    }
    //}

    if (!isJumping && window->KeyHold(GLFW_KEY_SPACE)) {
        isJumping = true;
        targetX = pos.x + 2;
        startX = pos.x;
        maxH = 0.3f;
    }

    if (isJumping) {
        if (targetX - pos.x > 0) {
            pos.x += 1.5f * deltaTimeSeconds;
            pos.y = (2 * (pos.x - targetX) * (pos.x - startX) * -0.5f + 0.2f); // y = 0 <=> x == targetX sau x == startX
        }
        else
            isJumping = false;
    }

    // Render bunny
    RenderMesh(meshes["bunny"], glm::vec3(pos.x, pos.y, pos.z), glm::vec3(1));

    // Render the object again but with different properties
    RenderMesh(meshes["box"], glm::vec3(-1, 0.5f, 0));

    // TODO(student): We need to render (a.k.a. draw) the mesh that
    // was previously loaded. We do this using `RenderMesh`. Check the
    // signature of this function to see the meaning of its parameters.
    // You can draw the same mesh any number of times.
}


void Lab1::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input

    // TODO(student): Add some key hold events that will let you move
    // a mesh instance on all three axes. You will also need to
    // generalize the position used by `RenderMesh`.

    // todo 5
    float moveSpeed = 4 * deltaTime;
    if (window->KeyHold(GLFW_KEY_W))
        pos.z -= moveSpeed;

	if (window->KeyHold(GLFW_KEY_S))
		pos.z += moveSpeed;
	
    if (window->KeyHold(GLFW_KEY_A))
		pos.x -= moveSpeed;
	
    if (window->KeyHold(GLFW_KEY_D))
		pos.x += moveSpeed;
	
    if (window->KeyHold(GLFW_KEY_E))
		pos.y -= moveSpeed;
	
    if (window->KeyHold(GLFW_KEY_Q))
		pos.y += moveSpeed;
}


void Lab1::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_F) {
        // TODO(student): Change the values of the color components.

    }

    if (key == GLFW_KEY_P) {
        if (envState == 0)
            envColor.z = 0.3f;
        else envColor.z = 0;

        envState = (envState + 1) % 2;
    }

    // TODO(student): Add a key press event that will let you cycle
    // through at least two meshes, rendered at the same position.
    // You will also need to generalize the mesh name used by `RenderMesh`.

    if (key == GLFW_KEY_1) {
        changingState = (changingState + 1) % 3;
    }

    //if (key == GLFW_KEY_SPACE) {
    //    int newX = pos.x + 2;
    //    while (pos.x != newX) {
    //        pos.x += 0.1f;
    //        pos.y = sqrt(jumpRadius * jumpRadius - pos.x * pos.x);
    //    }
    //}

    // Funny grid-like movevent
	//if (key == GLFW_KEY_W) {
	//	pos.x += 0.5f;
	//}

 //   if (key == GLFW_KEY_S) {
 //       pos.x -= 0.5f;
 //   }

 //   if (key == GLFW_KEY_A) {
 //       pos.z += 0.5f;
 //   }

 //   if (key == GLFW_KEY_D) {
 //       pos.z -= 0.5f;
 //   }

 //   if (key == GLFW_KEY_E) {
 //       pos.y += 0.5f;
 //   }

 //   if (key == GLFW_KEY_Q) {
 //       pos.y -= 0.5f;
 //   }
}


void Lab1::OnKeyRelease(int key, int mods)
{
    // Add key release event
    //if (key == GLFW_KEY_P && envColor.z == 0.3f)
    //    envColor.z = 0;
}


void Lab1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Lab1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}

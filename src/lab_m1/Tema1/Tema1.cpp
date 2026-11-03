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
	glm::ivec2 resolution = window->GetResolution();    // get resolution of the window
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    // player always starts first for convenience
    // TODO: make first turn random after implementing opponent AI
    isPlayerTurn = true;

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 1;

    // TODO(student): Compute coordinates of a square's center, and store
    // then in the `cx` and `cy` class variables (see the header). Use
    // `corner` and `squareSide`. These two class variables will be used
    // in the `Update()` function. Think about it, why do you need them?

    cx = corner.x + squareSide / 2;
    cy = corner.y + squareSide / 2;

    // Initialize tx and ty (the translation steps)
    translateX = resolution.x * 0.195f;
    translateY = resolution.y * 0.25f;

    // Initialize sx and sy (the scale factors)
    scaleX = resolution.x * 0.8f;
    scaleY = resolution.y * 0.12f;

    // Initialize car coordonates
    carX = 50;
    carY = 50;
    rotationAngle = 0;  // rad

    // Initialize angularStep
    angularStep = 0;

    // Northern Realms Deck
    // DEVNOTE: Try to keep very few healers for balancing reasons
    // total: 22 cards  -- TODO: later add x3 Decoy as DECOY type and x2 Commander's Horn as HORN (?) type
    // total strength = 149
    std::vector<Card*> northernRealmsDeck = {
        new Card("Geralt of Rivia", ATK, MELEE, 15),
        new Card("Ciri", HEAL, MELEE, 15),
        new Card("Yennefer of Vengerberg", ATK, RANGED, 7),
        new Card("Triss Merigold", DEF, RANGED, 7),
        new Card("Philippa Eilhart", ATK, RANGED, 10),
        new Card("Vernon Roche", ATK, MELEE, 10),
        new Card("John Natalis", DEF, MELEE, 10),
        new Card("Esterad Thyssen", ATK, MELEE, 10),
        new Card("Myserious ELF", HEAL, MELEE, 5), // TODO: turn into spy later
        new Card("Villentretenmerth", ATK, MELEE, 7),
        new Card("Catapult", ATK, SIEGE, 8),
        new Card("Catapult", ATK, SIEGE, 8),
        new Card("Prince Stennis", DEF, MELEE, 5),  // TODO: turn into spy later
        new Card("Crinfrid Reavers Dragon Hunter", DEF, RANGED, 5), // TODO: implement doubling score mechanic for doubles
        new Card("Crinfrid Reavers Dragon Hunter", DEF, RANGED, 5),
        new Card("Crinfrid Reavers Dragon Hunter", DEF, RANGED, 5),
        new Card("Dun Banner Medic", HEAL, SIEGE, 5),
        new Card("Sigismund Dijkstra", DEF, MELEE, 4),
        new Card("Blue Stripes Commando", ATK, MELEE, 4),
        new Card("Blue Stripes Commando", ATK, MELEE, 4),
        new Card("Blue Stripes Commando", ATK, MELEE, 4),
        new Card("Thaler", ATK, SIEGE, 1)   // TODO: turn into spy later
    };

    // player lanes
    Mesh* playerSquare = object2D::CreateSquare("playerSquare", corner, squareSide, glm::vec3(0, 0, 1), true);
    AddMeshToList(playerSquare);

	Mesh* playerHand = object2D::CreateSquare("playerHand", corner, squareSide, glm::vec3(0.32f, 0.24f, 0.16f), true);
    AddMeshToList(playerHand);

    Mesh* statsBar = object2D::CreateSquare("statsBar", corner, squareSide, glm::vec3(0.32f, 0.24f, 0.16f), true);
    AddMeshToList(statsBar);

	Lane* Hand = new Lane("Hand", true, playerSquare);

	Lane* melee1 = new Lane("melee1", true, playerSquare);
    Lane* range1 = new Lane("ran1", true, playerSquare);
    Lane* siege1 = new Lane("sieg1", true, playerSquare);

	// enemy lanes
	Mesh* enemySquare = object2D::CreateSquare("enemySquare", corner, squareSide, glm::vec3(1, 0, 0), true);
	AddMeshToList(enemySquare);

	Lane* melee2 = new Lane("melee2", false, enemySquare);
	Lane* range2 = new Lane("ran2", false, enemySquare);
	Lane* siege2 = new Lane("sieg2", false, enemySquare);
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
	glm::ivec2 resolution = window->GetResolution();

    // render board -- should remain static

    float offset = 0.005f * resolution.y;

	// render stats bar
    float barWidth= 0.19f * resolution.x - offset;
	float barHeight = resolution.y - 2 * offset;

    // TODO: fix the scale: the scale should happen from right to left from the bottom right corner of the status bar
    // currently: status bar does not scale proportionally
    modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(barWidth + offset, offset); // move pivot to bottom right
    //modelMatrix *= transform2D::Translate(offset, offset);
    modelMatrix *= transform2D::Scale(barWidth, barHeight);
    modelMatrix *= transform2D::Translate(-1, 0);   // move pivot back (original squareSide = 1, NOT barWidth) -- doesnt do anything rn
    RenderMesh2D(meshes["statsBar"], shaders["VertexColor"], modelMatrix);

    // render player hand
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(translateX, offset);
    modelMatrix *= transform2D::Scale(scaleX, 2 * scaleY);
    RenderMesh2D(meshes["playerHand"], shaders["VertexColor"], modelMatrix);

    // render player lanes
    for (int i = 0; i < 3; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(translateX, translateY + i * (scaleY + offset));
        modelMatrix *= transform2D::Scale(scaleX, scaleY);
        RenderMesh2D(meshes["playerSquare"], shaders["VertexColor"], modelMatrix);
    }

	// render enemy lanes
    for (int i = 3; i < 6; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(translateX, translateY + i * (scaleY + offset));
        modelMatrix *= transform2D::Scale(scaleX, scaleY);
        RenderMesh2D(meshes["enemySquare"], shaders["VertexColor"], modelMatrix);
    }

    //modelMatrix = glm::mat3(1);

    //modelMatrix *= transform2D::Translate(translateX, translateY);
    //modelMatrix *= transform2D::Scale(scaleX, scaleY);

    //RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);
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
        // keeps aspect ratio 16:9 so i dont have Wide Foltest Walking
        isResizing = true;

        if (width != lastWidth) {
			lastWidth = width;
            window->SetSize(width, 9 * width / 16);
        }
        else
			if (height != lastHeight) {
                lastHeight = height;
				window->SetSize(16 * height / 9, height);
			}

        isResizing = false;
    }
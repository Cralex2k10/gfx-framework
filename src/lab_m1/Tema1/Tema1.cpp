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
    playerTurn = true;

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 1;

    // TODO(student): Compute coordinates of a square's center, and store
    // then in the `cx` and `cy` class variables (see the header). Use
    // `corner` and `squareSide`. These two class variables will be used
    // in the `Update()` function. Think about it, why do you need them?

    cx = cardSize / 2;
    cy = cardSize / 2;

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

    // check if mouse is dragging ANY card currently
    isDragging = false;

    // Northern Realms Deck
    // DEVNOTE: Try to keep very few healers for balancing reasons
    // total: 22 cards  -- TODO: later add x3 Decoy as DECOY type and x2 Commander's Horn as HORN (?) type
    // total strength = 149
    cardSize = 50;
    playerDeck = {
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

    // init the hand the player gets
    playerHand = vector<Card*>();
    DrawHand(); // stored to playerHand
    for (int i = 0; i < handSize; i++)

    // card drawer -- TODO: add shapes
    for (int i = 0; i < playerHand.size(); i++) {
        Mesh* tempCard;

        switch (playerHand[i]->action) {
            case ATK: tempCard = object2D::CreateSquare(playerHand[i]->name, corner, cardSize, glm::vec3(1, 0, 0), true);
            case DEF: tempCard = object2D::CreateSquare(playerHand[i]->name, corner, cardSize, glm::vec3(0, 1, 0), true);
            case HEAL: tempCard = object2D::CreateSquare(playerHand[i]->name, corner, cardSize, glm::vec3(0, 0, 1), true);
        }
        AddMeshToList(tempCard);
    }
    

    // player lanes
    Mesh* playerSquare = object2D::CreateSquare("playerSquare", corner, squareSide, glm::vec3(0.5882f, 0.8078f, 1), true);  // light-blue
    AddMeshToList(playerSquare);

    // TODO: Make enemy hand that doesn't render but plays
	Mesh* playerHandLane = object2D::CreateSquare("playerHand", corner, squareSide, glm::vec3(0.32f, 0.24f, 0.16f), true);
    AddMeshToList(playerHandLane);

    Mesh* statsBar = object2D::CreateSquare("statsBar", corner, squareSide, glm::vec3(0.32f, 0.24f, 0.16f), true);
    AddMeshToList(statsBar);

    playerLanes = vector<Lane*>();

	Lane* Hand = new Lane("Hand", true, &playerTurn, playerSquare);

	Lane* melee1 = new Lane("melee", true, &playerTurn, playerSquare);
    Lane* range1 = new Lane("ranged", true, &playerTurn, playerSquare);
    Lane* siege1 = new Lane("siege", true, &playerTurn, playerSquare);

    // siege1, range1, melee1 are pushed from bottom to top render-wise -- ex. siege1 is lowest so its first in playerLanes
    playerLanes.push_back(siege1);
    playerLanes.push_back(range1);
    playerLanes.push_back(melee1);
    playerLanes.push_back(Hand);    // hand lane is tied to id 3

	// enemy lanes
	Mesh* enemySquare = object2D::CreateSquare("enemySquare", corner, squareSide, glm::vec3(1, 0.4902f, 0.4902f), true);
	AddMeshToList(enemySquare);

    enemyLanes = vector<Lane*>();

	Lane* melee2 = new Lane("melee", false, &playerTurn, enemySquare);
	Lane* range2 = new Lane("ranged", false, &playerTurn, enemySquare);
	Lane* siege2 = new Lane("siege", false, &playerTurn, enemySquare);

    playerLanes.push_back(melee2);
    playerLanes.push_back(range2);
    playerLanes.push_back(siege2);
 }

 // method to get 10 random cards from deck
void Tema1::DrawHand() {
    for (int i = 0; i < handSize; i++) {
        int id;
        do    // draw until you get card that was not drawn yet
            id = rand() % playerDeck.size();
        while (playerDeck[id]->wasDrawn);

        playerDeck[id]->wasDrawn = true;
        playerHand.push_back(playerDeck[id]);
    }
}

bool Tema1::getPlayerTurn() {
    return playerTurn;
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

    // render cards
    modelMatrix = glm::mat3(1);
    
    modelMatrix *= transform2D::Translate(playerHand[0]->pos.x, playerHand[0]->pos.y);

    modelMatrix *= transform2D::Translate(cx, cy);
    modelMatrix *= transform2D::Scale(1, 1.5f);
    modelMatrix *= transform2D::Translate(-cx, -cy);

    RenderMesh2D(meshes[playerHand[0]->name], shaders["VertexColor"], modelMatrix);

    // render board
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
        
        playerLanes[i]->setPos(translateX, translateY + i * (scaleY + offset));
        modelMatrix *= transform2D::Translate(translateX, translateY + i * (scaleY + offset));

        playerLanes[i]->setSize(scaleX, scaleY);
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
        glm::ivec2 resolution = window->GetResolution();
        mouseY = resolution.y - mouseY; // invert mouseY for correct compute

        for (int i = 0; i < playerHand.size(); i++) {
            if (playerHand[i]->isDragged) {
                playerHand[i]->pos.x = mouseX - cardSize / 2.0f;
                playerHand[i]->pos.y = mouseY - cardSize / 2.0f;
            }
        }
    }


    void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
    {
        //printf("Pressed %d\n", button);
        // Add mouse button press event
        glm::ivec2 resolution = window->GetResolution();
        mouseY = resolution.y - mouseY; // invert mouseY for correct compute

        if (button == GLFW_MOUSE_BUTTON_2) {
            if (!isDragging) {
                cout << "mouseX = " << mouseX << "; mouseY = " << mouseY << "\n";
                for (int i = 0; i < playerHand.size(); i++) {
                    if ((mouseX >= playerHand[i]->pos.x && mouseX <= playerHand[i]->pos.x + cardSize
                        && mouseY >= playerHand[i]->pos.y && mouseY <= playerHand[i]->pos.y + cardSize
                        || playerHand[i]->isDragged) && !playerHand[i]->used) {
                        cout << "Moving " << playerHand[i]->name << "\n";
                        //playerDeck[i]->pos.x = mouseX;
                        //playerDeck[i]->pos.y = mouseY;ea
                        playerHand[i]->isSelected = true;   // TODO remember to deselect at some point
                        playerHand[i]->isDragged = true;    // TODO remember to turn false when btn release

                        // game state variables
                        //cout << "setting isDragging to true\n";
                        isDragging = true;
                        draggedCard = playerHand[i];    // card that is going to be dragged
                        cout << "dragging " << playerHand[i]->name << " of type " << playerHand[i]->type << "\n";

                        playerHand[i]->initPos = playerHand[i]->pos;

                        // DEVNOTE TODO: consider removing break once cards don't overlap
                        break;
                    }

                    if (!playerHand[i]->isDragged) {
                        playerHand[i]->isSelected = false;  // deselect if i drag a different card
                    }
                }
            }
            else { // if is dragging and click again
                cout << "checking lanes...\n";
                bool foundValidLane = false;
                for (int i = 0; i < 3; i++) {
                    // TODO lane check
                    if (playerLanes[i]->isInLane(mouseX, mouseY) && playerLanes[i]->isValid(draggedCard)) {
                        cout << "adding card " << draggedCard->name << " to lane " << playerLanes[i]->type << "\n";
                        foundValidLane = true;

                        cout << "center of lane = " << playerLanes[i]->getCenter() << "\n";
                        draggedCard->pos = playerLanes[i]->getCenter(); // place card on center
                        playerLanes[i]->addCard(draggedCard);
                        draggedCard->used = true;

                        break;
                    }
                }
                if (!foundValidLane) {  // reset position, no new valid position found
                    cout << "INVALID POSITION! reseting card position\n";

                    draggedCard->pos = draggedCard->initPos;
                }
                
                // stop dragging regardless of outcome
                draggedCard->isSelected = false;  // I'd prever if it remained selected on invalid input
                draggedCard->isDragged = false;

                // a toggle kind of click, does not need code for release
                isDragging = false;
            }
        }

        if (button == GLFW_MOUSE_BUTTON_3) {    // on right click
            cout << "Stopped dragging\n";
            for (int i = 0; i < playerHand.size(); i++) {
                playerHand[i]->isSelected = false;  // deselect everything on right click
                playerHand[i]->isDragged = false;
                // a toggle kind of click, does not need code for release
                isDragging = false;

                playerHand[i]->pos = playerHand[i]->initPos;
            }
        }
    }


    void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
    {
        glm::ivec2 resolution = window->GetResolution();
        mouseY = resolution.y - mouseY; // invert mouseY for correct compute

        // Add mouse button release event
        //if (button = GLFW_MOUSE_BUTTON_2) { // on left click
        //    for (int i = 0; i < playerHand.size(); i++) {
        //        if (playerHand[i]->isDragged == true) {
        //            // update the last frame position   -- TODO make it pick the closest lane
        //            playerHand[i]->pos.x = mouseX;
        //            playerHand[i]->pos.y = mouseY;
        //            playerHand[i]->isDragged = false;
        //            isDragging = false;
        //        }
        //    }
        //}
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
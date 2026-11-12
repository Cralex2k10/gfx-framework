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
    delete player, enemy;
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

    cardSize = 50;

    cx = cardSize / 2;
    cy = cardSize / 2;

    // Initialize tx and ty (the translation steps)
    translateX = resolution.x * 0.195f;
    translateY = resolution.y * 0.25f;
    offset = 0.005f * resolution.y; // offset between lanes

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
    vector<Card*> northernRealms = {
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

    playerDeck = northernRealms;    // might be able to remove

    // TODO: as-is im pretty sure both players compute the same instance of the deck, i want each player with a different deck
    player = new Player(true, northernRealms);
    enemy = new Player(false, northernRealms);

    // player lanes
    Mesh* playerSquare = object2D::CreateSquare("playerSquare", corner, squareSide, glm::vec3(6 / 255.f, 115 / 255.f, 145 / 255.f), true);  // cyan
    AddMeshToList(playerSquare);

    // TODO: Make enemy hand that doesn't render but plays
	Mesh* playerHandLane = object2D::CreateSquare("playerHand", corner, squareSide, glm::vec3(0.32f, 0.24f, 0.16f), true);
    AddMeshToList(playerHandLane);

    // TODO: Replace statsBar with a full darker brown background 
    // -- player hp, round, turn and maybe weather will just be placed in the darker brown area to the left of the table
    Mesh* statsBar = object2D::CreateSquare("statsBar", corner, squareSide, glm::vec3(0.32f, 0.24f, 0.16f), true);
    AddMeshToList(statsBar);

    playerLanes = vector<Lane*>();

    // init hand, set size and pos and drawn cards in hand
	Hand = new Lane("Hand", true, &playerTurn, playerHandLane);
    Hand->setPos(translateX, offset);
    Hand->setSize(scaleX, 2 * scaleY);

    // init the hand the player gets
    playerHand = vector<Card*>();
    DrawCards(player, handSize); // stored to playerHand
    DrawCards(enemy, handSize);

    Hand->printCards();
    //for (int i = 0; i < handSize; i++)

    // card drawer -- TODO: add shapes
    for (int i = 0; i < player->hand.size(); i++) {
        Mesh* tempCard;

        switch (player->hand[i]->action) {                                    // i still dont really get why i need 3 coords to create a 2D square -- glm::vec3(playerHand[i]->pos, 0)
        case ATK: tempCard = object2D::CreateSquare(player->hand[i]->name, corner, cardSize, glm::vec3(163 / 255.f, 0, 0), true); break;  // red
        case DEF: tempCard = object2D::CreateSquare(player->hand[i]->name, corner, cardSize, glm::vec3(10 / 255.f, 125 / 255.f, 41 / 255.f), true); break;    // green
        case HEAL: tempCard = object2D::CreateSquare(player->hand[i]->name, corner, cardSize, glm::vec3(0 / 255.f, 170 / 255.f, 217 / 255.f), true); break;   // blue
        }
        AddMeshToList(tempCard);
    }

    for (int i = 0; i < enemy->hand.size(); i++) {
        Mesh* tempCard;

        switch (enemy->hand[i]->action) {                                    // i still dont really get why i need 3 coords to create a 2D square -- glm::vec3(playerHand[i]->pos, 0)
        case ATK: tempCard = object2D::CreateSquare(enemy->hand[i]->name, corner, cardSize, glm::vec3(163 / 255.f, 0, 0), true); break;  // red
        case DEF: tempCard = object2D::CreateSquare(enemy->hand[i]->name, corner, cardSize, glm::vec3(10 / 255.f, 125 / 255.f, 41 / 255.f), true); break;    // green
        case HEAL: tempCard = object2D::CreateSquare(enemy->hand[i]->name, corner, cardSize, glm::vec3(0 / 255.f, 170 / 255.f, 217 / 255.f), true); break;   // blue
        }
        AddMeshToList(tempCard);
    }

    Mesh* cardOutline = object2D::CreateSquare("cardOutline", corner, cardSize, glm::vec3(36 / 255.f, 40 / 255.f, 41 / 255.f), false); // dark gray
    AddMeshToList(cardOutline);

	Lane* melee1 = new Lane("melee", true, &playerTurn, playerSquare);
    Lane* range1 = new Lane("ranged", true, &playerTurn, playerSquare);
    Lane* siege1 = new Lane("siege", true, &playerTurn, playerSquare);

    // siege1, range1, melee1 are pushed from bottom to top render-wise -- ex. siege1 is lowest so its first in playerLanes
    playerLanes.push_back(siege1);
    playerLanes.push_back(range1);
    playerLanes.push_back(melee1);
    //playerLanes.push_back(Hand);    // hand lane is tied to id 3

	// enemy lanes
	Mesh* enemySquare = object2D::CreateSquare("enemySquare", corner, squareSide, glm::vec3(112 / 255.f, 13 / 255.f, 2 / 255.f), true);
	AddMeshToList(enemySquare);

    enemyLanes = vector<Lane*>();

	Lane* melee2 = new Lane("melee", false, &playerTurn, enemySquare);
	Lane* range2 = new Lane("ranged", false, &playerTurn, enemySquare);
	Lane* siege2 = new Lane("siege", false, &playerTurn, enemySquare);

    enemyLanes.push_back(melee2);
    enemyLanes.push_back(range2);
    enemyLanes.push_back(siege2);

    // initialize positions and sizes for each player lane
    for (int i = 0; i < playerLanes.size(); i++) {
        playerLanes[i]->setPos(translateX, translateY + i * (scaleY + offset));
        playerLanes[i]->setSize(scaleX, scaleY);
    }

    // initialize positions and sizes for each enemy lane
    for (int i = 0; i < enemyLanes.size(); i++) {
        enemyLanes[i]->setPos(translateX, translateY + (i + playerLanes.size()) * (scaleY + offset));   // for drawing purposes
        enemyLanes[i]->setSize(scaleX, scaleY);
    }
 }

 // method to get 10 random cards from deck
void Tema1::DrawCards(Player* p, int count) {
    //Hand->getSize();
    for (int i = 0; i < count; i++) {
        int id;
        do    // draw until you get card that was not drawn yet
            id = rand() % p->deck.size();
        while (p->deck[id]->wasDrawn);

        p->deck[id]->wasDrawn = true;    // drawn meaning picked
        p->hand.push_back(p->deck[id]);
        tableCards.push_back(p->deck[id]);   // get let the table have an evidence of all the cards in game

        if (p->isPlayer == true)
            Hand->addCard(p->deck[id]);
        //cout << playerDeck[id]->name << "is at coords [" << playerDeck[id]->pos.x << ", " << playerDeck[id]->pos.y << "]\n";
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

	// render stats bar
    float barWidth= 0.19f * resolution.x - offset;
	float barHeight = resolution.y - 2 * offset;

    //for (int i = 0; i < tableCards.size(); i++) {
    //    modelMatrix = glm::mat3(1);

    //    modelMatrix *= transform2D::Translate(tableCards[i]->pos.x, tableCards[i]->pos.y);

    //    modelMatrix *= transform2D::Translate(cx, cy);
    //    modelMatrix *= transform2D::Scale(1, 1.5f);
    //    modelMatrix *= transform2D::Translate(-cx, -cy);

    //    // make outline a little thicker
    //    glLineWidth(5.0f);
    //    RenderMesh2D(meshes["cardOutline"], shaders["VertexColor"], modelMatrix);
    //    glLineWidth(1.0f);

    //    RenderMesh2D(meshes[tableCards[i]->name], shaders["VertexColor"], modelMatrix);

    //    //modelMatrix *= transform2D::Translate(-0.05f * cardSize, -0.05f * cardSize);
    //}

    // render cards in Hand
    for (int i = 0; i < player->hand.size(); i++) {
        modelMatrix = glm::mat3(1);

        modelMatrix *= transform2D::Translate(player->hand[i]->pos.x, player->hand[i]->pos.y);

        modelMatrix *= transform2D::Translate(cx, cy);
        modelMatrix *= transform2D::Scale(1, 1.5f);
        modelMatrix *= transform2D::Translate(-cx, -cy);

        // make outline a little thicker
        glLineWidth(5.0f);
        RenderMesh2D(meshes["cardOutline"], shaders["VertexColor"], modelMatrix);
        glLineWidth(1.0f);

        RenderMesh2D(meshes[player->hand[i]->name], shaders["VertexColor"], modelMatrix);
    }

    // render cards on board lanes
    for (int l = 0; l < playerLanes.size(); l++) {
        for (int i = 0; i < playerLanes[l]->cards.size(); i++) {
            modelMatrix = glm::mat3(1);

            modelMatrix *= transform2D::Translate(playerLanes[l]->cards[i]->pos.x, playerLanes[l]->cards[i]->pos.y);

            modelMatrix *= transform2D::Translate(cx, cy);
            modelMatrix *= transform2D::Scale(1, 1.5f);
            modelMatrix *= transform2D::Translate(-cx, -cy);

            // make outline a little thicker
            glLineWidth(5.0f);
            RenderMesh2D(meshes["cardOutline"], shaders["VertexColor"], modelMatrix);
            glLineWidth(1.0f);

            RenderMesh2D(meshes[playerLanes[l]->cards[i]->name], shaders["VertexColor"], modelMatrix);
        }
    }

    // render board
    modelMatrix = glm::mat3(1);
	modelMatrix *= transform2D::Translate(barWidth + offset, offset); // move pivot to bottom right
    //modelMatrix *= transform2D::Translate(offset, offset);
    modelMatrix *= transform2D::Scale(barWidth, barHeight);
    modelMatrix *= transform2D::Translate(-1, 0);   // move pivot back (original squareSide = 1, NOT barWidth) -- doesnt do anything rn
    RenderMesh2D(meshes["statsBar"], shaders["VertexColor"], modelMatrix);

    // render player hand lane
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(Hand->pos.x, Hand->pos.y);
    modelMatrix *= transform2D::Scale(Hand->width, Hand->height);

    RenderMesh2D(meshes["playerHand"], shaders["VertexColor"], modelMatrix);

    // render player lanes
    for (int i = 0; i < 3; i++) {
        modelMatrix = glm::mat3(1);
        
        //playerLanes[i]->setPos(translateX, translateY + i * (scaleY + offset));
        modelMatrix *= transform2D::Translate(playerLanes[i]->pos.x, playerLanes[i]->pos.y);
        modelMatrix *= transform2D::Scale(playerLanes[i]->width, playerLanes[i]->height);

        RenderMesh2D(meshes["playerSquare"], shaders["VertexColor"], modelMatrix);
    }

	// render enemy lanes
    for (int i = 0; i < 3; i++) {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(enemyLanes[i]->pos.x, enemyLanes[i]->pos.y);
        modelMatrix *= transform2D::Scale(enemyLanes[i]->width, enemyLanes[i]->height);
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
        if (playerTurn == false) {
            cout << "It's not your turn yet!\n";
            return;
        }

        // Add key press event
        if (key == GLFW_KEY_ENTER)
        {
            playerTurn = !playerTurn;
            cout << "\nplayerTurn is now " << playerTurn << "\n";
        }
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

        for (int i = 0; i < player->hand.size(); i++) {
            if (player->hand[i]->isDragged) {
                player->hand[i]->pos.x = mouseX - cardSize / 2.0f;
                player->hand[i]->pos.y = mouseY - cardSize / 2.0f;
            }
        }
    }


    void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
    {
        if (playerTurn == false) {
            cout << "It's not your turn yet!\n";
            return;
        }

        //printf("Pressed %d\n", button);
        // Add mouse button press event
        glm::ivec2 resolution = window->GetResolution();
        mouseY = resolution.y - mouseY; // invert mouseY for correct compute

        if (button == GLFW_MOUSE_BUTTON_2) {
            if (!isDragging) {
                cout << "mouseX = " << mouseX << "; mouseY = " << mouseY << "\n";
                // dragging card from hand
                for (int i = 0; i < player->hand.size(); i++) {
                    if ((mouseX >= player->hand[i]->pos.x && mouseX <= player->hand[i]->pos.x + cardSize
                        && mouseY >= player->hand[i]->pos.y && mouseY <= player->hand[i]->pos.y + cardSize
                        || player->hand[i]->isDragged) && !player->hand[i]->used) {

                        cout << "Moving " << player->hand[i]->name << "\n";

                        player->hand[i]->isSelected = true;
                        player->hand[i]->isDragged = true;    // TODO remember to turn false when btn release

                        // game state variables
                        //cout << "setting isDragging to true\n";
                        isDragging = true;
                        draggedCard = player->hand[i];    // card that is going to be dragged
                        cout << "dragging " << player->hand[i]->name << " of type " << player->hand[i]->type << "\n";

                        player->hand[i]->initPos = player->hand[i]->pos;

                        // DEVNOTE TODO: consider removing break once cards don't overlap
                        break;
                    }

                    if (!player->hand[i]->isDragged) {
                        player->hand[i]->isSelected = false;  // deselect if i drag a different card
                    }
                }

                // undo move - bring card back to hand
                for (int l = 0; l < playerLanes.size(); l++) {
                    for (int i = 0; i < playerLanes[l]->cards.size(); i++) {
                        if ((mouseX >= playerLanes[l]->cards[i]->pos.x && mouseX <= playerLanes[l]->cards[i]->pos.x + cardSize
                            && mouseY >= playerLanes[l]->cards[i]->pos.y && mouseY <= playerLanes[l]->cards[i]->pos.y + cardSize
                            && !playerLanes[l]->cards[i]->isDragged) && playerLanes[l]->cards[i]->used) {

                            cout << "Moving " << player->hand[i]->name << "back to hand\n";
                            
                            playerLanes[l]->cards[i]->used = false;
                            Hand->addCard(playerLanes[l]->cards[i]);
                            player->hand = Hand->cards;

                            playerLanes[l]->removeCard(playerLanes[l]->cards[i]);
                            
                            break;
                        }
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
                        Hand->removeCard(draggedCard);

                        // update player hand after removing draggedCard -- remove if you end up going with Lane enemy hand approach
                        player->hand = Hand->cards;

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

            draggedCard->isSelected = false;
            draggedCard->isDragged = false;
            isDragging = false;
            draggedCard->pos = draggedCard->initPos;
        }
    }


    void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
    {
        glm::ivec2 resolution = window->GetResolution();
        mouseY = resolution.y - mouseY; // invert mouseY for correct compute

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
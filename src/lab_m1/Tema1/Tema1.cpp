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

// Deck generator -- gets a deck from a bunch of predefined decks (i only have one deck now)
// DEVNOTE: Try to keep very few healers for balancing reasons
// total: 28 cards  -- TODO: later add x3 Decoy as DECOY type and x2 Commander's Horn as HORN (?) type
// total strength = 149 - 4
vector<Card*> Tema1::CreateDeck(const string& name) {
    if (name == "northernRealms") {
        return {
            new Card("Geralt of Rivia", ATK, MELEE, 10),
            new Card("Ciri", HEAL, MELEE, 7),   // 15
            new Card("Yennefer of Vengerberg", ATK, RANGED, 7),
            new Card("Triss Merigold", DEF, RANGED, 3),     // 7
            new Card("Philippa Eilhart", ATK, RANGED, 6),
            new Card("Vernon Roche", ATK, MELEE, 8),
            new Card("John Natalis", DEF, MELEE, 3),        // 10
            new Card("Esterad Thyssen", ATK, MELEE, 10),
            new Card("Myserious ELF", HEAL, MELEE, 4), // TODO: turn into spy later // 5
            new Card("Villentretenmerth", ATK, MELEE, 7),
            new Card("Catapult", ATK, SIEGE, 8),
            new Card("Catapult", ATK, SIEGE, 8),
            new Card("Prince Stennis", DEF, MELEE, 3), // 5 // TODO: turn into spy later
            new Card("Crinfrid Reavers Dragon Hunter", DEF, RANGED, 3), // 5 // TODO: implement doubling score mechanic for doubles
            new Card("Crinfrid Reavers Dragon Hunter", DEF, RANGED, 3), // 5
            new Card("Crinfrid Reavers Dragon Hunter", DEF, RANGED, 3), // 5
            new Card("Dun Banner Medic", HEAL, SIEGE, 2),       // 5
            new Card("Sigismund Dijkstra", ATK, MELEE, 6),
            new Card("Blue Stripes Commando", ATK, MELEE, 4),
            new Card("Blue Stripes Commando", ATK, MELEE, 4),
            new Card("Blue Stripes Commando", ATK, MELEE, 4),
            new Card("Thaler", ATK, SIEGE, 1),   // TODO: turn into spy later
            new Card("Clear Weather", CLEAR, ANY, 0),
            new Card("Clear Weather", CLEAR, ANY, 0),
            new Card("Biting Frost", FROST, MELEE, -1),
            new Card("Biting Frost", FROST, MELEE, -1),
            new Card("Impenetrable Fog", FROST, RANGED, -1),
            new Card("Torrential Rain", FROST, SIEGE, -1)
        };
    }

    // shouldn't ever happen 
    cout << "\nWrong deck name\n";
    return vector<Card*>();
}

void Tema1::Init()
{
	glm::ivec2 resolution = window->GetResolution();    // get resolution of the window
    baseRes = glm::vec2(resolution);

    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)baseRes.x, 0, (float)baseRes.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 50));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    // player always starts first for convenience
    // TODO: make first turn random after implementing opponent AI
    playerTurn = true;
    turnsPlayed = 0;

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
    gameOngoing = true;

    //playerDeck = northernRealms;    // might be able to remove

    // TODO: as-is im pretty sure both players compute the same instance of the deck, i want each player with a different deck
    player = new Player(true, CreateDeck("northernRealms"));
    enemy = new Player(false, CreateDeck("northernRealms"));

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

    Mesh* fullHpTile = object2D::CreateSquare("fullHpTile", corner, squareSide, glm::vec3(0, 1, 0), true);
    AddMeshToList(fullHpTile);
        
    Mesh* emptyHpTile = object2D::CreateSquare("emptyHpTile", corner, squareSide, glm::vec3(77 / 255.f, 89 / 255.f, 92 / 255.f), true); // light-ish gray
    AddMeshToList(emptyHpTile);

    Mesh* playerWonRoundUI = object2D::CreateShield("playerWonRoundUI", corner, squareSide, glm::vec3(10 / 255.f, 152 / 255.f, 191 / 255.f), true); // lighter cyan
    AddMeshToList(playerWonRoundUI);

    Mesh* emptyWonRoundUI = object2D::CreateShield("emptyWonRoundUI", corner, squareSide, glm::vec3(77 / 255.f, 89 / 255.f, 92 / 255.f), true); // light-ish gray
    AddMeshToList(emptyWonRoundUI);

    playerLanes = vector<Lane*>();

    Mesh* melee = object2D::CreateTriangle("melee", corner, squareSide, glm::vec3(230 / 255.f, 214 / 255.f, 41 / 255.f), true); // Yellow
    AddMeshToList(melee);

    Mesh* ranged = object2D::CreateSquare("ranged", corner, squareSide, glm::vec3(230 / 255.f, 214 / 255.f, 41 / 255.f), true); // Yellow
    AddMeshToList(ranged);

    Mesh* siege = object2D::CreatePentagon("siege", corner, squareSide, glm::vec3(230 / 255.f, 214 / 255.f, 41 / 255.f), true); // Yellow
    AddMeshToList(siege);
    
    // for clear weather
    Mesh* any = object2D::CreateCircle("any", corner, squareSide, glm::vec3(230 / 255.f, 214 / 255.f, 41 / 255.f), true); // Yellow
    AddMeshToList(any);

    // init hand, set size and pos and drawn cards in hand
	Hand = new Lane("Hand", true, &playerTurn, playerHandLane);
    Hand->setPos(translateX, offset);
    Hand->setSize(scaleX, 2 * scaleY);

    // init the hand the player gets
    playerHand = vector<Card*>();
    DrawCards(player, handSize); // stored to playerHand
    DrawCards(enemy, handSize);

    cout << "\nPlayer cards:\n";
    Hand->printCards();

    cout << "\nEnemy cards:\n";
    for (Card* card : enemy->hand) {
        cout << card->name << "\n";
    }

    // card drawer -- TODO: add shapes
    for (int i = 0; i < player->hand.size(); i++) {
        Mesh* tempCard;

        switch (player->hand[i]->action) {                                    // i still dont really get why i need 3 coords to create a 2D square -- glm::vec3(playerHand[i]->pos, 0)
            case ATK: tempCard = object2D::CreateSquare(player->hand[i]->name, corner, cardSize, glm::vec3(163 / 255.f, 0, 0), true); break;  // red
            case DEF: tempCard = object2D::CreateSquare(player->hand[i]->name, corner, cardSize, glm::vec3(0 / 255.f, 170 / 255.f, 217 / 255.f), true); break;  // blue
            case HEAL: tempCard = object2D::CreateSquare(player->hand[i]->name, corner, cardSize, glm::vec3(10 / 255.f, 125 / 255.f, 41 / 255.f), true); break; // green
            case CLEAR: tempCard = object2D::CreateSquare(player->hand[i]->name, corner, cardSize, glm::vec3(184 / 255.f, 142 / 255.f, 29 / 255.f), true); break;    // golden
            case FROST: tempCard = object2D::CreateSquare(player->hand[i]->name, corner, cardSize, glm::vec3(1, 1, 1), true); break;    // white
        }
        AddMeshToList(tempCard);
    }

    for (int i = 0; i < enemy->hand.size(); i++) {
        Mesh* tempCard;

        switch (enemy->hand[i]->action) {                                    // i still dont really get why i need 3 coords to create a 2D square -- glm::vec3(playerHand[i]->pos, 0)
            case ATK: tempCard = object2D::CreateSquare(enemy->hand[i]->name, corner, cardSize, glm::vec3(163 / 255.f, 0, 0), true); break;  // red
            case DEF: tempCard = object2D::CreateSquare(enemy->hand[i]->name, corner, cardSize, glm::vec3(0 / 255.f, 170 / 255.f, 217 / 255.f), true); break;    // blue
            case HEAL: tempCard = object2D::CreateSquare(enemy->hand[i]->name, corner, cardSize, glm::vec3(10 / 255.f, 125 / 255.f, 41 / 255.f), true); break;   // green
            case CLEAR: tempCard = object2D::CreateSquare(enemy->hand[i]->name, corner, cardSize, glm::vec3(184 / 255.f, 142 / 255.f, 29 / 255.f), true); break;    // golden
            case FROST: tempCard = object2D::CreateSquare(enemy->hand[i]->name, corner, cardSize, glm::vec3(1, 1, 1), true); break;    // white
        }
        AddMeshToList(tempCard);
    }

    Mesh* cardOutline = object2D::CreateSquare("cardOutline", corner, cardSize, glm::vec3(36 / 255.f, 40 / 255.f, 41 / 255.f), false); // dark gray
    AddMeshToList(cardOutline);

    Mesh* laneOutline = object2D::CreateSquare("laneOutline", corner, squareSide, glm::vec3(23 / 255.f, 23 / 255.f, 23 / 255.f), false); // dark gray
    AddMeshToList(laneOutline);

    Mesh* outlineWonRoundUI = object2D::CreateShield("outlineWonRoundUI", corner, squareSide, glm::vec3(23 / 255.f, 23 / 255.f, 23 / 255.f), false); // light-ish gray
    AddMeshToList(outlineWonRoundUI);

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

    Mesh* enemyWonRoundUI = object2D::CreateShield("enemyWonRoundUI", corner, squareSide, glm::vec3(112 / 255.f, 13 / 255.f, 2 / 255.f), true);
    AddMeshToList(enemyWonRoundUI);

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
    int drawnAtk = 0;
    for (int i = 0; i < count; i++) {
        int id;
        bool needAtk = (i < 5);

        do {  // draw until you get card that was not drawn yet
            id = rand() % p->deck.size();
        }
        while (p->deck[id]->wasDrawn || (needAtk && p->deck[id]->action != ATK));

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



    // enemy ai move logic
    if (playerTurn == false) {
        while (enemy->cardsPlayed == 0)
            enemy->cardsPlayed = 1; //rand() % maxCardsPlayed; // replace with this for custom card count per turn

        for (int i = 0; i < enemy->cardsPlayed; i++) {
            
            // compute final scores and announce winner 
            // -- or skip turn entirely until player drains all enemy hp or also runs out of cards
            if (enemy->hand.size() == 0)
                break;

            int id = rand() % enemy->hand.size();

            // place randomly selected card in correct lane


            if (enemy->hand[id]->action == CLEAR) {
                cout << "\nEnemy played " << enemy->hand[id]->name << "\n";

                for (Lane* lane : enemyLanes) {
                    for (Card* card : lane->cards) {
                        if (card->action == CLEAR || card->action == FROST) {
                            lane->removeCard(card);
                        }
                    }
                }

                for (Lane* lane : playerLanes) {
                    for (Card* card : lane->cards) {
                        if (card->action == CLEAR || card->action == FROST) {
                            lane->removeCard(card);
                        }
                    }
                }
            }
            else {
                // special case for FROST and SPY if i have the time
                if (enemy->hand[id]->action == FROST) {
                    for (Lane* lane : playerLanes) {
                        if (lane->type == enemy->hand[id]->type) {
                            lane->addCard(enemy->hand[id]);
                            cout << "\nEnemy played " << enemy->hand[id]->name << "\n";

                            // remove card from hand
                            auto it = std::find(enemy->hand.begin(), enemy->hand.end(), enemy->hand[id]);
                            if (it != enemy->hand.end()) {
                                enemy->hand.erase(it);
                            }

                            break;
                        }
                    }
                }
                else {  // case for other cards
                    for (Lane* lane : enemyLanes) {
                        if (lane->type == enemy->hand[id]->type) {
                            lane->addCard(enemy->hand[id]);
                            cout << "\nEnemy played " << enemy->hand[id]->name << "\n";

                            // remove card from hand
                            auto it = std::find(enemy->hand.begin(), enemy->hand.end(), enemy->hand[id]);
                            if (it != enemy->hand.end()) {
                                enemy->hand.erase(it);
                            }

                            break;
                        }
                    }
                }
            }

        }

        enemy->roundScore = glm::vec3(0);
        for (Lane* lane : enemyLanes) {
            enemy->roundScore += lane->computeScores();
        }

        enemy->printScores();

        playerTurn = !playerTurn;

        turnsPlayed = (turnsPlayed + 1) % 2;
        
        checkRoundEnd();

        if (player->roundsWon == 2 || enemy->roundsWon == 2 || !gameOngoing) {
            // one last call to update roundsWonUI

            auto camera = GetSceneCamera();
            camera->SetOrthographic(0, (float)baseRes.x, 0, (float)baseRes.y, 0.01f, 400);
            camera->SetPosition(glm::vec3(0, 0, 50));
            camera->SetRotation(glm::vec3(0, 0, 0));
            camera->Update();
        }
        else
            cout << "\nNow it's your turn! Place a card or three!\n";
    }

    // render cards in Hand
    for (int i = 0; i < player->hand.size(); i++) {
        {
            // draw icon
            modelMatrix = glm::mat3(1);

            modelMatrix *= transform2D::Translate(player->hand[i]->pos.x, player->hand[i]->pos.y);

            modelMatrix *= transform2D::Translate(cx / 2 + 1.85f, cy / 2);
            modelMatrix *= transform2D::Scale(laneIconSize, laneIconSize);
            //modelMatrix *= transform2D::Translate(-cx, -cy);

            RenderMesh2D(meshes[player->hand[i]->type], shaders["VertexColor"], modelMatrix);
        }

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

    // render cards on player lanes
    for (int l = 0; l < playerLanes.size(); l++) {
        for (int i = 0; i < playerLanes[l]->cards.size(); i++) {
            {
                // draw icon
                modelMatrix = glm::mat3(1);

                modelMatrix *= transform2D::Translate(playerLanes[l]->cards[i]->pos.x, playerLanes[l]->cards[i]->pos.y);

                modelMatrix *= transform2D::Translate(cx / 2 + 1.85f, cy / 2);
                modelMatrix *= transform2D::Scale(laneIconSize, laneIconSize);
                //modelMatrix *= transform2D::Translate(-cx, -cy);

                RenderMesh2D(meshes[playerLanes[l]->cards[i]->type], shaders["VertexColor"], modelMatrix);
            }

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

    // render cards on enemy lanes
    for (int l = 0; l < enemyLanes.size(); l++) {
        for (int i = 0; i < enemyLanes[l]->cards.size(); i++) {
            {
                // draw icon
                modelMatrix = glm::mat3(1);

                modelMatrix *= transform2D::Translate(enemyLanes[l]->cards[i]->pos.x, enemyLanes[l]->cards[i]->pos.y);

                modelMatrix *= transform2D::Translate(cx / 2 + 1.85f, cy / 2);
                modelMatrix *= transform2D::Scale(laneIconSize, laneIconSize);
                //modelMatrix *= transform2D::Translate(-cx, -cy);

                RenderMesh2D(meshes[enemyLanes[l]->cards[i]->type], shaders["VertexColor"], modelMatrix);
            }

            modelMatrix = glm::mat3(1);

            modelMatrix *= transform2D::Translate(enemyLanes[l]->cards[i]->pos.x, enemyLanes[l]->cards[i]->pos.y);

            modelMatrix *= transform2D::Translate(cx, cy);
            modelMatrix *= transform2D::Scale(1, 1.5f);
            modelMatrix *= transform2D::Translate(-cx, -cy);

            // make outline a little thicker
            glLineWidth(5.0f);
            RenderMesh2D(meshes["cardOutline"], shaders["VertexColor"], modelMatrix);
            glLineWidth(1.0f);

            RenderMesh2D(meshes[enemyLanes[l]->cards[i]->name], shaders["VertexColor"], modelMatrix);
        }
    }

    // render player hand lane
    modelMatrix = glm::mat3(1);
    modelMatrix *= transform2D::Translate(Hand->pos.x, Hand->pos.y);
    modelMatrix *= transform2D::Scale(Hand->width, Hand->height);

    // make outline a little thicker
    glLineWidth(3.0f);
    RenderMesh2D(meshes["laneOutline"], shaders["VertexColor"], modelMatrix);
    glLineWidth(1.0f);

    RenderMesh2D(meshes["playerHand"], shaders["VertexColor"], modelMatrix);

    // render player lanes
    for (int i = 0; i < 3; i++) {
        // draw the lane's icons (type)
        modelMatrix = glm::mat3(1);

        modelMatrix *= transform2D::Translate(playerLanes[i]->pos.x + 20, playerLanes[i]->pos.y + playerLanes[i]->height / 4);
        modelMatrix *= transform2D::Scale(laneIconSize * 2, laneIconSize * 2);
        RenderMesh2D(meshes[playerLanes[i]->type], shaders["VertexColor"], modelMatrix);

        // draw the lane itself
        modelMatrix = glm::mat3(1);
        
        //playerLanes[i]->setPos(translateX, translateY + i * (scaleY + offset));
        modelMatrix *= transform2D::Translate(playerLanes[i]->pos.x, playerLanes[i]->pos.y);
        modelMatrix *= transform2D::Scale(playerLanes[i]->width, playerLanes[i]->height);

        // make outline a little thicker
        glLineWidth(3.0f);
        RenderMesh2D(meshes["laneOutline"], shaders["VertexColor"], modelMatrix);
        glLineWidth(1.0f);

        RenderMesh2D(meshes["playerSquare"], shaders["VertexColor"], modelMatrix);
    }

	// render enemy lanes
    for (int i = 0; i < 3; i++) {
        // draw lane's icon
        modelMatrix = glm::mat3(1);

        modelMatrix *= transform2D::Translate(enemyLanes[i]->pos.x + 20, enemyLanes[i]->pos.y + enemyLanes[i]->height / 4);
        modelMatrix *= transform2D::Scale(laneIconSize * 2, laneIconSize * 2);
        RenderMesh2D(meshes[enemyLanes[i]->type], shaders["VertexColor"], modelMatrix);

        // draw the lane itself
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(enemyLanes[i]->pos.x, enemyLanes[i]->pos.y);
        modelMatrix *= transform2D::Scale(enemyLanes[i]->width, enemyLanes[i]->height);

        // make outline a little thicker
        glLineWidth(3.0f);
        RenderMesh2D(meshes["laneOutline"], shaders["VertexColor"], modelMatrix);
        glLineWidth(1.0f);

        RenderMesh2D(meshes["enemySquare"], shaders["VertexColor"], modelMatrix);
    }


    // render player hp
    int rowCount = rowCount = player->maxHP / 5 + (player->maxHP % 5 == 0 ? 0 : 1);
    {
        int tileCount = 0;

        for (int i = 0; i < player->maxHP / 5; i++) {
            for (int j = 0; j < 5; j++) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(resolution.x / 100 + hpTileWidth * (j + 1), hpTileHeight * i + Hand->height + 20);
                modelMatrix *= transform2D::Scale(hpTileWidth, hpTileHeight);

                // make outline a little thicker
                glLineWidth(2);
                RenderMesh2D(meshes["laneOutline"], shaders["VertexColor"], modelMatrix);
                glLineWidth(1.0f);

                if (tileCount < player->hp)
                    RenderMesh2D(meshes["fullHpTile"], shaders["VertexColor"], modelMatrix);

                tileCount++;
            }
        }

        // render remaining hp tiles if maxHP !/ 5
        for (int k = 0; k < player->maxHP % 5; k++) {
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(resolution.x / 100 + hpTileWidth * (k + 1), player->maxHP / 5 * hpTileHeight + Hand->height + 20);
            modelMatrix *= transform2D::Scale(hpTileWidth, hpTileHeight);

            // make outline a little thicker
            glLineWidth(2);
            RenderMesh2D(meshes["laneOutline"], shaders["VertexColor"], modelMatrix);
            glLineWidth(1.0f);

            if (tileCount < player->hp)
                RenderMesh2D(meshes["fullHpTile"], shaders["VertexColor"], modelMatrix);
            
            tileCount++;
        }
    }

    // render player wonRounds UI
    {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(resolution.x / 100 + 4.85f * hpTileWidth, Hand->height + wonRoundSize + (rowCount - 1) * hpTileHeight);
        //modelMatrix *= transform2D::Translate(100 * (i + 1), 100);
        modelMatrix *= transform2D::Scale(wonRoundSize, wonRoundSize);

        // make outline a little thicker
        glLineWidth(2);
        RenderMesh2D(meshes["outlineWonRoundUI"], shaders["VertexColor"], modelMatrix);
        glLineWidth(1.0f);

        if (player->roundsWon == 2)
            RenderMesh2D(meshes["playerWonRoundUI"], shaders["VertexColor"], modelMatrix);
        else
            RenderMesh2D(meshes["emptyWonRoundUI"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(resolution.x / 100 + 4.85f * hpTileWidth - 5 - wonRoundSize, Hand->height + wonRoundSize + (rowCount - 1) * hpTileHeight);
        //modelMatrix *= transform2D::Translate(100 * (i + 1), 100);
        modelMatrix *= transform2D::Scale(wonRoundSize, wonRoundSize);

        // make outline a little thicker
        glLineWidth(2);
        RenderMesh2D(meshes["outlineWonRoundUI"], shaders["VertexColor"], modelMatrix);
        glLineWidth(1.0f);
        
        if (player->roundsWon >= 1)
            RenderMesh2D(meshes["playerWonRoundUI"], shaders["VertexColor"], modelMatrix);
        else
            RenderMesh2D(meshes["emptyWonRoundUI"], shaders["VertexColor"], modelMatrix);
    }

    // render enemy hp
    {
        int tileCount = 0;

        for (int i = 0; i < player->maxHP / 5; i++) {
            for (int j = 0; j < 5; j++) {
                modelMatrix = glm::mat3(1);
                modelMatrix *= transform2D::Translate(resolution.x / 100 + hpTileWidth * (j + 1), hpTileHeight * i + enemyLanes[0]->pos.y + 20);
                modelMatrix *= transform2D::Scale(hpTileWidth, hpTileHeight);

                // make outline a little thicker
                glLineWidth(2);
                RenderMesh2D(meshes["laneOutline"], shaders["VertexColor"], modelMatrix);
                glLineWidth(1.0f);

                if (tileCount < enemy->hp)
                    RenderMesh2D(meshes["fullHpTile"], shaders["VertexColor"], modelMatrix);
            
                tileCount++;
            }
        }

        // render remaining hp tiles if maxHP !/ 5
        for (int k = 0; k < player->maxHP % 5; k++) {
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(resolution.x / 100 + hpTileWidth * (k + 1), player->maxHP / 5 * hpTileHeight + enemyLanes[0]->pos.y + 20);
            modelMatrix *= transform2D::Scale(hpTileWidth, hpTileHeight);

            // make outline a little thicker
            glLineWidth(2);
            RenderMesh2D(meshes["laneOutline"], shaders["VertexColor"], modelMatrix);
            glLineWidth(1.0f);

            if (tileCount < enemy->hp)
                RenderMesh2D(meshes["fullHpTile"], shaders["VertexColor"], modelMatrix);

            tileCount++;
        }
    }

    // render enemy wonRounds UI
    {
        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(resolution.x / 100 + 4.85f * hpTileWidth, enemyLanes[0]->pos.y + wonRoundSize + (rowCount - 1) * hpTileHeight);
        //modelMatrix *= transform2D::Translate(100 * (i + 1), 100);
        modelMatrix *= transform2D::Scale(wonRoundSize, wonRoundSize);

        // make outline a little thicker
        glLineWidth(2);
        RenderMesh2D(meshes["outlineWonRoundUI"], shaders["VertexColor"], modelMatrix);
        glLineWidth(1.0f);

        if (enemy->roundsWon == 2)
            RenderMesh2D(meshes["enemyWonRoundUI"], shaders["VertexColor"], modelMatrix);
        else
            RenderMesh2D(meshes["emptyWonRoundUI"], shaders["VertexColor"], modelMatrix);

        modelMatrix = glm::mat3(1);
        modelMatrix *= transform2D::Translate(resolution.x / 100 + 4.85f * hpTileWidth - 5 - wonRoundSize, enemyLanes[0]->pos.y + wonRoundSize + (rowCount - 1) * hpTileHeight);
        //modelMatrix *= transform2D::Translate(100 * (i + 1), 100);
        modelMatrix *= transform2D::Scale(wonRoundSize, wonRoundSize);

        // make outline a little thicker
        glLineWidth(2);
        RenderMesh2D(meshes["outlineWonRoundUI"], shaders["VertexColor"], modelMatrix);
        glLineWidth(1.0f);


        if (enemy->roundsWon >= 1)
            RenderMesh2D(meshes["enemyWonRoundUI"], shaders["VertexColor"], modelMatrix);
        else
            RenderMesh2D(meshes["emptyWonRoundUI"], shaders["VertexColor"], modelMatrix);
    }

    // render board
    modelMatrix = glm::mat3(1);
    //modelMatrix *= transform2D::Translate(barWidth + offset, offset); // move pivot to bottom right
    //modelMatrix *= transform2D::Translate(offset, offset);
    modelMatrix *= transform2D::Scale(baseRes.x, baseRes.y);
    //modelMatrix *= transform2D::Translate(-1, 0);   // move pivot back (original squareSide = 1, NOT barWidth) -- doesnt do anything rn

    RenderMesh2D(meshes["statsBar"], shaders["VertexColor"], modelMatrix);

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

void Tema1::checkRoundEnd() {
    // check if 2 turns where played - if so, compare scores
    if (turnsPlayed == 0) {

        // enemy atk
        // decrease enemy damage with player defence
        if (enemy->roundScore.x - player->roundScore.y >= 0)
            enemy->roundScore.x -= player->roundScore.y;
        else
            enemy->roundScore.x = 0;

        // player atk
        // decrease player damage with enemy defence
        if (player->roundScore.x - enemy->roundScore.y >= 0)
            player->roundScore.x -= enemy->roundScore.y;
        else
            player->roundScore.x = 0;


        // enemy def
        if (enemy->hp - player->roundScore.x >= 0)
            enemy->hp -= player->roundScore.x;  // decrease enemy hp with remaining player damage
        else
            enemy->hp = 0;

        // player def
        if (player->hp - enemy->roundScore.x >= 0)
            player->hp -= enemy->roundScore.x;
        else
            player->hp = 0;


        // enemy heal
        if (enemy->hp > 0)
            if (enemy->hp + enemy->roundScore.z <= enemy->maxHP)
                enemy->hp += enemy->roundScore.z;
            else
                enemy->hp = enemy->maxHP;

        // player heal
        if (player->hp > 0)
            if (player->hp + player->roundScore.z <= player->maxHP)
                player->hp += player->roundScore.z;
            else
                player->hp = player->maxHP;

        cout << "\nPlayer HP is now " << player->hp << "\nEnemy HP is now " << enemy->hp << "\n";

        // tie-breaker
        if (player->hp == 0 && enemy->hp == 0) {
            if (enemy->roundScore.x > player->roundScore.x) // enemy wins
                enemy->roundsWon++;
            else
                if (enemy->roundScore.x == player->roundScore.x) { // tie
                    player->roundsWon++;
                    enemy->roundsWon++;
                }
                else
                    player->roundsWon++; // player wins
        }
        else // general cases
            if (enemy->hp == 0)
                player->roundsWon++;
            else
                if (player->hp == 0)
                    enemy->roundsWon++;

        // if out of cards
        if (player->hand.size() == 0) {
            if (enemy->hand.size() > 0) {
                enemy->roundsWon++;
            }   // both ran out of cards
            else {
                if (enemy->hp > player->hp)
                    enemy->roundsWon++;
                else 
                    if (enemy->hp < player->hp)
                        player->roundsWon++;
                    else {
                        if (enemy->roundsWon > player->roundsWon)
                            enemy->roundsWon++;
                        else
                            if (enemy->roundsWon < player->roundsWon)
                                player->roundsWon++;
                            else {
                                enemy->roundsWon++;
                                player->roundsWon++;
                            }
                    }
            }
        }


        // reset stats after applying effects
        for (Lane* lane : playerLanes)
            lane->resetStats();

        for (Lane* lane : enemyLanes)
            lane->resetStats();

        // reset roundScore after clash PLEASE
        player->roundScore = glm::vec3(0);
        enemy->roundScore = glm::vec3(0);

        if (enemy->roundsWon == 2 && player->roundsWon == 2) {
            // TODO: play again on key P
            cout << "\nTie!\n";
            gameOngoing = false;
            return;
        }

        if (enemy->roundsWon == 2) {
            // TODO: play again on key P
            cout << "\nEnemy won the game!\n";
            gameOngoing = false;
            return;
        } 
        else {
            if (player->roundsWon == 2) {
                // TODO: play again on key P
                cout << "\nYou won the game!\n";
                gameOngoing = false;
                return;
            }
            else 
                if (player->hp == 0 || enemy->hp == 0) {
                    cout << "Round ended. Reseting board...";
                    startRound();
                }
        }
    }
}

void Tema1::startRound() {
    for (Lane* lane : playerLanes)
        lane->clearLane();

    for (Lane* lane : enemyLanes)
        lane->clearLane();

    player->hp = player->maxHP;
    enemy->hp = enemy->maxHP;
}

void Tema1::OnKeyPress(int key, int mods)
{
    if (playerTurn == false) {
        cout << "It's not your turn yet!\n";
        return;
    }

    // Add key press event

    // end player turn
    if (key == GLFW_KEY_ENTER)
    {
        if (!gameOngoing)
            return;

        playerTurn = !playerTurn;
        player->cardsPlayed = 0;
        //enemy->cardsPlayed = 0;

        player->roundScore = glm::vec3(0);
        for (Lane* lane : playerLanes) {
            player->roundScore += lane->computeScores();
        }

        player->printScores();

        turnsPlayed = (turnsPlayed + 1) % 2;    // when turnsPlayed = 0 => both players played a turn and it't time to compare scores
        checkRoundEnd();

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
    if (!gameOngoing)
        return;

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

                    if (player->cardsPlayed >= maxCardsPlayed) {
                        cout << "\nAlready played " << maxCardsPlayed << " cards! End turn or deselect other cards.\n";
                        return;
                    }

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
                        && !playerLanes[l]->cards[i]->isDragged) && playerLanes[l]->cards[i]->used && playerLanes[l]->cards[i]->returnable) {

                        cout << "Moving " << player->hand[i]->name << "back to hand\n";
                            
                        playerLanes[l]->cards[i]->used = false;
                        Hand->addCard(playerLanes[l]->cards[i]);
                        player->hand = Hand->cards;

                        playerLanes[l]->removeCard(playerLanes[l]->cards[i]);
                        player->cardsPlayed--;
                            
                        break;
                    }
                }
            }
        }
        else { // if is dragging and click again
            cout << "checking lanes...\n";
            bool foundValidLane = false;

            if (draggedCard->action == CLEAR) {
                for (Lane* lane : enemyLanes) {
                    for (Card* card : lane->cards) {
                        if (card->action == CLEAR) {
                            lane->removeCard(card);
                        }
                    }
                }

                for (Lane* lane : playerLanes) {
                    for (Card* card : lane->cards) {
                        if (card->action == CLEAR) {
                            lane->removeCard(card);
                        }
                    }
                }
            }

            if (draggedCard->action == FROST) {
                for (int i = 0; i < 3; i++) {
                    if (enemyLanes[i]->isInLane(mouseX, mouseY) && enemyLanes[i]->isValid(draggedCard)) {
                        cout << "adding card " << draggedCard->name << " to lane " << enemyLanes[i]->type << "\n";
                        foundValidLane = true;

                        cout << "center of lane = " << enemyLanes[i]->getCenter() << "\n";
                        draggedCard->pos = enemyLanes[i]->getCenter(); // place card on center
                        enemyLanes[i]->addCard(draggedCard);
                        Hand->removeCard(draggedCard);

                        player->cardsPlayed++;

                        // update player hand after removing draggedCard -- remove if you end up going with Lane enemy hand approach
                        player->hand = Hand->cards;

                        draggedCard->used = true;

                        break;
                    }
                }
            }
            else {
                for (int i = 0; i < 3; i++) {
                    // TODO lane check
                    if (playerLanes[i]->isInLane(mouseX, mouseY) && playerLanes[i]->isValid(draggedCard)) {
                        cout << "adding card " << draggedCard->name << " to lane " << playerLanes[i]->type << "\n";
                        foundValidLane = true;

                        cout << "center of lane = " << playerLanes[i]->getCenter() << "\n";
                        draggedCard->pos = playerLanes[i]->getCenter(); // place card on center
                        playerLanes[i]->addCard(draggedCard);
                        Hand->removeCard(draggedCard);

                        player->cardsPlayed++;

                        // update player hand after removing draggedCard -- remove if you end up going with Lane enemy hand approach
                        player->hand = Hand->cards;

                        draggedCard->used = true;

                        break;
                    }
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
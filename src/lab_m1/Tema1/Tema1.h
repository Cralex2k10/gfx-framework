#pragma once

#include "components/simple_scene.h"

namespace m1
{
    enum cardAction {
        ATK,    // TODO: red
        DEF,    // TODO: blue
        HEAL   // TODO: green
    };

    enum cardType {
        MELEE,
        RANGED,
        SIEGE
    };

    // inline means the compiler replaces the method header with the method code at every turn
    // DEVNOTE: read this if you get lost again: https://www.geeksforgeeks.org/cpp/inline-functions-cpp/
    inline std::string cardTypeToString(cardType type) {
        switch (type) {
            case MELEE: 
                return "melee";
            case RANGED:
                return "ranged";
            case SIEGE:
                return "siege";
        }
    }

    class Card : public Mesh
    {
    public:
        bool isSelected;
        bool isDragged;
        bool wasDrawn;
        bool used;
        std::string name;
        cardAction action;
        std::string type;
        int value;
        glm::vec2 pos;
        glm::vec2 initPos;
        //Lane lane;  // assigned lane at a given time

        float cardSize, width, height;

        Card(const std::string& name, cardAction action, cardType type, int value);//, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices);
        ~Card();
    };

    class Lane : public Mesh
    {
    public:
        float width, height;

        bool isPlayer;
        bool* playerTurn;   // bool ref so it updates as soon as playerTurn in Tema1 changes
        Mesh* shape;
        std::string type;
        std::vector<Card*> cards;
        //glm::vec2 center;   // center of lane -- is it necessary???
        glm::vec2 pos;

        int totalAtk, totalDef, totalHeal;

        Lane(const std::string& type, bool isPlayer, bool* playerTurn, Mesh* shape);
        ~Lane();

        bool isInLane(float x, float y);
        bool isValid(Card* card);
        glm::vec2 getCenter();
        void addCard(Card* card);
        void removeCard(Card* card);
        void updateCardsPos();
        void setSize(float width, float height);
        void getSize();
        void setPos(float x, float y);
        void printCards();
    };

    class Player {
    public:
        Player(bool isPlayer, std::vector<Card*> deck);
        ~Player();

    public:
        bool isPlayer;
        std::vector<Card*> deck;
        std::vector<Card*> hand;
        std::string name;
        int roundScore;
        int roundsWon;
        int hp;

        int cardsPlayed;
    };

    class Tema1 : public gfxc::SimpleScene
    {
    public:
        Tema1();
        ~Tema1();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        
        // student-defined methods
        //void DrawCard(std::string& type);
        void DrawCards(Player* player, int count);
        bool getPlayerTurn();

    protected:
        float cx, cy;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY, offset;
        float angularStep;

        // TODO(student): If you need any other class variables, define them here.
        bool ascending = true;
        bool growing = true;
        float rtx, rty;
        float gtx, gty;
        float btx, bty;

        float carX, carY;
        float rotationAngle;

        // fixed card size
        float cardSize;
        // variables to ensure aspect ratio 
        float lastWidth, lastHeight;
        // TODO: consider removing isResizing if obsolete
        bool isResizing;
        bool isDragging; // game-owned variable to check if mouse is dragging ANY card currently
        Card* draggedCard;   // card that is currently dragged with the mouse
        // used to identify if card hovers eligible lane

        Player* player;
        Player* enemy;

        int handSize = 10;
        std::vector<Card*> playerDeck;
        std::vector<Card*> playerHand;
        std::vector<Card*> tableCards;

        Lane* Hand;
        std::vector<Lane*> playerLanes;
        std::vector<Lane*> enemyLanes;
        //std::vector<Card*> northernRealmsDeck;

        bool playerTurn;
        int maxCardsPlayed = 3;
    };
}   // namespace m1
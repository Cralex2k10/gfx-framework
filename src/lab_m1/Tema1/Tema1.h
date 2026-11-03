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
        bool used;
        std::string name;
        std::string action;
        std::string type;
        int value;

        Card(const std::string& name, cardAction action, cardType type, int value);//, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices);
        ~Card();

        void buildCard(std::string& type);
    };

    class Lane : public Mesh
    {
    public:
        bool isPlayer;
        Mesh* shape;
        std::string type;
        std::vector<Card*> cards;

        int totalAtk, totalDef, totalHeal;

        Lane(const std::string& type, bool isPlayer, Mesh* shape);
        ~Lane();

        bool isInLane(float x, float y);
        glm::vec2 getLaneMiddle();
        void addCard(Card* card);
        void removeCard(Card* card);
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

    protected:
        float cx, cy;
        glm::mat3 modelMatrix;
        float translateX, translateY;
        float scaleX, scaleY;
        float angularStep;

        // TODO(student): If you need any other class variables, define them here.
        bool ascending = true;
        bool growing = true;
        float rtx, rty;
        float gtx, gty;
        float btx, bty;

        float carX, carY;
        float rotationAngle;

        // variables to ensure aspect ratio 
        float lastWidth, lastHeight;
        // TODO: consider removing isResizing if obsolete
        bool isResizing;

        bool isPlayerTurn;
    };
}   // namespace m1
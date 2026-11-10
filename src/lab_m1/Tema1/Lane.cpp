#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace m1;
using namespace std;


float width, height;

// dunno why but i need to type std::string even though i already wrote using namespace std
// get errors otherwise, pls dont ask
Lane::Lane(const std::string& type, bool isPlayer, bool* playerTurn, Mesh* shape) : Mesh(type)
{
	this->isPlayer = isPlayer;
	this->shape = shape;
	this->type = type;
	this->playerTurn = playerTurn;

	// might be unnecessary
	this->pos.x = 0;
	this->pos.y = 0;

	this->cards = vector<Card*>();
	this->totalAtk = 0;	// decrease enemy HP
	this->totalDef = 0;	// decrease enemy DMG
	this->totalHeal = 0;	// increase own HP (TODO: BEFORE or AFTER dealing damage?)
}

Lane::~Lane() 
{
	this->cards = vector<Card*>();
}

bool Lane::isInLane(float x, float y) {
	glm::vec2 bottomLeft = this->pos;	// shape->vertices[0].position;
	glm::vec2 topRight = glm::vec2(this->pos.x + width, this->pos.y + height); // shape->vertices[2].position;

	//cout << "valid area for " << this->type << " is x = [" << bottomLeft.x << ", " << topRight.x << "], y = [" << bottomLeft.y << ", " << topRight.y << "]\n";

	if (x >= bottomLeft.x && x <= topRight.x && y >= bottomLeft.y && y <= topRight.y) {
		return true;
	}
	return false;
}

bool Lane::isValid(Card* card) {
	if (this->isPlayer == *(bool *)playerTurn && this->type == card->type) {
		return true;
	}
	return false;
}

// TODO: implement getLaneMiddle() -> (mid.x, mid.y)
// in order to correctly place cards in the middle, need to transform the pivot from corner to center => 2 transforms
glm::vec2 Lane::getCenter() {
	float midX = this->pos.x + width / 2.0f;
	float midY = this->pos.y + height / 2.0f;
	return glm::vec2(midX, midY);
}

void Lane::addCard(Card* card) {
	this->cards.push_back(card);
	if (this->cards.size() % 2 == 0) {
		// TODO: sort for even number of cards
		card->pos = this->getCenter();
	}
	else {
		// TODO: sort for odd number of cards
		card->pos = this->getCenter();
	}
}

void Lane::removeCard(Card* card) {
	auto it = std::find(cards.begin(), cards.end(), card);
	if (it != cards.end()) {
		cards.erase(it);
	}
}

// separate function in case i want to resize dynamically for some reason
void Lane::setSize(float width, float height) {
	this->width = width;
	this->height = height;
}

void Lane::setPos(float x, float y) {
	this->pos.x = x;
	this->pos.y = y;
}
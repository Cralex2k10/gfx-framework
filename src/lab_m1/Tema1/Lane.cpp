#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace m1;
using namespace std;

// dunno why but i need to type std::string even though i already wrote using namespace std
// get errors otherwise, pls dont ask
Lane::Lane(const std::string& type, bool isPlayer, Mesh* shape) : Mesh(type)
{
	this->isPlayer = isPlayer;
	this->shape = shape;
	this->type = type;
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
	glm::vec2 bottomLeft = shape->vertices[0].position;
	glm::vec2 topRight = shape->vertices[2].position;

	if (x >= bottomLeft.x && x <= topRight.x && y >= bottomLeft.y && y <= topRight.y) {
		return true;
	}
	return false;
}

// TODO: implement getLaneMiddle() -> (mid.x, mid.y)
// in order to correctly place cards in the middle, need to transform the pivot from corner to center => 2 transforms
glm::vec2 Lane::getLaneMiddle() {
	float midX = (shape->vertices[0].position.x + shape->vertices[2].position.x) / 2;
	float midY = (shape->vertices[0].position.y + shape->vertices[2].position.y) / 2;
	return glm::vec2(midX, midY);
}

void Lane::addCard(Card* card) {
	this->cards.push_back(card);
	if (this->cards.size() % 2 == 0) {
		// TODO: sort for even number of cards

	}
	else {
		// TODO: sort for odd number of cards

	}
}

void Lane::removeCard(Card* card) {
	auto it = std::find(cards.begin(), cards.end(), card);
	if (it != cards.end()) {
		cards.erase(it);
	}
}
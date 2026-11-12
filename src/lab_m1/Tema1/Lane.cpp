#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace m1;
using namespace std;


float width, height;
float cardSize = 50;

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

	updateCardsPos();
}

void Lane::updateCardsPos() {
	// this is pretty goofy, please don't judge me too hard
	if (cards.size() == 0)
		return;
	glm::vec2 actualCenter = this->getCenter() - glm::vec2(cards[0]->width / 2.0f, cards[0]->height / 3.0f);
	float offset = 7.5f;

	if (this->cards.size() % 2 == 0) {
		// sort for even number of cards
		for (int i = 0; i < cards.size() / 2; i++) {	// first half of lane cards (drawn to the left of the lane's center)
			cards[i]->pos.x = actualCenter.x - (cards.size() / 2 - (i + 1)) * (cards[i]->width + offset);
			cards[i]->pos.y = actualCenter.y;
		}
		for (int i = 0; i < cards.size() / 2; i++) {	// second half of lane cards (drawn to the right of the lane's center)
			cards[cards.size() / 2 + i]->pos.x = actualCenter.x + (i + 1) * (cards[cards.size() / 2 + i]->width + offset);
			cards[cards.size() / 2 + i]->pos.y = actualCenter.y;
		}
	}
	else {
		// sort for odd number of cards
		for (int i = 0; i < cards.size() / 2; i++) {	// first half of lane cards (drawn to the left of the lane's center)
			cards[i]->pos.x = actualCenter.x - (cards.size() / 2 - (i + 1)) * (cards[i]->width + offset) - (cards[i]->width + offset);	// push another one back to make foom for the middle card
			cards[i]->pos.y = actualCenter.y;
		}

		cards[cards.size() / 2]->pos = actualCenter;

		for (int i = 0; i < cards.size() / 2; i++) {	// second half of lane cards (drawn to the right of the lane's center)
			cards[cards.size() / 2 + i + 1]->pos.x = actualCenter.x + (i + 1) * (cards[cards.size() / 2 + i + 1]->width + offset);
			cards[cards.size() / 2 + i + 1]->pos.y = actualCenter.y;
		}
	}
}

/*
	How i thought about this formula (so i dont forget):
		cards[i]->pos.x = actualCenter.x - (cards.size() / 2 - (i + 1))  * (cards[i]->width + offset);

	- since i couldn't properly move the pivot to the center of the card for some reason, i computed the actual center of the lane
	and pushed the card the size of a card to the left + a little offset so the cards dont overlap (no real point to that)
	- cards.size() / 2 - (i + 1) -- i use this to ensure the cards are rendered in the lane in the same order they appear in the cards vector
	
	* the additional "- (cards[i]->width + offset)" for odd number of cards

	- i dont really know why that works, i just used it as a reference to how much i would have to offset the cards to make room
	for the card in the middle (the *odd* one) and noticed the group of cards was already in the middle of the lane with no overlapping cards
	- so hooray i guess :D
*/

void Lane::removeCard(Card* card) {
	auto it = std::find(cards.begin(), cards.end(), card);
	if (it != cards.end()) {
		cards.erase(it);
	}

	updateCardsPos();
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

void Lane::getSize() {
	glm::vec2 bottomLeft = this->pos;	// shape->vertices[0].position;
	glm::vec2 topRight = glm::vec2(this->pos.x + width, this->pos.y + height); // shape->vertices[2].position;
	cout << "valid area for " << this->type << " is x = [" << bottomLeft.x << ", " << topRight.x << "], y = [" << bottomLeft.y << ", " << topRight.y << "]\n";
}

void Lane::printCards() {
	cout << "in lane " << this->type << " are cards:\n";
	for (int i = 0; i < cards.size(); i++) {
		cout << cards[i]->name << "\n";
	}
}
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
	this->laneAtk = 0;	// decrease enemy HP
	this->laneDef = 0;	// decrease enemy DMG
	this->laneHeal = 0;	// increase own HP (TODO: BEFORE or AFTER dealing damage?)
	this->frost = false;
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
	if ((this->isPlayer == *(bool*)playerTurn || (this->isPlayer != *(bool*)playerTurn && card->action == FROST)) && this->type == card->type || card->action == CLEAR) {
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
			cards[i]->pos.x = actualCenter.x - (cards.size() / 2 - (i + 0.5f)) * (cards[i]->width + offset);
			cards[i]->pos.y = actualCenter.y;
		}
		for (int i = 0; i < cards.size() / 2; i++) {	// second half of lane cards (drawn to the right of the lane's center)
			cards[cards.size() / 2 + i]->pos.x = actualCenter.x + (i + 0.5f) * (cards[cards.size() / 2 + i]->width + offset);
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

glm::vec3 Lane::computeScores() {
	this->laneAtk = 0;
	this->laneDef = 0;
	this->laneHeal = 0;

	// n-am prea inteles care e faza cu "Efectele se aplica la fiecare tura"
	// acum se aplica ATK 1 tura, DEF 3 ture, HEAL 1 tura
	// daca e vorba sa se aplice aceleasi efecte la fiecare tura perpetuu:
	// 1. e broken pt ca geralt da 10 dmg si poti face stack la shields fiind virtual unbeatable
	// 2. tot ce tb sa fac e sa sterg && card->turnsActive != 0 si eventual card->turnsActive--; 
	// din fiecare rand de aici
	// pls give 5 puncte :,)
	for (Card* card : cards) {
		if (card->action == ATK && card->turnsActive != 0) {
			laneAtk += card->value;
			card->turnsActive--;

			if (this->frost)
				laneAtk -= 1;
		}

		if (card->action == DEF && card->turnsActive != 0) {
			laneDef += card->value;
			card->turnsActive--;
			//card->scored = true;	// UNCOMMENT if you want to not keep def scores over turns

			if (this->frost)
				laneAtk -= 1;
		}

		if (card->action == HEAL && card->turnsActive != 0) {
			laneHeal += card->value;
			card->turnsActive--;

			if (this->frost)
				laneAtk -= 1;
		}

		if (card->action == FROST && card->turnsActive != 0) {
			this->frost = true;
			card->turnsActive--;
		}

		if (card->action == FROST && card->turnsActive == 0) {
			this->frost = false;
		}

		if (card->action == CLEAR && card->turnsActive != 0) {
			this->frost = false;
			card->turnsActive--;
		}
	}

	return glm::vec3(laneAtk, laneDef, laneHeal);
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

void Lane::resetStats() {
	this->laneAtk = 0;
	this->laneDef = 0;	// def should probably remain turn by turn
	this->laneHeal = 0;

	for (Card* card : this->cards) {
		card->returnable = false;
	}
}

void Lane::clearLane() {
	this->cards.clear();

	this->laneAtk = 0;
	this->laneDef = 0;
	this->laneHeal = 0;
}
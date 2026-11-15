#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace m1;
using namespace std;

Player::Player(bool isPlayer, vector<Card*> deck) {
	this->isPlayer = isPlayer;
	this->deck = deck;
	this->hand = vector<Card*>();
	this->roundScore = glm::vec3(0);
	this->roundsWon = 0;
	this->hp = maxHP;

	this->cardsPlayed = 0;	// resets to 0 every turn
}

Player::~Player() 
{
}

void Player::printScores() {
	cout << "\nTotal " << (this->isPlayer ? "player" : "enemy") << " scores: ATK = " << roundScore.x << ", DEF = " << roundScore.y << ", HEAL = " << roundScore.z << "\n";
	//this->roundScore = glm::vec3(0);
}
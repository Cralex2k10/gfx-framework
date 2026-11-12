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
	this->roundScore = 0;
	this->roundsWon = 0;
	this->hp = 30;	// subject to change
}

Player::~Player() 
{
}
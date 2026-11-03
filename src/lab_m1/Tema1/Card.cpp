#include "lab_m1/Tema1/Tema1.h"

#include <vector>
#include <iostream>

#include "lab_m1/lab3/transform2D.h"
#include "lab_m1/lab3/object2D.h"

using namespace std;
using namespace m1;

Card::~Card()
{
}

//, const std::vector<VertexFormat>& vertices, const std::vector<unsigned short>& indices)
Card::Card(const std::string& name, cardAction action, cardType type, int value) : Mesh(cardTypeToString(type))	// Needed converter from CardType enum to string
{
	this->isSelected = false;
	this->name = name;
	this->action = action;
	this->type = type;
	this->value = value;
	this->used = false;	// allows the card to be selected from the deck only once	
	// could make cards static (?)
}

void Card::buildCard(string& type) // maybe change name to DrawCard()
{
	object2D::CreateSquare(type, glm::vec3(0, 0, 0), 1, glm::vec3(1, 1, 1), true);
}
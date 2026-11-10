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
	this->isDragged = false;
	this->name = name;
	this->action = action;
	this->type = cardTypeToString(type);
	this->value = value;
	this->wasDrawn = false;
	this->used = false;	// allows the card to be selected from the deck only once	
	// TODO change to update automatically on lane sort
	// no need to attribute lane on create.
	this->pos = glm::vec2(0, 0);
	
	// could make cards static (?)
	//object2D::CreateSquare(this->type, glm::vec3(0, 0, 0), 1, glm::vec3(1, 1, 1), true);
}

//void Card::drawCard(string& type) // maybe change name to DrawCard()
//{
//	object2D::CreateSquare(type, glm::vec3(0, 0, 0), 1, glm::vec3(1, 1, 1), true);
//}


#include <cstdlib>

#include "character.hpp"

float Character::randNormal(){
	return (float)rand() / (float)RAND_MAX;
}

float Character::randCenteredNormal(){
	return randNormal() * 2.0 - 1.0;
}

Character::Character(unsigned int size,const FloatSet *ref)
:FloatSet(size,ref){
	
}

Character::~Character(){
	
}
#include <cstdlib>

#include "set.hpp"

float randNormal(){
	return (float)rand() / (float)RAND_MAX;
}

float randCenteredNormal(){
	return randNormal() * 2.0 - 1.0;
}

Set::Set()
:vals{}{
	
}

Set::Set(FILE *in)
:vals{}{
	for(unsigned int i = 0;i < SET_SIZE;++i){
		vals[i] = normalizedInt16::read(in);
	}
}

void Set::write(FILE *out) const{
	for(unsigned int i = 0;i < SET_SIZE;++i){
		normalizedInt16::write(out,vals[i]);
	}
}

float Set::get(unsigned int i) const{
	return normalizedInt16::toFloat(vals[i]);
}

Set *Set::set(unsigned int i,float newVal){
	vals[i] = normalizedInt16::fromFloat(newVal);
	
	return this;
}
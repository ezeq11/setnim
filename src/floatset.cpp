#include <cstdlib>

#include "floatset.hpp"

float FloatSet::randNormal(){
	return (float)rand() / (float)RAND_MAX;
}

float FloatSet::randCenteredNormal(){
	return randNormal() * 2.0 - 1.0;
}

FloatSet::FloatSet(unsigned int size)
:count(size),vals(new float[size]()){
	
}

FloatSet::FloatSet(unsigned int size,const FloatSet *ref)
:count(size),vals(new float[size]()){
	if(ref == NULL){
		return;
	}
	
	for(unsigned int i = 0;i < count;++i){
		set(i,ref->get(i));
	}
}

FloatSet::~FloatSet(){
	delete[] vals;
}

unsigned int FloatSet::size() const{
	return count;
}

float FloatSet::get(unsigned int i) const{
	if(i >= count){
		return 0.0;
	}
	
	return vals[i];
}

void FloatSet::set(unsigned int i,float newVal){
	if(i >= count){
		return;
	}
	
	vals[i] = newVal;
}
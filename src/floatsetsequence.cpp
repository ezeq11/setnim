#include <cmath>

#include "floatsetsequence.hpp"
#include "render.hpp"

FloatSetSequence::FloatSetSequence(FloatSet *newBuffer)
:sets(),selected(sets.end()),buffer(0.0,newBuffer){
	
}

void FloatSetSequence::add(float time,FloatSet *set){
	sets.insert(find(time),new TimedContainer(time,set));
}

FloatSet *FloatSetSequence::select(float time){
	std::list<TimedContainer *>::iterator it = find(time);
	
	if(it == sets.begin()){
		selected = sets.end();
		
		return NULL;
	}
	
	selected = --it;
	
	return (*selected)->getSet();
}

void FloatSetSequence::deselect(){
	selected = sets.end();
}

void FloatSetSequence::remove(){
	if(selected == sets.end()){
		return;
	}
	
	delete *selected;
	sets.erase(selected);
	selected = sets.end();
}

void FloatSetSequence::move(float newTime){
	if(selected == sets.end()){
		return;
	}
	
	std::list<TimedContainer *>::iterator newPosition = find(newTime);
	TimedContainer *moved = *selected;
	
	sets.erase(selected);
	sets.insert(newPosition,moved);
	
	selected = --newPosition;
}

const FloatSet *FloatSetSequence::getInstant(float time){
	std::list<TimedContainer *>::iterator it = find(time);
	
	if(it == sets.begin()){
		return NULL;
	}
	
	buffer.setTime(time);
	TimedContainer *end = *it;
	TimedContainer *begin = *(--it);
	
	buffer.interpolateFrom(*begin,*end);
	
	return buffer.getSet();
}

#define DRAW_ITERATOR() render::drawSequenceBar(it == selected,((*it)->getTime() - begin) / (end - begin))

void FloatSetSequence::drawBar(float begin,float end){
	std::list<TimedContainer *>::iterator it = find(begin);
	
	if(it != sets.begin()){
		--it;
		DRAW_ITERATOR();
		++it;
	}
	
	while(it != sets.end() && (*it)->getTime() < end){
		DRAW_ITERATOR();
		++it;
	}
	
	if(it != sets.end()){
		DRAW_ITERATOR();
	}
}

std::list<FloatSetSequence::TimedContainer *>::iterator FloatSetSequence::find(float time){
	std::list<TimedContainer *>::iterator it = sets.begin();
	
	while(it != sets.end() && (*it)->getTime() < time){
		++it;
	}
	
	return it;
}

FloatSetSequence::TimedContainer::TimedContainer(float newTime,FloatSet *newSet)
:time(newTime),set(newSet),transitions(newSet == NULL ? NULL : new Transition[newSet->size()]()){
	
}

FloatSetSequence::TimedContainer::~TimedContainer(){
	delete[] transitions;
}

float FloatSetSequence::TimedContainer::getTime() const{
	return time;
}

void FloatSetSequence::TimedContainer::setTime(float newTime){
	time = newTime;
}

FloatSet *FloatSetSequence::TimedContainer::getSet() const{
	return set;
}

FloatSetSequence::Transition FloatSetSequence::TimedContainer::getTransition(unsigned int i) const{
	if(set == NULL || i >= set->size()){
		return (FloatSetSequence::Transition)0;
	}
	
	return transitions[i];
}

void FloatSetSequence::TimedContainer::setTransition(unsigned int i,Transition newTransition){
	if(set == NULL || i >= set->size()){
		return;
	}
	
	transitions[i] = newTransition;
}

void FloatSetSequence::TimedContainer::interpolateFrom(unsigned int i,const TimedContainer &beginning,const TimedContainer &end){
	if(set == NULL || i >= set->size()){
		return;
	}
	
	float deltaT = (time - beginning.time) / (end.time - beginning.time);
	float deltaV = end.set->get(i) - beginning.set->get(i);
	
	set->set(i,beginning.set->get(i) + interpolate(transitions[i],deltaT) * deltaV);
}

void FloatSetSequence::TimedContainer::interpolateFrom(const TimedContainer &beginning,const TimedContainer &end){
	if(set == NULL){
		return;
	}
	
	float deltaT,deltaV;
	deltaT = (time - beginning.time) / (end.time - beginning.time);
	
	for(unsigned int i = 0;i < set->size();++i){
		deltaV = end.set->get(i) - beginning.set->get(i);
		
		set->set(i,beginning.set->get(i) + interpolate(transitions[i],deltaT) * deltaV);
	}
}

float FloatSetSequence::TimedContainer::interpolate(Transition t,float x){
	switch(t){
		case TRANSITION_X:
			return x;
			
		case TRANSITION_X_SQUARED:
			return x * x;
			
		case TRANSITION_X_CUBED:
			return x * x * x;
			
		case TRANSITION_SIN:
			x = sin(PI * x / 2.0);
			
			return x;
			
		case TRANSITION_SIN_SQUARED:
			x = sin(PI * x / 2.0);
			
			return x * x;
			
		case TRANSITION_SIN_CUBED:
			x = sin(PI * x / 2.0);
			
			return x * x * x;
			
		case TRANSITION_CIRCLE_CONVEX:
			return sqrt(1.0 - (x - 1.0) * (x - 1.0));
			
		case TRANSITION_CIRCLE_CONCAVE:
			return 1.0 - sqrt(1.0 - x * x);
			
		case TRANSITION_ZERO:
			return 0.0;
			
		case TRANSITION_ONE:
			return 1.0;
			
		case TRANSITION_COUNT:
		default:
			break;
	}
	
	return interpolate((Transition)0,x);
}
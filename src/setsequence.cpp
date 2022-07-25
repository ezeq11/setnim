#include <cmath>
#include <iterator>

#include "setsequence.hpp"
#include "render.hpp"

float interpolate(Transition t,float x){
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

// ====================================================================================

SetSequence::TimedSet::TimedSet(temporal::val newTime)
:Set(),time(newTime),transitions{}{
	
}

SetSequence::TimedSet::TimedSet(FILE *in)
:Set(in),time(temporal::read(in)),transitions{}{
	for(unsigned int i = 0;i < SET_SIZE;++i){
		transitions[i] = (Transition)uInt32::read(in);
	}
}

void SetSequence::TimedSet::write(FILE *out) const{
	Set::write(out);
	temporal::write(out,time);
	
	for(unsigned int i = 0;i < SET_SIZE;++i){
		uInt32::write(out,(uint32_t)transitions[i]);
	}
}

temporal::val SetSequence::TimedSet::getTime() const{
	return time;
}

SetSequence::TimedSet *SetSequence::TimedSet::setTime(temporal::val newTime){
	time = newTime;
	
	return this;
}

float SetSequence::TimedSet::get(unsigned int i) const{
	return Set::get(i);
}

SetSequence::TimedSet *SetSequence::TimedSet::set(unsigned int i,float newVal){
	Set::set(i,newVal);
	
	return this;
}

Transition SetSequence::TimedSet::getTransition(unsigned int i) const{
	return transitions[i];
}

SetSequence::TimedSet *SetSequence::TimedSet::setTransition(unsigned int i,Transition newTransition){
	transitions[i] = newTransition;
	
	return this;
}

// ====================================================================================

std::list<SetSequence::TimedSet *>::iterator SetSequence::find(temporal::val time){
	std::list<TimedSet *>::iterator it = sets.begin();
	
	while(it != sets.end() && (*it)->getTime() < time){
		++it;
	}
	
	return it;
}

std::list<SetSequence::TimedSet *>::const_iterator SetSequence::find(temporal::val time) const{
	std::list<TimedSet *>::const_iterator it = sets.begin();
	
	while(it != sets.end() && (*it)->getTime() < time){
		++it;
	}
	
	return it;
}

// ------------------------------------------------------------------------------------

SetSequence::SetSequence(unsigned int newRenderIndex)
:renderIndex(newRenderIndex),buffer(),sets(),selected(sets.end()){
	
}

SetSequence::SetSequence(FILE *in)
:renderIndex(uInt32::read(in)),buffer(),sets(),selected(sets.end()){
	unsigned int seqSize = uInt32::read(in);
	unsigned int selectionIndex = uInt32::read(in);
	
	// Populate
	for(unsigned int i = 0;i < seqSize;++i){
		sets.push_back(new TimedSet(in));
	}
	
	// Count
	selected = sets.begin();
	
	for(unsigned int i = 0;i < selectionIndex;++i){
		++selected;
	}
}

void SetSequence::write(FILE *out) const{
	uInt32::write(out,(uint32_t)renderIndex);
	
	uInt32::write(out,(uint32_t)sets.size());
	uInt32::write(out,(uint32_t)std::distance(sets.begin(),(std::list<TimedSet *>::const_iterator)selected));
	
	for(std::list<TimedSet *>::const_iterator it = sets.begin();it != sets.end();++it){
		(*it)->write(out);
	}
}

SetSequence::~SetSequence(){
	for(std::list<TimedSet *>::iterator it = sets.begin();it != sets.end();++it){
		delete *it;
	}
}

unsigned int SetSequence::getRenderIndex() const{
	return renderIndex;
}

const Set *SetSequence::getBuffer() const{
	return &buffer;
}

bool SetSequence::bufferInstant(temporal::val time){
	std::list<TimedSet *>::const_iterator it = find(time);
	
	if(it == sets.begin()){
		return false;
	}
	
	TimedSet *end = *it;
	TimedSet *begin = *(--it);
	
	float deltaT,deltaV;
	deltaT = temporal::toFloat(time - begin->getTime()) / temporal::toFloat(end->getTime() - begin->getTime());
	
	for(unsigned int i = 0;i < SET_SIZE;++i){
		deltaV = end->get(i) - begin->get(i);
		
		buffer.set(i,begin->get(i) + interpolate(begin->getTransition(i),deltaT) * deltaV);
	}
	
	return true;
}

bool SetSequence::bufferCurrent(){
	if(selected == sets.end()){
		return false;
	}
	
	buffer.copyFrom(*(Set *)(*selected));
	return true;
}

void SetSequence::add(temporal::val time){
	sets.insert(find(time),new TimedSet(time));
}

void SetSequence::select(temporal::val time){
	std::list<TimedSet *>::iterator lastSelected = selected;
	std::list<TimedSet *>::iterator it = find(time);
	
	if(it == sets.begin()){
		selected = sets.end();
		
		return;
	}
	
	selected = --it;
	
	if(lastSelected == selected){
		selected = sets.end();
	}
}

void SetSequence::deselect(){
	selected = sets.end();
}

Set *SetSequence::current() const{
	return selected == sets.end() ? NULL : *selected;
}

void SetSequence::remove(){
	if(selected == sets.end()){
		return;
	}
	
	delete *selected;
	
	sets.erase(selected);
	selected = sets.end();
}

void SetSequence::move(temporal::val newTime){
	if(selected == sets.end()){
		return;
	}
	
	TimedSet *moved = *selected;
	moved->setTime(newTime);
	
	sets.erase(selected);
	std::list<TimedSet *>::iterator newPosition = find(newTime);
	sets.insert(newPosition,moved);
	
	selected = --newPosition;
}

#define DRAW_ITERATOR() render::drawSequenceBar(it == selected ? render::SEQ_BAR_HIGHLIGHTED : render::SEQ_BAR_NORMAL,temporal::toFloat((*it)->getTime() - begin) / temporal::toFloat(end - begin))

void SetSequence::drawBar(temporal::val begin,temporal::val end) const{
	std::list<TimedSet *>::const_iterator it = find(begin);
	
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
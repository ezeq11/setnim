#include <cmath>
#include <iterator>

#include "setsequence.hpp"
#include "render.hpp"
#include "renderers/main.hpp"

const char *labelTransition(Transition t){
	switch(t){
		case TRANSITION_X:
			return "x";
			
		case TRANSITION_X_SQUARED:
			return "x^2";
			
		case TRANSITION_X_CUBED:
			return "x^3";
			
		case TRANSITION_SIN:
			return "sin(x)";
			
		case TRANSITION_SIN_SQUARED:
			return "sin(x)^2";
			
		case TRANSITION_SIN_CUBED:
			return "sin(x)^3";
			
		case TRANSITION_CIRCLE_CONVEX:
			return "convex circle";
			
		case TRANSITION_CIRCLE_CONCAVE:
			return "concave circle";
			
		case TRANSITION_ZERO:
			return "0";
			
		case TRANSITION_ONE:
			return "1";
			
		case TRANSITION_COUNT:
		default:
			break;
	}
	
	return NULL;
}

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

SetSequence::TimedSet::TimedSet(normalizedUInt16::val newTime)
:Set(),time(newTime),transitions{}{
	
}

SetSequence::TimedSet::TimedSet(FILE *in)
:Set(in),time(normalizedUInt16::read(in)),transitions{}{
	for(unsigned int i = 0;i < SET_SIZE;++i){
		transitions[i] = (Transition)uInt32::read(in);
	}
}

void SetSequence::TimedSet::write(FILE *out) const{
	Set::write(out);
	normalizedUInt16::write(out,time);
	
	for(unsigned int i = 0;i < SET_SIZE;++i){
		uInt32::write(out,(uint32_t)transitions[i]);
	}
}

SetSequence::TimedSet &SetSequence::TimedSet::copyFrom(const TimedSet &t){
	Set::copyFrom(t);
	
	time = t.time;
	
	for(unsigned int i = 0;i < SET_SIZE;++i){
		transitions[i] = t.transitions[i];
	}
	
	return *this;
}

normalizedUInt16::val SetSequence::TimedSet::getTime() const{
	return time;
}

SetSequence::TimedSet &SetSequence::TimedSet::setTime(normalizedUInt16::val newTime){
	time = newTime;
	
	return *this;
}

float SetSequence::TimedSet::get(unsigned int i) const{
	return Set::get(i);
}

SetSequence::TimedSet &SetSequence::TimedSet::set(unsigned int i,float newVal){
	Set::set(i,newVal);
	
	return *this;
}

Transition SetSequence::TimedSet::getTransition(unsigned int i) const{
	return transitions[i];
}

SetSequence::TimedSet &SetSequence::TimedSet::setTransition(unsigned int i,Transition newTransition){
	transitions[i] = newTransition;
	
	return *this;
}

// ====================================================================================

std::list<SetSequence::TimedSet *>::iterator SetSequence::find(normalizedUInt16::val time){
	std::list<TimedSet *>::iterator it = sets.begin();
	
	while(it != sets.end() && (*it)->getTime() < time){
		++it;
	}
	
	return it;
}

std::list<SetSequence::TimedSet *>::const_iterator SetSequence::find(normalizedUInt16::val time) const{
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
	
	for(unsigned int i = 0;i < seqSize;++i){
		sets.push_back(new TimedSet(in));
	}
}

void SetSequence::write(FILE *out) const{
	uInt32::write(out,(uint32_t)renderIndex);
	uInt32::write(out,(uint32_t)sets.size());
	
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

const Set &SetSequence::getBuffer() const{
	return buffer;
}

bool SetSequence::bufferInstant(normalizedUInt16::val time){
	std::list<TimedSet *>::const_iterator it = find(time);
	
	if(it == sets.begin()){
		return false;
	}
	
	if(it == sets.end()){
		buffer.copyFrom(*(Set *)(*(--it)));
		return true;
	}
	
	TimedSet *end = *it;
	TimedSet *begin = *(--it);
	
	float deltaT,deltaV;
	deltaT = normalizedUInt16::toFloat(time - begin->getTime()) / normalizedUInt16::toFloat(end->getTime() - begin->getTime());
	
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

void SetSequence::select(normalizedUInt16::val time){
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

Transition SetSequence::getTransition(unsigned int i) const{
	if(selected == sets.end()){
		return TRANSITION_COUNT;
	}
	
	return (*selected)->getTransition(i);
}

void SetSequence::setTransition(unsigned int i,Transition newTransition){
	if(selected == sets.end()){
		return;
	}
	
	(*selected)->setTransition(i,newTransition);
}

void SetSequence::add(normalizedUInt16::val time){
	sets.insert(find(time),(TimedSet *)&renderers::sets::init(renderIndex,*(new TimedSet(time))));
}

void SetSequence::duplicate(normalizedUInt16::val time){
	if(selected == sets.end()){
		return;
	}
	
	sets.insert(find(time),&((new TimedSet(time))->copyFrom(**selected).setTime(time)));
}

void SetSequence::remove(){
	if(selected == sets.end()){
		return;
	}
	
	delete *selected;
	
	sets.erase(selected);
	selected = sets.end();
}

void SetSequence::move(normalizedUInt16::val newTime){
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

#define DRAW_ITERATOR() render::UI::markers::drawSeqFrame( \
	it == selected ? render::UI::markers::SEQ_FRAME_HIGHLIGHTED : render::UI::markers::SEQ_FRAME_NORMAL, \
	normalizedUInt16::toFloat((*it)->getTime() - begin) / normalizedUInt16::toFloat(end - begin) \
)

void SetSequence::drawMarkers(normalizedUInt16::val begin,normalizedUInt16::val end) const{
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
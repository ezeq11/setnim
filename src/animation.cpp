#include "animation.hpp"
#include "render.hpp"

void charSceneAddBefore(Character *c,unsigned int i,bool isCurrent){
	c->addBefore(new FloatSetSequence(c));
}

void charSceneAddAfter(Character *c,unsigned int i,bool isCurrent){
	c->addAfter(new FloatSetSequence(c));
}

void charSceneForward(Character *c,unsigned int i,bool isCurrent){
	c->forward();
}

void charSceneBackward(Character *c,unsigned int i,bool isCurrent){
	c->backward();
}

void charSceneRemove(Character *c,unsigned int i,bool isCurrent){
	delete c->current();
	c->remove();
}

void charSceneClear(Character *c,unsigned int i,bool isCurrent){
	while(c->length() > 0){
		charSceneRemove(c,0,false);
	}
}

void Animation::charPrepare(Character *newChar){
	if(newChar == NULL){
		return;
	}
	
	charSceneClear(newChar,0,false);
	
	for(unsigned int i = 0;i < sceneLengths.length();++i){
		if(i < sceneLengths.pos()){
			charSceneAddBefore(newChar,0,false);
		}else{
			charSceneAddAfter(newChar,0,false);
		}
	}
}

Animation::~Animation(){
	sceneClear();
	charClear();
}

void Animation::charAddBefore(Character *newChar){
	charPrepare(newChar);
	characters.addBefore(newChar);
}

void Animation::charAddAfter(Character *newChar){
	charPrepare(newChar);
	characters.addAfter(newChar);
}

void Animation::charForward(){
	characters.forward();
}

void Animation::charBackward(){
	characters.backward();
}

void Animation::charRemove(){
	if(characters.current() == NULL){
		return;
	}
	
	charSceneClear(characters.current(),0,false);
	delete characters.current();
	
	characters.remove();
}

void Animation::charClear(){
	while(characters.length() > 0){
		charRemove();
	}
}

int charMarkerCurrentPos;

void charDrawMarker(Character *c,unsigned int i,bool isCurrent){
	render::drawCharMarker(isCurrent,(int)i - charMarkerCurrentPos);
}

void Animation::charDrawMarkers(){
	charMarkerCurrentPos = characters.pos();
	characters.forAll(charDrawMarker);
}

void Animation::sceneAddBefore(float length){
	characters.forAll(charSceneAddBefore);
	sceneLengths.addBefore(new float[1]{length});
}

void Animation::sceneAddAfter(float length){
	characters.forAll(charSceneAddAfter);
	sceneLengths.addAfter(new float[1]{length});
}

void Animation::sceneForward(){
	characters.forAll(charSceneForward);
	sceneLengths.forward();
}

void Animation::sceneBackward(){
	characters.forAll(charSceneBackward);
	sceneLengths.backward();
}

void Animation::sceneRemove(){
	characters.forAll(charSceneRemove);
	
	delete sceneLengths.current();
	sceneLengths.remove();
}

void Animation::sceneClear(){
	while(sceneLengths.length() > 0){
		sceneRemove();
	}
}

int sceneMarkerCurrentPos;

void sceneDrawMarker(float *length,unsigned int i,bool isCurrent){
	render::drawSceneMarker(isCurrent,(int)i - sceneMarkerCurrentPos);
}

void Animation::sceneDrawMarkers(){
	sceneMarkerCurrentPos = sceneLengths.pos();
	sceneLengths.forAll(sceneDrawMarker);
}

float Animation::sceneGetLength(){
	return sceneLengths.current() == NULL ? 0.0 : *(sceneLengths.current());
}

void Animation::sceneSetLength(float newLength){
	if(sceneLengths.current() == NULL){
		return;
	}
	
	sceneLengths.current()[0] = newLength;
}

const Character *Animation::currentCharacter(){
	return characters.current();
}

FloatSetSequence *Animation::currentSequence(){
	if(characters.current() == NULL){
		return NULL;
	}
	
	return characters.current()->current();
}
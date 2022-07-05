#include "animation.hpp"

void charSceneAddBefore(Character *c){
	c->addBefore(new FloatSetSequence(c));
}

void charSceneAddAfter(Character *c){
	c->addAfter(new FloatSetSequence(c));
}

void charSceneForward(Character *c){
	c->forward();
}

void charSceneBackward(Character *c){
	c->backward();
}

void charSceneRemove(Character *c){
	delete c->current();
	c->remove();
}

void charSceneClear(Character *c){
	while(c->length() > 0){
		charSceneRemove(c);
	}
}

void Animation::charPrepare(Character *newChar){
	if(newChar == NULL){
		return;
	}
	
	charSceneClear(newChar);
	
	for(unsigned int i = 0;i < sceneLengths.length();++i){
		if(i < sceneLengths.pos()){
			charSceneAddBefore(newChar);
		}else{
			charSceneAddAfter(newChar);
		}
	}
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
	
	charSceneClear(characters.current());
	delete characters.current();
	
	characters.remove();
}

void Animation::charClear(){
	while(characters.length() > 0){
		charRemove();
	}
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
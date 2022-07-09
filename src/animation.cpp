#include "animation.hpp"
#include "render.hpp"

SceneRenderSeq::SceneRenderSeq(Renderable *newR,SetSequence<float> *newS)
:r(newR),s(newS){
	
}

SceneRenderSeq::~SceneRenderSeq(){
	delete s;
}

Renderable *SceneRenderSeq::getRenderable() const{
	return r;
}

SetSequence<float> *SceneRenderSeq::getSeq() const{
	return s;
}

// ====================================================================================

Scene::Scene(float newTime)
:time(newTime){
	
}

float Scene::getTime() const{
	return time;
}

void Scene::setTime(float newTime){
	time = newTime;
}

// ====================================================================================

void Animation::renderablesAddBefore(Renderable *newRenderable){
	renderables.addBefore(newRenderable);
}

void Animation::renderablesAddAfter(Renderable *newRenderable){
	renderables.addAfter(newRenderable);
}

void Animation::renderablesForward(){
	renderables.forward();
}

void Animation::renderablesBackward(){
	renderables.backward();
}

// -------------------------------

Renderable *renderableToRemove;

bool seqRemoveRenderable(SceneRenderSeq *s){
	return s->getRenderable() == renderableToRemove;
}

void sceneRemoveRenderable(Scene *s,unsigned int i,bool isCurrent){
	s->removeIf(&seqRemoveRenderable);
}

void Animation::renderablesRemove(){
	renderableToRemove = renderables.current();
	scenes.forAll(&sceneRemoveRenderable);
	
	renderables.remove();
}

// -------------------------------

void sceneClear(Scene *s,unsigned int i,bool isCurrent){
	s->clear();
}

void Animation::renderablesClear(){
	scenes.forAll(&sceneClear);
	renderables.clear();
}

// ------------------------------------------------------------------------------------

void Animation::sceneAddBefore(float length){
	scenes.addBefore(new Scene(length));
}

void Animation::sceneAddAfter(float length){
	scenes.addAfter(new Scene(length));
}

void Animation::sceneForward(){
	scenes.forward();
}

void Animation::sceneBackward(){
	scenes.backward();
}

void Animation::sceneRemove(){
	scenes.remove();
}

void Animation::sceneClear(){
	scenes.clear();
}

float Animation::sceneGetLength() const{
	if(scenes.current() == NULL){
		return 0.0;
	}
	
	return scenes.current()->getTime();
}

void Animation::sceneSetLength(float newLength){
	if(scenes.current() == NULL){
		return;
	}
	
	scenes.current()->setTime(newLength);
}

// -------------------------------

float totalTime;

void sceneSumTime(Scene *s,unsigned int i,bool isCurrent){
	totalTime += s->getTime();
}

float Animation::length() const{
	totalTime = 0.0;
	scenes.forAll(&sceneSumTime);
	
	return totalTime;
}

// ------------------------------------------------------------------------------------

void Animation::seqAddBefore(){
	if(renderables.current() == NULL || scenes.current() == NULL){
		return;
	}
	
	scenes.current()->addBefore(new SceneRenderSeq(renderables.current(),renderables.current()->allocNewSeq()));
}

void Animation::seqAddAfter(){
	if(renderables.current() == NULL || scenes.current() == NULL){
		return;
	}
	
	scenes.current()->addAfter(new SceneRenderSeq(renderables.current(),renderables.current()->allocNewSeq()));
}

void Animation::seqForward(){
	if(scenes.current() == NULL){
		return;
	}
	
	scene.current()->forward();
}

void Animation::seqBackward(){
	if(scenes.current() == NULL){
		return;
	}
	
	scene.current()->backward();
}

void Animation::seqRemove(){
	if(scenes.current() == NULL){
		return;
	}
	
	scene.current()->remove();
}

void Animation::seqClear(){
	if(scenes.current() == NULL){
		return;
	}
	
	scene.current()->clear();
}

SetSequence<float> *Animation::seqCurrent() const{
	if(scenes.current() == NULL || scenes.current()->current() == NULL){
		return NULL;
	}
	
	return scenes.current()->current()->getSeq();
}

// ------------------------------------------------------------------------------------

int currentMarkerPos;

void drawRenderableMarker(Renderable *r,unsigned int i,bool isCurrent){
	render::drawRenderableMarker(isCurrent,(int)i - currentMarkerPos);
}

void drawSceneMarker(Scene *s,unsigned int i,bool isCurrent){
	render::drawSceneMarker(isCurrent,(int)i - currentMarkerPos);
}

void Animation::drawMarkers() const{
	currentMarkerPos = renderables.pos();
	renderables.forAll(&drawRenderableMarker);
	
	currentMarkerPos = scenes.pos();
	scenes.forAll(&drawSceneMarker);
}

// -------------------------------

bool stillCurrentRenderableFrame;

void drawSeqFrame(SceneRenderSeq *s,unsigned int i,bool isCurrent){
	if(isCurrent && stillCurrentRenderableFrame){
		
	}else{
		s->getSeq()->bufferInstant(time);
		s->getRenderable()->draw(time);
	}
}

void Animation::render(float time,bool stillActiveFrame) const{
	if(scenes.current() == NULL){
		return;
	}
	
	stillCurrentRenderableFrame = stillActiveFrame;
	scenes.current()->forAll(&drawSeqFrame);
}
#include "animation.hpp"
#include "render.hpp"

Scene::Scene(temporal::val newLength)
:length(newLength){
	
}

Scene::Scene(FILE *in,Renderable **buffers,unsigned int bufferCount)
:length(temporal::read(in)){
	unsigned int seqCount = uInt32::read(in);
	
	for(unsigned int i = 0;i < seqCount;++i){
		addBefore(new SetSequence(in,buffers,bufferCount));
	}
	
	for(unsigned int i = 0;i < seqCount;++i){
		backward();
	}
}

FILE *fileIO;
Renderable **bufferTable;
unsigned int bufferTableSize;

void writeSeq(SetSequence *s,unsigned int i,bool isCurrent){
	s->write(fileIO,bufferTable,bufferTableSize);
}

void Scene::write(FILE *out,Renderable **buffers,unsigned int bufferCount) const{
	fileIO = out;
	bufferTable = buffers;
	bufferTableSize = bufferCount;
	
	temporal::write(out,length);
	forAll(&writeSeq);
}

temporal::val Scene::getLength() const{
	return length;
}

void Scene::setLength(temporal::val newLength){
	length = newLength;
}

// ====================================================================================

Animation::Animation(){
	
}

Animation::Animation(FILE *in){
	// Renderables
	unsigned int renderablesCount = uInt32::read(in);
	bufferTable = new Renderable *[renderablesCount];
	
	for(unsigned int i = 0;i < renderablesCount;++i){
		bufferTable[i] = new Renderable(in);
		
		renderables.addBefore(bufferTable[i]);
	}
	
	for(unsigned int i = 0;i < renderablesCount;++i){
		renderables.backward();
	}
	
	// Scenes
	unsigned int scenesCount = uInt32::read(in);
	
	for(unsigned int i = 0;i < scenesCount;++i){
		scenes.addBefore(new Scene(in,bufferTable,renderablesCount));
	}
	
	for(unsigned int i = 0;i < scenesCount;++i){
		scenes.backward();
	}
	
	delete[] bufferTable;
}

void writeRenderable(Renderable *r,unsigned int i,bool isCurrent){
	r->write(fileIO);
}

void tableRenderable(Renderable *r,unsigned int i,bool isCurrent){
	bufferTable[i] = r;
}

void writeScene(Scene *s,unsigned int i,bool isCurrent){
	s->write(fileIO,bufferTable,bufferTableSize);
}

void Animation::write(FILE *out) const{
	fileIO = out;
	
	// Renderables
	uInt32::write(out,(uint32_t)renderables.length());
	renderables.forAll(&writeRenderable);
	
	// Scenes
	bufferTableSize = renderables.length();
	bufferTable = new Renderable *[bufferTableSize];
	renderables.forAll(&tableRenderable);
	
	uInt32::write(out,(uint32_t)scenes.length());
	scenes.forAll(&writeScene);
	
	delete[] bufferTable;
}

// ------------------------------------------------------------------------------------

void Animation::renderablesAddBefore(unsigned int renderIndex){
	renderables.addBefore(new Renderable(renderIndex));
}

void Animation::renderablesAddAfter(unsigned int renderIndex){
	renderables.addAfter(new Renderable(renderIndex));
}

void Animation::renderablesForward(){
	renderables.forward();
}

void Animation::renderablesBackward(){
	renderables.backward();
}

// -------------------------------

Renderable *renderableToRemove;

bool seqRemoveRenderable(SetSequence *s){
	return s->getBuffer() == renderableToRemove;
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

void emptyScene(Scene *s,unsigned int i,bool isCurrent){
	s->clear();
}

void Animation::renderablesClear(){
	scenes.forAll(&emptyScene);
	renderables.clear();
}

// ------------------------------------------------------------------------------------

void Animation::sceneAddBefore(temporal::val length){
	scenes.addBefore(new Scene(length));
}

void Animation::sceneAddAfter(temporal::val length){
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

bool Animation::sceneAvailable() const{
	return scenes.current() != NULL;
}

temporal::val Animation::sceneGetLength() const{
	if(scenes.current() == NULL){
		return 0;
	}
	
	return scenes.current()->getLength();
}

void Animation::sceneSetLength(temporal::val newLength){
	if(scenes.current() == NULL){
		return;
	}
	
	scenes.current()->setLength(newLength);
}

// -------------------------------

temporal::val totalTime;

void sceneSumTime(Scene *s,unsigned int i,bool isCurrent){
	totalTime += s->getLength();
}

temporal::val Animation::length() const{
	totalTime = 0;
	scenes.forAll(&sceneSumTime);
	
	return totalTime;
}

// ------------------------------------------------------------------------------------

void Animation::seqAddBefore(){
	if(renderables.current() == NULL || scenes.current() == NULL){
		return;
	}
	
	scenes.current()->addBefore(new SetSequence(renderables.current()));
}

void Animation::seqAddAfter(){
	if(renderables.current() == NULL || scenes.current() == NULL){
		return;
	}
	
	scenes.current()->addAfter(new SetSequence(renderables.current()));
}

void Animation::seqForward(){
	if(scenes.current() == NULL){
		return;
	}
	
	scenes.current()->forward();
}

void Animation::seqBackward(){
	if(scenes.current() == NULL){
		return;
	}
	
	scenes.current()->backward();
}

void Animation::seqRemove(){
	if(scenes.current() == NULL){
		return;
	}
	
	scenes.current()->remove();
}

void Animation::seqClear(){
	if(scenes.current() == NULL){
		return;
	}
	
	scenes.current()->clear();
}

SetSequence *Animation::seqCurrent() const{
	if(scenes.current() == NULL){
		return NULL;
	}
	
	return scenes.current()->current();
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

void drawSeqFrame(SetSequence *s,unsigned int i,bool isCurrent){
	if(isCurrent && stillCurrentRenderableFrame){
		
	}else{
		// s->getSeq()->bufferInstant(time);
		// s->getRenderable()->draw(time);
	}
}

void Animation::render(float time,bool stillActiveFrame) const{
	if(scenes.current() == NULL){
		return;
	}
	
	stillCurrentRenderableFrame = stillActiveFrame;
	scenes.current()->forAll(&drawSeqFrame);
}
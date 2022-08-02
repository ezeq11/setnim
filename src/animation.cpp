#include "animation.hpp"
#include "render.hpp"

Scene::Scene(frames::val newLength)
:length(newLength){
	
}

Scene::Scene(FILE *in)
:length(frames::read(in)){
	unsigned int seqCount = uInt32::read(in);
	
	for(unsigned int i = 0;i < seqCount;++i){
		addAfter(new SetSequence(in));
		forward();
	}
	
	for(unsigned int i = 0;i < seqCount;++i){
		backward();
	}
}

FILE *fileIO;

void writeSeq(SetSequence *s,unsigned int i,bool isCurrent){
	s->write(fileIO);
}

void Scene::write(FILE *out) const{
	fileIO = out;
	
	frames::write(out,length);
	
	uInt32::write(out,(uint32_t)PointerIterable<SetSequence>::length());
	forAll(&writeSeq);
}

frames::val Scene::getLength() const{
	return length;
}

void Scene::setLength(frames::val newLength){
	length = newLength;
}

// ====================================================================================

Animation::Animation(){
	
}

Animation::Animation(FILE *in){
	// Scenes
	unsigned int scenesCount = uInt32::read(in);
	
	for(unsigned int i = 0;i < scenesCount;++i){
		scenes.addAfter(new Scene(in));
		scenes.forward();
	}
	
	for(unsigned int i = 0;i < scenesCount;++i){
		scenes.backward();
	}
}

void writeScene(Scene *s,unsigned int i,bool isCurrent){
	s->write(fileIO);
}

void Animation::write(FILE *out) const{
	fileIO = out;
	
	uInt32::write(out,(uint32_t)scenes.length());
	scenes.forAll(&writeScene);
}

// ------------------------------------------------------------------------------------

void Animation::sceneAddBefore(frames::val length){
	scenes.addBefore(new Scene(length));
}

void Animation::sceneAddAfter(frames::val length){
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

frames::val Animation::sceneGetLength() const{
	if(scenes.current() == NULL){
		return 0;
	}
	
	return scenes.current()->getLength();
}

void Animation::sceneSetLength(frames::val newLength){
	if(scenes.current() == NULL){
		return;
	}
	
	scenes.current()->setLength(newLength);
}

// -------------------------------

frames::val totalTime;

void sceneSumTime(Scene *s,unsigned int i,bool isCurrent){
	totalTime += s->getLength();
}

frames::val Animation::length() const{
	totalTime = 0;
	scenes.forAll(&sceneSumTime);
	
	return totalTime;
}

// ------------------------------------------------------------------------------------

void Animation::seqAddBefore(unsigned int renderIndex){
	if(scenes.current() == NULL){
		return;
	}
	
	scenes.current()->addBefore(new SetSequence(renderIndex));
}

void Animation::seqAddAfter(unsigned int renderIndex){
	if(scenes.current() == NULL){
		return;
	}
	
	scenes.current()->addAfter(new SetSequence(renderIndex));
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

frames::val tallyTime,completeTime;
unsigned int sceneCount;

int currentMarkerPos;

void drawSceneMarker(Scene *s,unsigned int i,bool isCurrent){
	render::UI::markers::drawScene(
		frames::toFloat(tallyTime) / frames::toFloat(completeTime),
		frames::toFloat(tallyTime + s->getLength()) / frames::toFloat(completeTime),
		isCurrent,
		i + 1 != sceneCount
	);
	
	tallyTime += s->getLength();
}

void drawSequenceMarker(SetSequence *s,unsigned int i,bool isCurrent){
	render::UI::markers::drawSequence(isCurrent,(int)i - currentMarkerPos);
}

void Animation::drawMarkers() const{
	// Scenes
	tallyTime = 0;
	completeTime = length();
	sceneCount = scenes.length();
	
	scenes.forAll(&drawSceneMarker);
	
	// Sequences
	if(scenes.current() != NULL){
		currentMarkerPos = scenes.current()->pos();
		scenes.current()->forAll(&drawSequenceMarker);
	}
}

// -------------------------------

void (**renderFunctions)(const Set &,float);

frames::val renderTime;
normalizedUInt16::val seqTime;

bool stillCurrentSeqFrame;

void drawSeqFrame(SetSequence *s,unsigned int i,bool isCurrent){
	if((isCurrent && stillCurrentSeqFrame && s->bufferCurrent()) || s->bufferInstant(seqTime)){
		renderFunctions[s->getRenderIndex()](s->getBuffer(),frames::toFloat(renderTime));
	}
}

void drawScene(Scene *s,unsigned int i,bool isCurrent){
	if(renderTime >= tallyTime && (renderTime < tallyTime + s->getLength() || (renderTime == completeTime && completeTime == tallyTime + s->getLength()))){
		seqTime = normalizedUInt16::fromFloat(frames::toFloat(renderTime - tallyTime) / frames::toFloat(s->getLength()));
		s->forAll(&drawSeqFrame);
	}
	
	tallyTime += s->getLength();
}

void Animation::render(void (**renderers)(const Set &,float),frames::val time,bool stillActiveFrame) const{
	renderFunctions = renderers;
	
	tallyTime = 0;
	completeTime = length();
	renderTime = time;
	
	stillCurrentSeqFrame = stillActiveFrame;
	
	scenes.forAll(&drawScene);
}
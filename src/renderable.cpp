#include "renderable.hpp"

Renderable::Renderable(unsigned int newRenderIndex)
:renderIndex(newRenderIndex),buffer(){
	
}

Renderable::Renderable(FILE *in)
:renderIndex(uInt32::read(in)),buffer(){
	
}

void Renderable::write(FILE *out) const{
	uInt32::write(out,(uint16_t)renderIndex);
}

unsigned int Renderable::getRenderIndex() const{
	return renderIndex;
}

Set *Renderable::getBuffer(){
	return &buffer;
}
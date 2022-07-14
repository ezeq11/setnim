#include "renderable.hpp"

Renderable::Renderable(unsigned int newRenderIndex)
:renderIndex(newRenderIndex),buffer(){
	
}

Renderable::Renderable(FILE *in)
:renderIndex(uInt32::read(in)),buffer(){
	
}

Renderable *Renderable::write(FILE *out) const{
	uInt32::write(out,(uint16_t)renderIndex);
	
	return this;
}

unsigned int Renderable::getRenderIndex() const{
	return renderIndex;
}

Set *Renderable::getBuffer() const{
	return &buffer;
}
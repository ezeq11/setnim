#ifndef RENDERABLE_INCLUDED
	#include "set.hpp"
	
	class Renderable{
		private:
			unsigned int renderIndex;
			Set buffer;
			
		public:
			Renderable(unsigned int newRenderIndex);
			Renderable(FILE *in);
			Renderable *write(FILE *out) const;
			
			unsigned int getRenderIndex() const;
			Set *getBuffer() const;
	};
	
	#define RENDERABLE_INCLUDED
#endif
#ifndef RENDERABLE_INCLUDED
	#include "set.hpp"
	
	class Renderable{
		private:
			unsigned int renderIndex;
			Set buffer;
			
		public:
			Renderable(unsigned int newRenderIndex);
			Renderable(FILE *in);
			void write(FILE *out) const;
			
			unsigned int getRenderIndex() const;
			Set *getBuffer();
	};
	
	#define RENDERABLE_INCLUDED
#endif
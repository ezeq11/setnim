#ifndef RENDERERS_MAIN_INCLUDED
	#include "set.hpp"
	
	namespace renderers{
		unsigned int count();
		
		void init();
		void (**get())(const Set &,float);
		
		const char *label(unsigned int currentRenderer);
		
		namespace sets{
			void init(unsigned int renderer,Set &s);
			
			const char *label(unsigned int renderer,unsigned int property);
		}
	}
	
	#define RENDERERS_MAIN_INCLUDED
#endif
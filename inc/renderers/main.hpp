#ifndef RENDERERS_MAIN_INCLUDED
	namespace renderers{
		unsigned int count();
		
		const char *label(unsigned int currentRenderer);
		
		namespace sets{
			const char *label(unsigned int renderer,unsigned int property);
		}
	}
	
	#define RENDERERS_MAIN_INCLUDED
#endif
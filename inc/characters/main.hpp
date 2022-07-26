#ifndef CHARACTERS_MAIN_INCLUDED
	namespace chars{
		unsigned int count();
		
		const char *label(unsigned int i);
		
		namespace sets{
			const char *label(unsigned int i,unsigned int j);
		}
	}
	
	#define CHARACTERS_MAIN_INCLUDED
#endif
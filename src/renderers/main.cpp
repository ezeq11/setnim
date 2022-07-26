#include "renderers/main.hpp"

#define RENDERERS_COUNT 3

namespace renderers{
	unsigned int count(){
		return RENDERERS_COUNT;
	}
	
	const char *labels[RENDERERS_COUNT] = {
		"Ghost 1",
		"Ghost 2",
		"Ghost 3"
	};
	
	const char *label(unsigned int currentRenderer){
		return labels[currentRenderer];
	}
	
	namespace sets{
		const char *(*labels[RENDERERS_COUNT])(unsigned int);
		
		const char *label(unsigned int renderer,unsigned int property){
			return "---";
			// return labels[i](j);
		}
	}
}
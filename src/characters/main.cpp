#include "characters/main.hpp"

#define CHARACTERS_COUNT 3

namespace chars{
	unsigned int count(){
		return CHARACTERS_COUNT;
	}
	
	const char *labels[CHARACTERS_COUNT] = {
		"Ghost 1",
		"Ghost 2",
		"Ghost 3"
	};
	
	const char *label(unsigned int i){
		return labels[i];
	}
}
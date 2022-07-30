#include "renderers/main.hpp"

#include "renderers/ghostie.hpp"

#define RENDERERS_COUNT 1

namespace renderers{
	// Variables ------------------------------------------
	void (*inits[RENDERERS_COUNT])() = {
		&(Ghostie::init)
	};
	
	void (*renderers[RENDERERS_COUNT])(const Set &,float) = {
		&(Ghostie::renderer)
	};
	
	const char *labels[RENDERERS_COUNT] = {
		"Ghostie"
	};
	
	// Functions ------------------------------------------
	unsigned int count(){
		return RENDERERS_COUNT;
	}
	
	void init(){
		for(unsigned int i = 0;i < RENDERERS_COUNT;++i){
			inits[i]();
		}
	}
	
	void (**get())(const Set &,float){
		return renderers;
	}
	
	const char *label(unsigned int currentRenderer){
		return labels[currentRenderer];
	}
	
	namespace sets{
		// Variables ------------------------------------------
		void (*inits[RENDERERS_COUNT])(Set &) = {
			&(Ghostie::set::init)
		};
		
		const char *(*labels[RENDERERS_COUNT])(unsigned int) = {
			&(Ghostie::set::label)
		};
		
		// Functions ------------------------------------------
		Set &init(unsigned int renderer,Set &s){
			inits[renderer](s);
			
			return s;
		}
		
		const char *label(unsigned int renderer,unsigned int property){
			const char *received = labels[renderer](property);
			
			return received == NULL ? "---" : received;
		}
	}
}
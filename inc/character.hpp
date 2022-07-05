#ifndef CHARACTER_INCLUDED
	#include "iterable.hpp"
	#include "floatsetsequence.hpp"
	
	struct Character: public FloatSet, public Iterable<FloatSetSequence *,(FloatSetSequence *)NULL>{
		// Randomness Utilities
		static float randNormal();
		static float randCenteredNormal();
		
		// Character Contracts
		Character(unsigned int size,const FloatSet *ref);
		virtual ~Character();
		
		virtual FloatSet *allocFloatSet(FloatSet *ref) = 0;
		virtual void draw(float time) const = 0;
	};
	
	#define CHARACTER_INCLUDED
#endif
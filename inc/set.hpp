#ifndef SET_INCLUDED
	#include "types.hpp"
	
	// Utilities
	#define PI 3.14159265
	
	float randNormal();
	float randCenteredNormal();
	
	// Sets
	#define SET_SIZE 32
	
	class Set{
		protected:
			normalizedInt16::val vals[SET_SIZE];
			
		public:
			Set();
			Set(FILE *in);
			Set *write(FILE *out) const;
			
			float get(unsigned int i) const;
			Set *set(unsigned int i,float newVal);
	};
	
	#define SET_INCLUDED
#endif
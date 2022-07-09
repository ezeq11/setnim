#ifndef RENDERABLE_INCLUDED
	#include <cstdlib>
	
	#include "setsequence.hpp"
	
	class Renderable{
		protected:
			// Randomness Utilities
			static float randNormal(){
				return (float)rand() / (float)RAND_MAX;
			}
			
			static float randCenteredNormal(){
				return randNormal() * 2.0 - 1.0;
			}
			
		public:
			// Renderable Contracts
			virtual ~Renderable(){
				
			}
			
			virtual SetSequence<float> *allocNewSeq() const = 0;
			virtual void draw(float time) const = 0;
	};
	
	#define RENDERABLE_INCLUDED
#endif
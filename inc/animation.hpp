#ifndef ANIMATION_INCLUDED
	#include "character.hpp"
	
	class Animation{
		private:
			Iterable<Character *,(Character *)NULL> characters;
			Iterable<float *,(float *)NULL> sceneLengths;
			
			void charPrepare(Character *newChar);
			
		public:
			void charAddBefore(Character *newChar);
			void charAddAfter(Character *newChar);
			void charForward();
			void charBackward();
			void charRemove();
			void charClear();
			
			void sceneAddBefore(float length);
			void sceneAddAfter(float length);
			void sceneForward();
			void sceneBackward();
			void sceneRemove();
			void sceneClear();
			
			float sceneGetLength();
			void sceneSetLength(float newLength);
			
			const Character *currentCharacter();
			FloatSetSequence *currentSequence();
	};
	
	#define ANIMATION_INCLUDED
#endif
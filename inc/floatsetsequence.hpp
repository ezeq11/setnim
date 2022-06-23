#ifndef FLOATSET_SEQUENCE_INCLUDED
	#include <list>
	
	#include "floatset.hpp"
	
	class FloatSetSequence{
		public:
			enum Transition{
				TRANSITION_X,
				TRANSITION_X_SQUARED,
				TRANSITION_X_CUBED,
				
				TRANSITION_SIN,
				TRANSITION_SIN_SQUARED,
				TRANSITION_SIN_CUBED,
				
				TRANSITION_CIRCLE_CONVEX,
				TRANSITION_CIRCLE_CONCAVE,
				
				TRANSITION_ZERO,
				TRANSITION_ONE,
				
				TRANSITION_COUNT
			};
			
			FloatSetSequence(FloatSet *newBuffer);
			
			void add(float time,FloatSet *set);
			
			FloatSet *select(float time);
			void deselect();
			
			void remove();
			void move(float newTime);
			
			const FloatSet *getInstant(float time);
			
		private:
			class TimedContainer{
				public:
					TimedContainer(float newTime,FloatSet *newSet);
					~TimedContainer();
					
					float getTime() const;
					void setTime(float newTime);
					
					FloatSet *getSet() const;
					
					Transition getTransition(unsigned int i) const;
					void setTransition(unsigned int i,Transition newTransition);
					
					void interpolateFrom(unsigned int i,const TimedContainer &beginning,const TimedContainer &end);
					void interpolateFrom(const TimedContainer &beginning,const TimedContainer &end);
					
				private:
					float time;
					
					FloatSet *set;
					Transition *transitions;
					
					float interpolate(Transition t,float x);
			};
			
			std::list<TimedContainer *> sets;
			std::list<TimedContainer *>::iterator *selected;
			
			TimedContainer buffer;
			
			std::list<TimedContainer *>::iterator find(float time);
	};
	
	#define FLOATSET_SEQUENCE_INCLUDED
#endif
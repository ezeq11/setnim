#ifndef SEQUENCE_INCLUDED
	#include <list>
	
	#include "set.hpp"
	#include "renderable.hpp"
	
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
	
	float interpolate(Transition t,float x);
	
	class SetSequence{
		private:
			class TimedSet: public Set{
				private:
					temporal::val time;
					Transition transitions[SET_SIZE];
					
				public:
					TimedSet(temporal::val newTime);
					TimedSet(FILE *in);
					void write(FILE *out) const;
					
					temporal::val getTime() const;
					TimedSet *setTime(temporal::val newTime);
					
					float get(unsigned int i) const;
					TimedSet *set(unsigned int i,float newVal);
					
					Transition getTransition(unsigned int i) const;
					TimedSet *setTransition(unsigned int i,Transition newTransition);
			};
			
			Renderable *buffer;
			
			std::list<TimedSet *> sets;
			std::list<TimedSet *>::iterator selected;
			
			std::list<TimedSet *>::iterator find(temporal::val time);
			std::list<TimedSet *>::const_iterator find(temporal::val time) const;
			unsigned int find(Renderable **buffers,unsigned int bufferCount,Renderable *sought) const;
		
		public:
			SetSequence(Renderable *newBuffer);
			SetSequence(FILE *in,Renderable **buffers,unsigned int bufferCount);
			void write(FILE *out,Renderable **buffers,unsigned int bufferCount) const;
			
			~SetSequence();
			
			Renderable *getBuffer() const;
			
			void add(temporal::val time);
			
			void select(temporal::val time);
			void deselect();
			Set *current() const;
			
			void remove();
			void move(temporal::val newTime);
			
			void bufferInstant(temporal::val time) const;
			void drawBar(temporal::val begin,temporal::val end) const;
	};
	
	#define SEQUENCE_INCLUDED
#endif
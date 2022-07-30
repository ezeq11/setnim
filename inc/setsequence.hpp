#ifndef SEQUENCE_INCLUDED
	#include <list>
	
	#include "set.hpp"
	
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
	
	const char *labelTransition(Transition t);
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
					
					TimedSet &copyFrom(const TimedSet &t);
					
					temporal::val getTime() const;
					TimedSet &setTime(temporal::val newTime);
					
					float get(unsigned int i) const;
					TimedSet &set(unsigned int i,float newVal);
					
					Transition getTransition(unsigned int i) const;
					TimedSet &setTransition(unsigned int i,Transition newTransition);
			};
			
			unsigned int renderIndex;
			Set buffer;
			
			std::list<TimedSet *> sets;
			std::list<TimedSet *>::iterator selected;
			
			std::list<TimedSet *>::iterator find(temporal::val time);
			std::list<TimedSet *>::const_iterator find(temporal::val time) const;
			
		public:
			SetSequence(unsigned int newRenderIndex);
			SetSequence(FILE *in);
			void write(FILE *out) const;
			
			~SetSequence();
			
			unsigned int getRenderIndex() const;
			const Set &getBuffer() const;
			
			bool bufferInstant(temporal::val time);
			bool bufferCurrent();
			
			
			
			void select(temporal::val time);
			void deselect();
			
			Set *current() const;
			
			Transition getTransition(unsigned int i) const;
			void setTransition(unsigned int i,Transition newTransition);
			
			
			
			void add(temporal::val time);
			void duplicate(temporal::val time);
			
			void remove();
			void move(temporal::val newTime);
			
			void drawMarkers(temporal::val begin,temporal::val end) const;
	};
	
	#define SEQUENCE_INCLUDED
#endif
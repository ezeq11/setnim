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
					normalizedUInt16::val time;
					Transition transitions[SET_SIZE];
					
				public:
					TimedSet(normalizedUInt16::val newTime);
					TimedSet(FILE *in);
					void write(FILE *out) const;
					
					TimedSet &copyFrom(const TimedSet &t);
					
					normalizedUInt16::val getTime() const;
					TimedSet &setTime(normalizedUInt16::val newTime);
					
					float get(unsigned int i) const;
					TimedSet &set(unsigned int i,float newVal);
					
					Transition getTransition(unsigned int i) const;
					TimedSet &setTransition(unsigned int i,Transition newTransition);
			};
			
			unsigned int renderIndex;
			Set buffer;
			
			std::list<TimedSet *> sets;
			std::list<TimedSet *>::iterator selected;
			
			std::list<TimedSet *>::iterator find(normalizedUInt16::val time);
			std::list<TimedSet *>::const_iterator find(normalizedUInt16::val time) const;
			
		public:
			SetSequence(unsigned int newRenderIndex);
			SetSequence(FILE *in);
			void write(FILE *out) const;
			
			~SetSequence();
			
			unsigned int getRenderIndex() const;
			const Set &getBuffer() const;
			
			bool bufferInstant(normalizedUInt16::val time);
			bool bufferCurrent();
			
			
			
			void select(normalizedUInt16::val time);
			void deselect();
			
			Set *current() const;
			
			Transition getTransition(unsigned int i) const;
			void setTransition(unsigned int i,Transition newTransition);
			
			
			
			void add(normalizedUInt16::val time);
			void duplicate(normalizedUInt16::val time);
			
			void remove();
			void move(normalizedUInt16::val newTime);
			
			void drawMarkers(normalizedUInt16::val begin,normalizedUInt16::val end) const;
	};
	
	#define SEQUENCE_INCLUDED
#endif
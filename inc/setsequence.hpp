#ifndef SEQUENCE_INCLUDED
	#include <list>
	#include <cmath>
	
	#include "set.hpp"
	#include "render.hpp"
	
	template <typename type>
	class SetSequence{
		public:
			virtual ~SetSequence(){
				
			}
			
			virtual void add(temporal::val time) = 0;
			
			virtual void select(temporal::val time) = 0;
			virtual void deselect() = 0;
			
			virtual Set<type> *current() const = 0;
			
			virtual void remove() = 0;
			virtual void move(temporal::val newTime) = 0;
			
			virtual void bufferInstant(temporal::val time) = 0;
			virtual void drawBar(temporal::val begin,temporal::val end) = 0;
	};
	
	template <unsigned int size>
	class NormalizedInt16SetSequence: public SetSequence<float>{
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
			
			NormalizedInt16SetSequence(NormalizedInt16Set<size> *newBuffer)
			:sets(),selected(sets.end()),buffer(0,newBuffer){
				
			}
			
			~NormalizedInt16SetSequence(){
				for(typename std::list<TimedContainer *>::iterator it = sets.begin();it != sets.end();++it){
					delete (*it)->getSet();
					delete *it;
				}
			}
			
			void add(temporal::val time){
				sets.insert(find(time),new TimedContainer(time,new NormalizedInt16Set<size>()));
			}
			
			void select(temporal::val time){
				typename std::list<TimedContainer *>::iterator lastSelected = selected;
				typename std::list<TimedContainer *>::iterator it = find(time);
				
				if(it == sets.begin()){
					selected = sets.end();
					
					return;
				}
				
				selected = --it;
				
				if(lastSelected == selected){
					selected = sets.end();
				}
			}
			
			void deselect(){
				selected = sets.end();
			}
			
			Set<float> *current() const{
				return selected == sets.end() ? NULL : (*selected)->getSet();
			}
			
			void remove(){
				if(selected == sets.end()){
					return;
				}
				
				delete (*selected)->getSet();
				delete *selected;
				
				sets.erase(selected);
				selected = sets.end();
			}
			
			void move(temporal::val newTime){
				if(selected == sets.end()){
					return;
				}
				
				TimedContainer *moved = *selected;
				moved->setTime(newTime);
				
				sets.erase(selected);
				typename std::list<TimedContainer *>::iterator newPosition = find(newTime);
				sets.insert(newPosition,moved);
				
				selected = --newPosition;
			}
			
			void bufferInstant(temporal::val time){
				typename std::list<TimedContainer *>::iterator it = find(time);
				
				if(it == sets.begin()){
					return;
				}
				
				buffer.setTime(time);
				TimedContainer *end = *it;
				TimedContainer *begin = *(--it);
				
				buffer.interpolateFrom(*begin,*end);
			}
			
			#define DRAW_ITERATOR() render::drawSequenceBar(it == selected ? render::SEQ_BAR_HIGHLIGHTED : render::SEQ_BAR_NORMAL,temporal::toFloat((*it)->getTime() - begin) / temporal::toFloat(end - begin))
			
			void drawBar(temporal::val begin,temporal::val end){
				typename std::list<TimedContainer *>::iterator it = find(begin);
				
				if(it != sets.begin()){
					--it;
					DRAW_ITERATOR();
					++it;
				}
				
				while(it != sets.end() && (*it)->getTime() < end){
					DRAW_ITERATOR();
					++it;
				}
				
				if(it != sets.end()){
					DRAW_ITERATOR();
				}
			}
			
		private:
			class TimedContainer{
				public:
					TimedContainer(temporal::val newTime,NormalizedInt16Set<size> *newSet)
					:time(newTime),set(newSet),transitions{}{
						
					}
					
					temporal::val getTime() const{
						return time;
					}
					
					void setTime(temporal::val newTime){
						time = newTime;
					}
					
					NormalizedInt16Set<size> *getSet() const{
						return set;
					}
					
					Transition getTransition(unsigned int i) const{
						return transitions[i];
					}
					
					void setTransition(unsigned int i,Transition newTransition){
						transitions[i] = newTransition;
					}
					
					void interpolateFrom(unsigned int i,const TimedContainer &beginning,const TimedContainer &end){
						if(set == NULL){
							return;
						}
						
						float deltaT = temporal::toFloat(time - beginning.time) / temporal::toFloat(end.time - beginning.time);
						float deltaV = end.set->get(i) - beginning.set->get(i);
						
						set->set(i,beginning.set->get(i) + interpolate(transitions[i],deltaT) * deltaV);
					}
					
					void interpolateFrom(const TimedContainer &beginning,const TimedContainer &end){
						if(set == NULL){
							return;
						}
						
						float deltaT,deltaV;
						deltaT = temporal::toFloat(time - beginning.time) / temporal::toFloat(end.time - beginning.time);
						
						for(unsigned int i = 0;i < size;++i){
							deltaV = end.set->get(i) - beginning.set->get(i);
							
							set->set(i,beginning.set->get(i) + interpolate(transitions[i],deltaT) * deltaV);
						}
					}
					
				private:
					temporal::val time;
					
					NormalizedInt16Set<size> *set;
					Transition transitions[size];
					
					float interpolate(Transition t,float x){
						switch(t){
							case TRANSITION_X:
								return x;
								
							case TRANSITION_X_SQUARED:
								return x * x;
								
							case TRANSITION_X_CUBED:
								return x * x * x;
								
							case TRANSITION_SIN:
								x = sin(PI * x / 2.0);
								
								return x;
								
							case TRANSITION_SIN_SQUARED:
								x = sin(PI * x / 2.0);
								
								return x * x;
								
							case TRANSITION_SIN_CUBED:
								x = sin(PI * x / 2.0);
								
								return x * x * x;
								
							case TRANSITION_CIRCLE_CONVEX:
								return sqrt(1.0 - (x - 1.0) * (x - 1.0));
								
							case TRANSITION_CIRCLE_CONCAVE:
								return 1.0 - sqrt(1.0 - x * x);
								
							case TRANSITION_ZERO:
								return 0.0;
								
							case TRANSITION_ONE:
								return 1.0;
								
							case TRANSITION_COUNT:
							default:
								break;
						}
						
						return interpolate((Transition)0,x);
					}
			};
			
			typename std::list<TimedContainer *> sets;
			typename std::list<TimedContainer *>::iterator selected;
			
			TimedContainer buffer;
			
			typename std::list<TimedContainer *>::iterator find(temporal::val time){
				typename std::list<TimedContainer *>::iterator it = sets.begin();
				
				while(it != sets.end() && (*it)->getTime() < time){
					++it;
				}
				
				return it;
			}
	};
	
	#define SEQUENCE_INCLUDED
#endif
#ifndef SEQUENCE_INCLUDED
	#include <list>
	#include <cmath>
	
	#include "set.hpp"
	#include "render.hpp"
	
	template <typename type>
	class SetSequence{
		public:
			virtual ~Sequence(){
				
			}
			
			virtual void add(float time) = 0;
			
			virtual void select(float time) = 0;
			virtual void deselect() = 0;
			
			virtual Set<type> *current() = 0;
			
			virtual void remove() = 0;
			virtual void move(float newTime) = 0;
			
			virtual void bufferInstant(float time) = 0;
	};
	
	template <unsigned int size>
	class FloatSetSequence: public SetSequence<float>{
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
			
			FloatSetSequence(FloatSet<size> *newBuffer)
			:sets(),selected(sets.end()),lastSelected(sets.end()),buffer(0.0,newBuffer){
				
			}
			
			~FloatSetSequence(){
				for(std::list<TimedContainer *>::iterator it = sets.begin();it != sets.end();++it){
					delete (*it)->getSet();
					delete *it;
				}
			}
			
			void add(float time){
				sets.insert(find(time),new TimedContainer(time,new FloatSet<size>()));
			}
			
			void select(float time){
				lastSelected = selected;
				
				std::list<TimedContainer *>::iterator it = find(time);
				
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
			
			void move(float newTime){
				if(selected == sets.end()){
					return;
				}
				
				TimedContainer *moved = *selected;
				moved->setTime(newTime);
				
				sets.erase(selected);
				std::list<TimedContainer *>::iterator newPosition = find(newTime);
				sets.insert(newPosition,moved);
				
				selected = --newPosition;
			}
			
			void bufferInstant(float time){
				std::list<TimedContainer *>::iterator it = find(time);
				
				if(it == sets.begin()){
					return;
				}
				
				buffer.setTime(time);
				TimedContainer *end = *it;
				TimedContainer *begin = *(--it);
				
				buffer.interpolateFrom(*begin,*end);
			}
			
			#define DRAW_ITERATOR() render::drawSequenceBar(it == selected ? render::SEQ_BAR_HIGHLIGHTED : render::SEQ_BAR_NORMAL,((*it)->getTime() - begin) / (end - begin))
			
			void drawBar(float begin,float end){
				std::list<TimedContainer *>::iterator it = find(begin);
				
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
					TimedContainer(float newTime,FloatSet<size> *newSet)
					:time(newTime),set(newSet),transitions({}){
						
					}
					
					float getTime() const{
						return time;
					}
					
					void setTime(float newTime){
						time = newTime;
					}
					
					FloatSet<size> *getSet() const{
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
						
						float deltaT = (time - beginning.time) / (end.time - beginning.time);
						float deltaV = end.set->get(i) - beginning.set->get(i);
						
						set->set(i,beginning.set->get(i) + interpolate(transitions[i],deltaT) * deltaV);
					}
					
					void interpolateFrom(const TimedContainer &beginning,const TimedContainer &end){
						if(set == NULL){
							return;
						}
						
						float deltaT,deltaV;
						deltaT = (time - beginning.time) / (end.time - beginning.time);
						
						for(unsigned int i = 0;i < size;++i){
							deltaV = end.set->get(i) - beginning.set->get(i);
							
							set->set(i,beginning.set->get(i) + interpolate(transitions[i],deltaT) * deltaV);
						}
					}
					
				private:
					float time;
					
					FloatSet<size> *set;
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
			
			std::list<TimedContainer *> sets;
			std::list<TimedContainer *>::iterator selected,lastSelected;
			
			TimedContainer buffer;
			
			std::list<TimedContainer *>::iterator find(float time){
				std::list<TimedContainer *>::iterator it = sets.begin();
				
				while(it != sets.end() && (*it)->getTime() < time){
					++it;
				}
				
				return it;
			}
	};
	
	#define SEQUENCE_INCLUDED
#endif
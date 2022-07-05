#ifndef ITERABLE_INCLUDED
	#include <list>
	#include <iterator>
	
	template <typename T,T nullValue>
	class Iterable{
		private:
			typename std::list<T> iterables;
			typename std::list<T>::iterator p;
			
		public:
			Iterable()
			:iterables(),p(iterables.end()){
				
			}
			
			void forAll(void (*action)(T,unsigned int,bool)){
				unsigned int i = 0;
				
				for(typename std::list<T>::iterator it = iterables.begin();it != iterables.end();++it){
					action(*it,i,it == p);
					++i;
				}
			}
			
			unsigned int length(){
				return iterables.size();
			}
			
			unsigned int pos(){
				return std::distance(iterables.begin(),p);
			}
			
			T current(){
				return p == iterables.end() ? nullValue : *p;
			}
			
			T addBefore(T newIterable){
				iterables.insert(p,newIterable);
				
				if(p == iterables.end()){
					--p;
				}
				
				return current();
			}
			
			T addAfter(T newIterable){
				if(p == iterables.end()){
					return addBefore(newIterable);
				}
				
				++p;
				iterables.insert(p,newIterable);
				--p;
				--p;
				
				return current();
			}
			
			T forward(){
				if(p != iterables.end()){
					++p;
				}
				
				if(p == iterables.end()){
					--p;
				}
				
				return current();
			}
			
			T backward(){
				if(p != iterables.begin()){
					--p;
				}
				
				return current();
			}
			
			T remove(){
				if(p == iterables.end()){
					return current();
				}
				
				p = iterables.erase(p);
				
				if(p == iterables.end() && p != iterables.begin()){
					--p;
				}
				
				return current();
			}
			
			T clear(){
				iterables.clear();
				p = iterables.end();
				
				return current();
			}
	};
	
	#define ITERABLE_INCLUDED
#endif
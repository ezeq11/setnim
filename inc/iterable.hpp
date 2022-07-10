#ifndef ITERABLE_INCLUDED
	#include <list>
	#include <iterator>
	
	template <typename T,T nullValue>
	class Iterable{
		protected:
			typename std::list<T> iterables;
			typename std::list<T>::iterator p;
			
		public:
			Iterable()
			:iterables(),p(iterables.end()){
				
			}
			
			void forAll(void (*action)(T,unsigned int,bool)) const{
				unsigned int i = 0;
				
				for(typename std::list<T>::const_iterator it = iterables.begin();it != iterables.end();++it){
					action(*it,i,it == p);
					++i;
				}
			}
			
			void removeIf(bool (*pred)(T)){
				iterables.remove_if(pred);
			}
			
			unsigned int length() const{
				return iterables.size();
			}
			
			unsigned int pos() const{
				return std::distance(iterables.begin(),(typename std::list<T>::const_iterator)p);
			}
			
			T current() const{
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
	
	template <typename T>
	class PointerIterable: public Iterable<T *,(T *)NULL>{
		using Iterable<T *,(T *)NULL>::iterables;
		using Iterable<T *,(T *)NULL>::p;
		
		public:
			~PointerIterable(){
				for(typename std::list<T *>::iterator it = iterables.begin();it != iterables.end();++it){
					delete *it;
				}
			}
			
			void removeIf(bool (*pred)(T *)){
				for(typename std::list<T *>::iterator it = iterables.begin();it != iterables.end();){
					if(pred(*it)){
						delete *it;
						it = iterables.erase(it);
					}else{
						++it;
					}
				}
			}
			
			T *remove(){
				if(p == iterables.end()){
					return Iterable<T *,(T *)NULL>::current();
				}
				
				delete *p;
				return Iterable<T *,(T *)NULL>::remove();
			}
			
			T *clear(){
				for(typename std::list<T *>::iterator it = iterables.begin();it != iterables.end();++it){
					delete *it;
				}
				
				return Iterable<T *,(T *)NULL>::clear();
			}
	};
	
	#define ITERABLE_INCLUDED
#endif
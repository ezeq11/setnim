#ifndef SET_INCLUDED
	#include "types.hpp"
	
	#define PI 3.14159265
	
	template <typename type>
	class Set{
		public:
			virtual ~Set(){
				
			}
			
			virtual unsigned int count() const = 0;
			
			virtual type get(unsigned int i) const = 0;
			virtual Set<type> *set(unsigned int i,type newVal) = 0;
			
			virtual Set<type> *copyFrom(Set<type> *src) = 0;
	};
	
	template <unsigned int size>
	class NormalizedInt16Set: public Set<float>{
		private:
			normalizedInt16::val vals[size];
			
		public:
			NormalizedInt16Set()
			:vals{}{
				
			}
			
			unsigned int count() const{
				return size;
			}
			
			float get(unsigned int i) const{
				return normalizedInt16::toFloat(vals[i]);
			}
			
			Set<float> *set(unsigned int i,float newVal){
				vals[i] = normalizedInt16::fromFloat(newVal);
				
				return (Set<float> *)this;
			}
			
			Set<float> *copyFrom(Set<float> *src){
				for(unsigned int i = 0;i < size;++i){
					set(i,src->get(i));
				}
				
				return (Set<float> *)this;
			}
	};
	
	#define SET_INCLUDED
#endif
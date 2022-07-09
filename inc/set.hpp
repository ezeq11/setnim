#ifndef SET_INCLUDED
	#define PI 3.14159265
	
	template <typename type>
	class Set{
		public:
			virtual ~Set(){
				
			}
			
			virtual unsigned int count() = 0;
			
			virtual type get(unsigned int i) = 0;
			virtual Set<type> *set(unsigned int i,type newVal) = 0;
			
			virtual Set<type> *copyFrom(Set<type> *src) = 0;
	};
	
	template <unsigned int size>
	class FloatSet: public Set<float>{
		private:
			float vals[size];
			
		public:
			FloatSet()
			:vals({}){
				
			}
			
			unsigned int count(){
				return size;
			}
			
			float get(unsigned int i){
				return vals[i];
			}
			
			Set<float> *set(unsigned int i,float newVal){
				vals[i] = newVal;
				
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
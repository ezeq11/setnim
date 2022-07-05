#ifndef FLOATSET_INCLUDED
	#define PI 3.14159265
	
	class FloatSet{
		public:
			FloatSet(unsigned int size,const FloatSet *ref);
			~FloatSet();
			
			unsigned int size() const;
			
			float get(unsigned int i) const;
			void set(unsigned int i,float newVal);
			
		private:
			unsigned int count;
			float *vals;
	};
	
	#define FLOATSET_INCLUDED
#endif
#ifndef FLOATSET_INCLUDED
	#define PI 3.14159265
	
	class FloatSet{
		public:
			// Randomness
			static float randNormal();
			static float randCenteredNormal();
			
			// State Modification
			FloatSet(unsigned int size);
			FloatSet(unsigned int size,const FloatSet *ref);
			~FloatSet();
			
			unsigned int size() const;
			
			float get(unsigned int i) const;
			void set(unsigned int i,float newVal);
			
			virtual void draw(float x,float y,float size,float time) const = 0;
			
		private:
			unsigned int count;
			float *vals;
	};
	
	#define FLOATSET_INCLUDED
#endif
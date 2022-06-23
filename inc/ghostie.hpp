#ifndef GHOSTIE_INCLUDED
	#include "floatset.hpp"
	
	class Ghostie: public FloatSet{
		public:
			enum Val{
				BODY_SPEED,
				
				SHELL_SCALE,
				SHELL_ROT,
				
				EYELID_LL_POS,
				EYELID_LU_POS,
				EYELID_LU_ROT,
				
				EYELID_RL_POS,
				EYELID_RU_POS,
				EYELID_RU_ROT,
				
				FACE_DIST,
				FACE_ROT,
				
				HAND_L_POS,
				HAND_L_ROT,
				
				HAND_R_POS,
				HAND_R_ROT,
				
				VAL_COUNT
			};
			
			enum RandVal{
				RAND_EYE_L_POS,
				RAND_EYE_R_POS,
				
				RAND_EYELID_LL_ROT,
				RAND_EYELID_LU_ROT,
				RAND_EYELID_RL_ROT,
				RAND_EYELID_RU_ROT,
				
				RAND_VAL_COUNT
			};
			
			enum BodyStyle{
				BODY_NORMAL,
				BODY_SOLID,
				BODY_POINTY,
				
				BODY_STYLE_COUNT
			};
			
			enum ParticleAttrib{
				PARTICLE_RADIUS,
				PARTICLE_THETA,
				PARTICLE_PHASE_OFFSET,
				
				PARTICLE_ATTRIB_COUNT
			};
						
			static const unsigned int PARTICLE_COUNT = 50;
			
			Ghostie();
			void draw(float x,float y,float size,float time) const;
			
		private:
			float randVals[RAND_VAL_COUNT];
			float particles[PARTICLE_COUNT * PARTICLE_ATTRIB_COUNT];
			
			float getParticleAttrib(unsigned int i,ParticleAttrib attrib);
			void setParticleAttrib(unsigned int i,ParticleAttrib attrib,float newVal);
	};
	
	#define GHOSTIE_INCLUDED
#endif
#include <cmath>

#include "ghostie.hpp"

Ghostie::Ghostie()
:FloatSet(VAL_COUNT){
	randVals[RAND_EYE_L_POS] = randCenteredNormal() / 20.0;
	randVals[RAND_EYE_R_POS] = randCenteredNormal() / 20.0;
	
	randVals[RAND_EYELID_LL_ROT] = randCenteredNormal() * 5.0;
	randVals[RAND_EYELID_LU_ROT] = randCenteredNormal() * 5.0;
	
	randVals[RAND_EYELID_RL_ROT] = randCenteredNormal() * 5.0;
	randVals[RAND_EYELID_RU_ROT] = randCenteredNormal() * 5.0;
	
	for(unsigned int i = 0;i < PARTICLE_COUNT;++i){
		setParticleAttrib(i,PARTICLE_RADIUS,0.5 + 0.5 * sqrt(randNormal()));
		setParticleAttrib(i,PARTICLE_THETA,randNormal() * 2.0 * PI);
		setParticleAttrib(i,PARTICLE_PHASE_OFFSET,randCenteredNormal() * 2.0 * PI);
	}
}

void Ghostie::draw(float x,float y,float size,float time) const{
	
}

float Ghostie::getParticleAttrib(unsigned int i,ParticleAttrib attrib){
	return particles[i * PARTICLE_ATTRIB_COUNT + attrib];
}

void Ghostie::setParticleAttrib(unsigned int i,ParticleAttrib attrib,float newVal){
	particles[i * PARTICLE_ATTRIB_COUNT + attrib] = newVal;
}
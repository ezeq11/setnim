#ifndef RENDER_INCLUDED
	#include <SFML/Graphics.hpp>
	
	namespace render{
		bool init(sf::RenderWindow &renderWin,const sf::Vector2u &innerDim);
		void resize();
		
		#define GHOSTIE_PARTICLE_COUNT 50
		
		enum ghostieBody{
			BODY_NORMAL,
			BODY_SOLID,
			BODY_POINTY,
			
			BODY_COUNT
		};
		
		struct ghostiePose{
			// Body
			ghostieBody bodyStyle;
			float bodySpeed;
			float p[GHOSTIE_PARTICLE_COUNT * 3];
			
			float shellScale;
			float shellRotation;
			
			// Eyes
			float ll;
			float lup,lur;
			
			float rl;
			float rup,rur;
			
			// Randoms
			float _lpx,_rpx;
			float _llr,_lur;
			float _rlr,_rur;
			
			// Face
			float fd,fr;
			
			// Hands
			bool showHands;
			float hlp,hlr;
			float hrp,hrr;
			
			// Constructor
			ghostiePose();
		};
		
		void ghostie(float x,float y,float size,ghostiePose &pose,float time);
	}
	
	#define RENDER_INCLUDED
#endif
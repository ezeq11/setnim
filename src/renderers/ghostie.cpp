#include <cmath>

#include "renderers/ghostie.hpp"
#include "render.hpp"

namespace Ghostie{
	sf::CircleShape eye(1.0,64);
	sf::RectangleShape upperEyeLid(sf::Vector2f(1.0,1.0));
	sf::RectangleShape lowerEyeLid(sf::Vector2f(1.0,1.0));
	
	sf::Shape *body;
	sf::CircleShape bodyNormal(sqrt(4.0 / PI),64);
	sf::RectangleShape bodySolid(sf::Vector2f(2.0,2.0));
	sf::ConvexShape bodyPointy(4);
	
	void init(){
		eye.setOrigin(sf::Vector2f(1.0,1.0));
		eye.setFillColor(sf::Color(30,30,30,0xff));
		
		upperEyeLid.setOrigin(sf::Vector2f(0.5,1.0));
		upperEyeLid.setFillColor(sf::Color(0,0,0,0));
		
		lowerEyeLid.setOrigin(sf::Vector2f(0.5,0.0));
		lowerEyeLid.setFillColor(sf::Color(0,0,0,0));
		
		bodyNormal.setOrigin(sf::Vector2f(sqrt(4.0 / PI),sqrt(4.0 / PI)));
		bodySolid.setOrigin(sf::Vector2f(1.0,1.0));
		
		bodyPointy.setPoint(0,sf::Vector2f(-sqrt(2.0),0.0));
		bodyPointy.setPoint(1,sf::Vector2f(0.0,sqrt(2.0)));
		bodyPointy.setPoint(2,sf::Vector2f(sqrt(2.0),0.0));
		bodyPointy.setPoint(3,sf::Vector2f(0.0,-sqrt(2.0)));
		
		body = &bodyNormal;
	}
	
	Char charlie;
	
	void renderer(const Set &s,float time){
		charlie.draw(s,time);
	}
	
	namespace set{
		void init(Set &s){
			s.set(SIZE,0.5);
		}
		
		const char *label(unsigned int i){
			switch((Val)i){
				case X:
					return "x";
				case Y:
					return "y";
				case SIZE:
					return "size";
				case BODY_SPEED:
					return "body speed";
				case SHELL_SCALE:
					return "shell scale";
				case SHELL_ROT:
					return "shell rotation";
				case EYELID_LL_POS:
					return "eyelid ll position";
				case EYELID_LU_POS:
					return "eyelid lu position";
				case EYELID_LU_ROT:
					return "eyelid lu rotation";
				case EYELID_RL_POS:
					return "eyelid rl position";
				case EYELID_RU_POS:
					return "eyelid ru position";
				case EYELID_RU_ROT:
					return "eyelid ru rotation";
				case FACE_DIST:
					return "face distance";
				case FACE_ROT:
					return "face rotation";
				case HAND_L_POS:
					return "hand l position";
				case HAND_L_ROT:
					return "hand l rotation";
				case HAND_R_POS:
					return "hand r position";
				case HAND_R_ROT:
					return "hand r rotation";
				default:
					break;
			}
			
			return NULL;
		}
	}
	
	float Char::getR(unsigned int i) const{
		return randVals[i];
	}
	
	float Char::getParticleAttrib(unsigned int i,ParticleAttrib attrib) const{
		return particles[i * PARTICLE_ATTRIB_COUNT + attrib];
	}
	
	void Char::setParticleAttrib(unsigned int i,ParticleAttrib attrib,float newVal){
		particles[i * PARTICLE_ATTRIB_COUNT + attrib] = newVal;
	}
	
	Char::Char()
	:style(PARTICLE_STYLE_NORMAL){
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
	
	ParticleStyle Char::getStyle() const{
		return style;
	}
	
	void Char::setStyle(ParticleStyle newStyle){
		style = newStyle;
	}
	
	void Char::draw(const Set &s,float time) const{
		using namespace render;
		
		float rW = render::innerDimensions().x;
		float rH = render::innerDimensions().y;
		
		float x = s.get(X) * rW / 2.0;
		float y = s.get(Y) * rH / 2.0;
		float size = s.get(SIZE) * (rW < rH ? rW : rH);
		
		switch(style){
			case PARTICLE_STYLE_NORMAL:
				body = &bodyNormal;
				
				break;
			case PARTICLE_STYLE_SOLID:
				body = &bodySolid;
				
				break;
			case PARTICLE_STYLE_POINTY:
				body = &bodyPointy;
				
				break;
			case PARTICLE_STYLE_COUNT:
			default:
				break;
		}
		
		// Hands ------------------------------
		body->setFillColor(sf::Color(225,225,225,0xff));
		body->setScale((size * 0.75 / 4.0) / 2.0,(size / 4.0) / 2.0);
		
		view::draw(*body,sf::Transform().rotate(s.get(HAND_L_ROT) * 360.0).translate(x - size / 8.0,y - s.get(HAND_L_POS) * size * 0.75));
		view::draw(*body,sf::Transform().rotate(s.get(HAND_R_ROT) * 360.0).translate(x + size / 8.0,y - s.get(HAND_R_POS) * size * 0.75));
		
		// Body -------------------------------
		// bodyNormal.setFillColor(sf::Color(150,150,150,75.0));
		// view.draw(bodyNormal,sf::RenderStates(sf::Transform().translate(x,y).scale(size / 3.0,size / 3.0)));
		
		body->setFillColor(sf::Color(225,225,225,0xff));
		body->setScale(1.0,1.0);
		
		view::draw(*body,sf::Transform().translate(x,y).scale(size * s.get(SHELL_SCALE) / 2.0,size * s.get(SHELL_SCALE) / 2.0).rotate(s.get(SHELL_ROT) * 360.0));
		
		float pd,px,py;
		float factor,col;
		
		for(unsigned int i = 0;i < PARTICLE_COUNT;++i){
			pd = getParticleAttrib(i,PARTICLE_RADIUS) * sin(time * s.get(BODY_SPEED) * 1.0 + getParticleAttrib(i,PARTICLE_PHASE_OFFSET));
			col = 200.0 + fabs(pd) * 50.0;
			
			px = pd * cos(getParticleAttrib(i,PARTICLE_THETA)); // + sin(time + pose.p[i * 2 + 1] * 2.0) / 10.0;
			py = pd * sin(getParticleAttrib(i,PARTICLE_THETA)); // + cos(time + pose.p[i * 2 + 1] * 2.0) / 10.0;
			
			
			/*if(i < 15){
				factor = 5.0;
			}else if(i < 25){
				factor = 6.0;
			}else{
				factor = 7.5;
			}*/
			factor = 4.0 + 2.5 * (float)i / (float)PARTICLE_COUNT;
			
			body->setFillColor(sf::Color(col,col,col,0xff));
			view::draw(*body,sf::Transform().translate(x + (px * size / 2.0),y + (py * size / 2.0)).scale(size * 0.75 * (1.0 - 0.5 * fabs(pd)) / factor,size * (1.0 - 0.5 * fabs(pd)) / factor));//.rotate(time * 180.0)));
		}
		
		// view.draw(body,sf::RenderStates(sf::Transform().translate(x,y).scale(size / 2.0,size / 2.0)));
		
		// Eye Expressions --------------------
		float eyeHeight = size / 3.0;
		float eyeWidth = eyeHeight / 2.0;
		
		float eyeLeftX = ((eyeWidth * -2.3 / 2.0) * (0.9 + fabs(cos(s.get(FACE_DIST) * PI / 2.0)) * 0.1)) + (getR(RAND_EYE_L_POS) * eyeWidth);
		float eyeRightX = ((eyeWidth * 2.3 / 2.0) * (0.9 + fabs(cos(s.get(FACE_DIST) * PI / 2.0)) * 0.1)) + (getR(RAND_EYE_R_POS) * eyeWidth);
		
		view::temp::clear();
		
		// Eyes
		eye.setScale(sf::Vector2f(eyeWidth / 2.0,eyeHeight / 2.0));
		
		view::temp::draw(eye,sf::Transform().translate(eyeLeftX,0.0));
		view::temp::draw(eye,sf::Transform().translate(eyeRightX,0.0));
		
		// Lower Eyelids
		lowerEyeLid.setScale(sf::Vector2f(eyeWidth,eyeHeight / 1.5));
		
		view::temp::draw(lowerEyeLid,sf::BlendNone,sf::Transform().translate(eyeLeftX,(eyeHeight / 2.0) * (1.0 - s.get(EYELID_LL_POS))).rotate(getR(RAND_EYELID_LL_ROT)));
		view::temp::draw(lowerEyeLid,sf::BlendNone,sf::Transform().translate(eyeRightX,(eyeHeight / 2.0) * (1.0 - s.get(EYELID_RL_POS))).rotate(getR(RAND_EYELID_RL_ROT)));
		
		// Upper Eyelids
		upperEyeLid.setScale(sf::Vector2f(eyeWidth * 2.0,eyeHeight / 1.5));
		
		view::temp::draw(upperEyeLid,sf::BlendNone,sf::Transform().translate(eyeLeftX,(-eyeHeight / 2.0) * (1.0 - s.get(EYELID_LU_POS))).rotate(s.get(EYELID_LU_ROT) * 360.0 + getR(RAND_EYELID_LU_ROT)));
		view::temp::draw(upperEyeLid,sf::BlendNone,sf::Transform().translate(eyeRightX,(-eyeHeight / 2.0) * (1.0 - s.get(EYELID_RU_POS))).rotate(s.get(EYELID_RU_ROT) * 360.0 + getR(RAND_EYELID_RU_ROT)));
		
		// Face -------------------------------
		view::drawTemp(sf::Transform().translate(x,y).rotate(s.get(FACE_ROT) * 360.0).translate(size * 0.75 * s.get(FACE_DIST) / 2.0,0.0).rotate(-s.get(FACE_ROT) * 360.0 / 1.25));
	}
}
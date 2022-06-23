#include <cstdlib>
#include <cmath>

#include "render.hpp"

#define PI 3.14159265

// Window
sf::RenderWindow *renderWindow;

// View
sf::Vector2u innerDimension;
sf::RenderTexture view,viewTemp;
sf::Sprite viewSprite,viewTempSprite;

sf::RectangleShape viewBounds;

#define VIEW_TOP_BOTTOM_PADDING 100

// Ghostie
sf::CircleShape eye(1.0,64);
sf::RectangleShape upperEyeLid(sf::Vector2f(1.0,1.0));
sf::RectangleShape lowerEyeLid(sf::Vector2f(1.0,1.0));

sf::Shape *body;
sf::CircleShape bodyNormal(sqrt(4.0 / PI),64);
sf::RectangleShape bodySolid(sf::Vector2f(2.0,2.0));
sf::ConvexShape bodyPointy(4);

bool render::init(sf::RenderWindow &renderWin,const sf::Vector2u &innerDim){
	// Window --------------------------
	renderWindow = &renderWin;
	
	// View ----------------------------
	innerDimension = innerDim;
	
	if(!view.create(innerDimension.x,innerDimension.y)){
		return false;
	}
	
	if(!viewTemp.create(innerDimension.x,innerDimension.y)){
		return false;
	}
	
	view.setSmooth(true);
	
	sf::View innerView = sf::View(sf::Vector2f(0.0,0.0),sf::Vector2f(innerDimension));
	
	view.setView(innerView);
	viewTemp.setView(innerView);
	
	viewSprite.setTexture(view.getTexture(),true);
	viewSprite.setOrigin(sf::Vector2f(view.getSize() / 2u));
	
	viewTempSprite.setTexture(viewTemp.getTexture(),true);
	viewTempSprite.setOrigin(sf::Vector2f(viewTemp.getSize() / 2u));
	
	resize();
	
	sf::Vector2f viewBoundsSize = sf::Vector2f(innerDimension.x - 10,innerDimension.y - 10);
	
	viewBounds.setSize(viewBoundsSize);
	viewBounds.setOrigin(viewBoundsSize / 2.0f);
	
	viewBounds.setFillColor(sf::Color(0,0,0,0));
	viewBounds.setOutlineColor(sf::Color(0,0,0,0xff));
	viewBounds.setOutlineThickness(5.0);
	
	// Ghostie ---------------------------
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
	
	return true;
}

void render::resize(){
	float scale = (renderWindow->getView().getSize().y - VIEW_TOP_BOTTOM_PADDING) / innerDimension.y;
	
	viewSprite.setScale(sf::Vector2f(scale,scale));
}

float randNormal(){
	return (float)rand() / (float)RAND_MAX;
}

float randNegNormal(){
	return (randNormal() - 0.5) * 2.0;
}

render::ghostiePose::ghostiePose()
:bodyStyle(BODY_NORMAL),bodySpeed(1.0),shellScale(0.0),shellRotation(0.0),ll(0.0),lup(0.0),lur(0.0),rl(0.0),rup(0.0),rur(0.0),fd(0.0),fr(0.0),showHands(true),hlp(1.0),hlr(-5.0),hrp(1.0),hrr(5.0){
	float r,theta;
	
	for(unsigned int i = 0;i < GHOSTIE_PARTICLE_COUNT;++i){
		p[i * 3 + 0] = 0.5 + 0.5 * sqrt(randNormal());
		p[i * 3 + 1] = randNormal() * 2 * PI;
		p[i * 3 + 2] = randNegNormal() * 2 * PI;
	}
	
	
	_lpx = randNegNormal() / 20.0;
	_rpx = randNegNormal() / 20.0;
	
	_llr = randNegNormal() * 5.0;
	_lur = randNegNormal() * 5.0;
	
	_rlr = randNegNormal() * 5.0;
	_rur = randNegNormal() * 5.0;
}

void render::ghostie(float x,float y,float size,ghostiePose &pose,float time){
	view.clear(sf::Color(0,0,0,0));
	
	switch(pose.bodyStyle){
		case BODY_NORMAL:
			body = &bodyNormal;
			
			break;
		case BODY_SOLID:
			body = &bodySolid;
			
			break;
		case BODY_POINTY:
			body = &bodyPointy;
			
			break;
		case BODY_COUNT:
			break;
	}
	
	// Hands ------------------------------
	if(pose.showHands){
		body->setFillColor(sf::Color(225,225,225,0xff));
		body->setScale((size * 0.75 / 4.0) / 2.0,(size / 4.0) / 2.0);
		
		view.draw(*body,sf::RenderStates(sf::Transform().rotate(pose.hlr).translate(x - size / 8.0,y - pose.hlp * size * 0.75)));
		view.draw(*body,sf::RenderStates(sf::Transform().rotate(pose.hrr).translate(x + size / 8.0,y - pose.hrp * size * 0.75)));
	}
	
	// Body -------------------------------
	// bodyNormal.setFillColor(sf::Color(150,150,150,75.0));
	// view.draw(bodyNormal,sf::RenderStates(sf::Transform().translate(x,y).scale(size / 3.0,size / 3.0)));
	
	body->setFillColor(sf::Color(225,225,225,0xff));
	body->setScale(1.0,1.0);
	
	view.draw(*body,sf::RenderStates(sf::Transform().translate(x,y).scale(size * pose.shellScale / 2.0,size * pose.shellScale / 2.0).rotate(pose.shellRotation)));
	
	float pd,px,py;
	float factor,col;
	
	for(unsigned int i = 0;i < GHOSTIE_PARTICLE_COUNT;++i){
		pd = pose.p[i * 3 + 0] * sin(time * pose.bodySpeed * 1.0 + pose.p[i * 3 + 2]);
		col = 200.0 + fabs(pd) * 50.0;
		
		px = pd * cos(pose.p[i * 3 + 1]); // + sin(time + pose.p[i * 2 + 1] * 2.0) / 10.0;
		py = pd * sin(pose.p[i * 3 + 1]); // + cos(time + pose.p[i * 2 + 1] * 2.0) / 10.0;
		
		
		/*if(i < 15){
			factor = 5.0;
		}else if(i < 25){
			factor = 6.0;
		}else{
			factor = 7.5;
		}*/
		factor = 4.0 + 2.5 * (float)i / (float)GHOSTIE_PARTICLE_COUNT;
		
		body->setFillColor(sf::Color(col,col,col,0xff));
		view.draw(*body,sf::RenderStates(sf::Transform().translate(x + (px * size / 2.0),y + (py * size / 2.0)).scale(size * 0.75 * (1.0 - 0.5 * fabs(pd)) / factor,size * (1.0 - 0.5 * fabs(pd)) / factor)));//.rotate(time * 180.0)));
		
		//pose.p[i * 2 + 1] += 0.01;
		//pose.p[i * 2 + 0] += sin(px) * 0.01;
	}
	
	// view.draw(body,sf::RenderStates(sf::Transform().translate(x,y).scale(size / 2.0,size / 2.0)));
	
	// Eye Expressions --------------------
	float eyeHeight = size / 3.0;
	float eyeWidth = eyeHeight / 2.0;
	
	float eyeLeftX = ((eyeWidth * -2.3 / 2.0) * (0.9 + fabs(cos(pose.fd * PI / 2.0)) * 0.1)) + (pose._lpx * eyeWidth);
	float eyeRightX = ((eyeWidth * 2.3 / 2.0) * (0.9 + fabs(cos(pose.fd * PI / 2.0)) * 0.1)) + (pose._rpx * eyeWidth);
	
	viewTemp.clear(sf::Color(0,0,0,0));
	
	// Eyes
	eye.setScale(sf::Vector2f(eyeWidth / 2.0,eyeHeight / 2.0));
	
	viewTemp.draw(eye,sf::RenderStates(sf::Transform().translate(eyeLeftX,0.0)));
	viewTemp.draw(eye,sf::RenderStates(sf::Transform().translate(eyeRightX,0.0)));
	
	// Lower Eyelids
	lowerEyeLid.setScale(sf::Vector2f(eyeWidth,eyeHeight / 1.5));
	
	viewTemp.draw(lowerEyeLid,sf::RenderStates(sf::BlendNone,sf::Transform().translate(eyeLeftX,(eyeHeight / 2.0) * (1.0 - pose.ll)).rotate(pose._llr),NULL,NULL));
	viewTemp.draw(lowerEyeLid,sf::RenderStates(sf::BlendNone,sf::Transform().translate(eyeRightX,(eyeHeight / 2.0) * (1.0 - pose.rl)).rotate(pose._rlr),NULL,NULL));
	
	// Upper Eyelids
	upperEyeLid.setScale(sf::Vector2f(eyeWidth * 2.0,eyeHeight / 1.5));
	
	viewTemp.draw(upperEyeLid,sf::RenderStates(sf::BlendNone,sf::Transform().translate(eyeLeftX,(-eyeHeight / 2.0) * (1.0 - pose.lup)).rotate(pose.lur + pose._lur),NULL,NULL));
	viewTemp.draw(upperEyeLid,sf::RenderStates(sf::BlendNone,sf::Transform().translate(eyeRightX,(-eyeHeight / 2.0) * (1.0 - pose.rup)).rotate(pose.rur + pose._rur),NULL,NULL));
	
	// Face -------------------------------
	view.draw(viewTempSprite,sf::RenderStates(sf::Transform().translate(x,y).rotate(pose.fr).translate(size * 0.75 * pose.fd / 2.0,0.0).rotate(-pose.fr / 1.25)));
	
	// Final Expression
	view.draw(viewBounds);
	
	renderWindow->draw(viewSprite);
}
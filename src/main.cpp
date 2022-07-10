#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <ctime>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "render.hpp"
#include "ghostie.hpp"
#include "setsequence.hpp"
#include "timeView.hpp"
#include "animation.hpp"

#define PI 3.14159265
#define GOLDEN_RATIO 1.618033

#define WINDOW_SCREEN_HEIGHT_RATIO 0.6
#define WINDOW_SCREEN_WIDTH_RATIO 0.8

enum Expression{
	EXP_BLANK,
	EXP_ANGRY,
	EXP_WORRIED,
	EXP_BORED,
	EXP_CHEERFUL,
	
	EXP_COUNT
};

Ghostie::Char expresses[EXP_COUNT];

sf::RenderWindow window;

void resizeContext(float width,float height){
	window.setView(sf::View(sf::Vector2f(0.0,0.0),sf::Vector2f(width,height)));
	render::resize();
}

int main(){
	srand(time(NULL));
	
	// Window --------------------------------------------
	sf::ContextSettings glSettings;
	glSettings.antialiasingLevel = 4;
	glSettings.majorVersion = 3;
	glSettings.minorVersion = 3;
	
	unsigned int screenWidth = sf::VideoMode::getDesktopMode().width;
	unsigned int screenHeight = sf::VideoMode::getDesktopMode().height;
	
	unsigned int windowWidth = (float)screenWidth * WINDOW_SCREEN_WIDTH_RATIO;
	unsigned int windowHeight = (float)screenHeight * WINDOW_SCREEN_HEIGHT_RATIO;
	
	if(windowWidth < windowHeight * GOLDEN_RATIO){
		windowHeight = (float)windowHeight / GOLDEN_RATIO;
		
	}else{
		windowWidth = (float)windowHeight * GOLDEN_RATIO;
	}
	
	window.create(sf::VideoMode(windowWidth,windowHeight),"Ghostie Tiem",sf::Style::Default,glSettings);
	
	// Execution -----------------------------------------
	timeView::init();
	render::init(window,sf::Vector2u(1920,1080));
	Ghostie::init();
	
	resizeContext(windowWidth,windowHeight);
	
	bool run = true;
	
	sf::Event event;
	bool isCtrlDown,isAltDown,isShiftDown;
	
	float timeViewCursor;
	
	Animation anim;
	
	// unsigned int currentProperty = 0;
	
	
	
	sf::Clock clock;
	sf::Time transfer = clock.getElapsedTime();
	
	Expression lastExp = EXP_BLANK;
	Expression currExp = EXP_BLANK;
	
	Ghostie::Char pose;
	
	pose.set(Ghostie::X,0.0);
	pose.set(Ghostie::Y,0.0);
	pose.set(Ghostie::SIZE,500.0);
	
	expresses[EXP_ANGRY].set(Ghostie::EYELID_LU_POS,0.3);
	expresses[EXP_ANGRY].set(Ghostie::EYELID_LU_ROT,-30.0);
	expresses[EXP_ANGRY].set(Ghostie::EYELID_RU_POS,0.3);
	expresses[EXP_ANGRY].set(Ghostie::EYELID_RU_ROT,30.0);
	
	expresses[EXP_WORRIED].set(Ghostie::EYELID_LU_POS,0.3);
	expresses[EXP_WORRIED].set(Ghostie::EYELID_LU_ROT,30.0);
	expresses[EXP_WORRIED].set(Ghostie::EYELID_RU_POS,0.3);
	expresses[EXP_WORRIED].set(Ghostie::EYELID_RU_ROT,-30.0);
	
	expresses[EXP_BORED].set(Ghostie::EYELID_LU_POS,0.2);
	expresses[EXP_BORED].set(Ghostie::EYELID_RU_POS,0.2);
	
	expresses[EXP_CHEERFUL].set(Ghostie::EYELID_LL_POS,0.2);
	expresses[EXP_CHEERFUL].set(Ghostie::EYELID_RL_POS,0.2);
	
	float faceDist,faceRot;
	
	while(window.isOpen() && run){
		timeViewCursor = (float)sf::Mouse::getPosition(window).x / (float)window.getView().getSize().x;
		
		// Drawing ---------------------------------------
		window.clear(sf::Color(0,0,0,0xff));
		render::view::clear();
		
		// float pace = sin(clock.getElapsedTime().asSeconds());
		faceDist = ((float)sf::Mouse::getPosition(window).x / (float)window.getSize().x) * 2.0 - 1.0;
		faceDist = faceDist < -1.0 ? -1.0 : (faceDist > 1.0 ? 1.0 : faceDist);
		
		faceRot = ((float)sf::Mouse::getPosition(window).y / (float)window.getSize().y) * 2.0 - 1.0;
		faceRot = faceRot < -1.0 ? -1.0 : (faceRot > 1.0 ? 1.0 : faceRot);
		faceRot *= 90.0;
		
		if(faceDist > 0.0){
			faceRot *= -1.0;
		}
		
		pose.set(Ghostie::FACE_DIST,faceDist);
		pose.set(Ghostie::FACE_ROT,faceRot);
		
		float delta = clock.getElapsedTime().asSeconds() - transfer.asSeconds();
		
		if(delta < 0.15){
			pose.set(Ghostie::EYELID_LL_POS,expresses[lastExp].get(Ghostie::EYELID_LL_POS) + ((expresses[currExp].get(Ghostie::EYELID_LL_POS) - expresses[lastExp].get(Ghostie::EYELID_LL_POS)) * (delta / 0.15)));
			pose.set(Ghostie::EYELID_LU_POS,expresses[lastExp].get(Ghostie::EYELID_LU_POS) + ((expresses[currExp].get(Ghostie::EYELID_LU_POS) - expresses[lastExp].get(Ghostie::EYELID_LU_POS)) * (delta / 0.15)));
			pose.set(Ghostie::EYELID_LU_ROT,expresses[lastExp].get(Ghostie::EYELID_LU_ROT) + ((expresses[currExp].get(Ghostie::EYELID_LU_ROT) - expresses[lastExp].get(Ghostie::EYELID_LU_ROT)) * (delta / 0.15)));
			
			pose.set(Ghostie::EYELID_RL_POS,expresses[lastExp].get(Ghostie::EYELID_RL_POS) + ((expresses[currExp].get(Ghostie::EYELID_RL_POS) - expresses[lastExp].get(Ghostie::EYELID_RL_POS)) * (delta / 0.15)));
			pose.set(Ghostie::EYELID_RU_POS,expresses[lastExp].get(Ghostie::EYELID_RU_POS) + ((expresses[currExp].get(Ghostie::EYELID_RU_POS) - expresses[lastExp].get(Ghostie::EYELID_RU_POS)) * (delta / 0.15)));
			pose.set(Ghostie::EYELID_RU_ROT,expresses[lastExp].get(Ghostie::EYELID_RU_ROT) + ((expresses[currExp].get(Ghostie::EYELID_RU_ROT) - expresses[lastExp].get(Ghostie::EYELID_RU_ROT)) * (delta / 0.15)));
		}
		
		pose.draw(clock.getElapsedTime().asSeconds());
		
		// Done
		render::drawView();
		
		render::drawUIBackground(timeView::getBegin(),timeView::getEnd());
		
		anim.drawMarkers();
		
		if(anim.seqCurrent() != NULL){
			anim.seqCurrent()->drawBar(timeView::getBegin(),timeView::getEnd());
			render::drawSequenceBar(render::SEQ_BAR_CURSOR,timeViewCursor);
		}
		
		window.display();
		
		// Event Handling --------------------------------
		if(window.pollEvent(event)){
			isCtrlDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
			isAltDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);
			isShiftDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
			
			// if(anim.seqCurrent())
			
			switch(event.type){
				case sf::Event::Closed:
					run = false;
					
					break;
				case sf::Event::Resized:
					resizeContext(event.size.width,event.size.height);
					
					break;
				case sf::Event::KeyPressed:
					switch(event.key.code){
						case sf::Keyboard::Up:
							pose.set(Ghostie::HAND_L_POS,pose.get(Ghostie::HAND_L_POS) - 0.05);
							pose.set(Ghostie::HAND_R_POS,pose.get(Ghostie::HAND_R_POS) - 0.05);
							
							break;
						case sf::Keyboard::Down:
							pose.set(Ghostie::HAND_L_POS,pose.get(Ghostie::HAND_L_POS) + 0.05);
							pose.set(Ghostie::HAND_R_POS,pose.get(Ghostie::HAND_R_POS) + 0.05);
							
							break;
						case sf::Keyboard::Left:
							if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)){
								if(isCtrlDown){
									anim.renderablesAddBefore(new Ghostie::Char());
								}else{
									anim.renderablesBackward();
								}
							}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
								if(isCtrlDown){
									anim.sceneAddBefore(0.5);
								}else{
									anim.sceneBackward();
								}
							}else{
								pose.set(Ghostie::HAND_L_ROT,pose.get(Ghostie::HAND_L_ROT) - 1.0);
								pose.set(Ghostie::HAND_R_ROT,pose.get(Ghostie::HAND_R_ROT) - 1.0);
							}
							
							break;
						case sf::Keyboard::Right:
							if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)){
								if(isCtrlDown){
									anim.renderablesAddAfter(new Ghostie::Char());
								}else{
									anim.renderablesForward();
								}
							}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
								if(isCtrlDown){
									anim.sceneAddAfter(0.5);
								}else{
									anim.sceneForward();
								}
							}else{
								pose.set(Ghostie::HAND_L_ROT,pose.get(Ghostie::HAND_L_ROT) + 1.0);
								pose.set(Ghostie::HAND_R_ROT,pose.get(Ghostie::HAND_R_ROT) + 1.0);
							}
							
							break;
						case sf::Keyboard::D:
							if(sf::Keyboard::isKeyPressed(sf::Keyboard::C)){
								anim.renderablesRemove();
								
							}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
								anim.sceneRemove();
								
							}else if(isCtrlDown){
								if(anim.seqCurrent() != NULL){
									anim.seqCurrent()->remove();
								}
							}
							
							break;
						default:
							break;
					}
					
					break;
				case sf::Event::MouseButtonPressed:
					switch(event.mouseButton.button){
						case sf::Mouse::Left:
							/*transfer = clock.getElapsedTime();
							
							lastExp = currExp;
							currExp = (Expression)((currExp + 1) % EXP_COUNT);*/
							
							{
								float seqTime = timeView::getBegin() + timeViewCursor * (timeView::getEnd() - timeView::getBegin());
								
								if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
									if(anim.seqCurrent() != NULL){
										anim.seqCurrent()->add(seqTime);
									}
									
								}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
									if(anim.seqCurrent() != NULL){
										anim.seqCurrent()->move(seqTime);
									}
									
								}else{
									if(anim.seqCurrent() != NULL){
										anim.seqCurrent()->select(seqTime);
									}
								}
							}
							
							break;
						case sf::Mouse::Right:
							pose.setStyle((Ghostie::ParticleStyle)((pose.getStyle() + 1) % Ghostie::PARTICLE_STYLE_COUNT));
							
							switch(pose.getStyle()){
								case Ghostie::PARTICLE_STYLE_NORMAL:
									pose.set(Ghostie::BODY_SPEED,1.0);
									
									break;
								case Ghostie::PARTICLE_STYLE_SOLID:
									pose.set(Ghostie::BODY_SPEED,0.5);
									
									break;
								case Ghostie::PARTICLE_STYLE_POINTY:
									pose.set(Ghostie::BODY_SPEED,2.0);
									
									break;
								case Ghostie::PARTICLE_STYLE_COUNT:
									break;
							}
							
							break;
						default:
							break;
					}
				case sf::Event::MouseWheelScrolled:
					if(isShiftDown){
						timeView::zoomBy(event.mouseWheelScroll.delta);
					}else{
						timeView::translateBy(event.mouseWheelScroll.delta / 10.0);
					}
					
					break;
				default:
					break;
			}
		}
	}
	
	window.close();
	
	return EXIT_SUCCESS;
}
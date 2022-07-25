#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cctype>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "render.hpp"
#include "ghostie.hpp"
#include "setsequence.hpp"
#include "timeView.hpp"
#include "animation.hpp"
#include "textInput.hpp"

#include "characters/main.hpp"

#define PI 3.14159265
#define GOLDEN_RATIO 1.618033

#define WINDOW_SCREEN_HEIGHT_RATIO 0.6
#define WINDOW_SCREEN_WIDTH_RATIO 0.8

enum InputState{
	INPUT_DEFAULT,
	
	INPUT_NEW_SCENE_TIME_BEFORE,
	INPUT_NEW_SCENE_TIME_AFTER,
	INPUT_SET_SCENE_TIME,
	
	INPUT_NEW_SEQ_BEFORE,
	INPUT_NEW_SEQ_AFTER,
	
	INPUT_COUNT
};

enum Expression{
	EXP_BLANK,
	EXP_ANGRY,
	EXP_WORRIED,
	EXP_BORED,
	EXP_CHEERFUL,
	
	EXP_COUNT
};

Set expresses[EXP_COUNT];

sf::RenderWindow window;

void resizeContext(float width,float height){
	window.setView(sf::View(sf::Vector2f(0.0,0.0),sf::Vector2f(width,height)));
	render::resize();
}

bool timeInputAllowable(char c){
	return isdigit(c);
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
	
	if(!render::init(window,sf::Vector2u(1920,1080),"share-tech-mono-regular.ttf")){
		window.close();
		
		return EXIT_FAILURE;
	}
	
	Ghostie::init();
	
	resizeContext(windowWidth,windowHeight);
	
	bool run = true;
	
	sf::Event event;
	bool isCtrlDown,isShiftDown;
	
	InputState inputState = INPUT_DEFAULT;
	
	unsigned int currentCharacter = 0;
	float timeViewCursor;
	
	Animation anim;
	TextInput<TIME_DISPLAY_LEN,&timeInputAllowable> timeInput;
	// unsigned int currentProperty = 0;
	
	
	
	sf::Clock clock;
	sf::Time transfer = clock.getElapsedTime();
	
	Expression lastExp = EXP_BLANK;
	Expression currExp = EXP_BLANK;
	
	Ghostie::Char ghost;
	Set pose;
	
	pose.set(Ghostie::X,0.0)->set(Ghostie::Y,0.0)->set(Ghostie::SIZE,0.5)->set(Ghostie::BODY_SPEED,1.0);
	
	expresses[EXP_ANGRY].set(Ghostie::EYELID_LU_POS,0.3)->set(Ghostie::EYELID_LU_ROT,-30.0 / 360.0);
	expresses[EXP_ANGRY].set(Ghostie::EYELID_RU_POS,0.3)->set(Ghostie::EYELID_RU_ROT,30.0 / 360.0);
	
	expresses[EXP_WORRIED].set(Ghostie::EYELID_LU_POS,0.3)->set(Ghostie::EYELID_LU_ROT,30.0 / 360.0);
	expresses[EXP_WORRIED].set(Ghostie::EYELID_RU_POS,0.3)->set(Ghostie::EYELID_RU_ROT,-30.0 / 360.0);
	
	expresses[EXP_BORED].set(Ghostie::EYELID_LU_POS,0.2)->set(Ghostie::EYELID_RU_POS,0.2);
	
	expresses[EXP_CHEERFUL].set(Ghostie::EYELID_LL_POS,0.2)->set(Ghostie::EYELID_RL_POS,0.2);
	
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
		faceRot *= 90.0 / 360.0;
		
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
		
		ghost.draw(pose,clock.getElapsedTime().asSeconds());
		
		// Done
		render::drawView();
		
		render::drawUIBackground(timeView::getBegin(),timeView::getEnd());
		
		anim.drawMarkers();
		
		if(anim.seqCurrent() != NULL){
			anim.seqCurrent()->drawBar(timeView::getBegin(),timeView::getEnd());
			render::drawSequenceBar(render::SEQ_BAR_CURSOR,timeViewCursor);
		}
		
		switch(inputState){
			case INPUT_NEW_SCENE_TIME_BEFORE:
			case INPUT_NEW_SCENE_TIME_AFTER:
			case INPUT_SET_SCENE_TIME:
				render::drawTime(timeInput.buffer(),'_');
				
				break;
			default:
				if(anim.sceneAvailable()){
					render::drawTime(anim.sceneGetLength(),' ');
				}else{
					render::drawTime("",'-');
				}
				
				break;
		}
		
		switch(inputState){
			case INPUT_NEW_SEQ_BEFORE:
			case INPUT_NEW_SEQ_AFTER:
				render::drawCharacterLabels(currentCharacter);
				
				break;
			default:
				break;
		}
		
		window.display();
		
		// Event Handling --------------------------------
		if(window.pollEvent(event)){
			isCtrlDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
			// isAltDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);
			isShiftDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
			
			// if(anim.seqCurrent())
			
			switch(inputState){
				case INPUT_DEFAULT:
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
									if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
										if(isCtrlDown){
											inputState = INPUT_NEW_SEQ_BEFORE;
										}else{
											anim.seqBackward();
										}
									}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
										if(isCtrlDown){
											inputState = INPUT_NEW_SCENE_TIME_BEFORE;
										}else{
											anim.sceneBackward();
										}
									}else{
										pose.set(Ghostie::HAND_L_ROT,pose.get(Ghostie::HAND_L_ROT) - 1.0);
										pose.set(Ghostie::HAND_R_ROT,pose.get(Ghostie::HAND_R_ROT) - 1.0);
									}
									
									break;
								case sf::Keyboard::Right:
									if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
										if(isCtrlDown){
											inputState = INPUT_NEW_SEQ_AFTER;
										}else{
											anim.seqForward();
										}
									}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
										if(isCtrlDown){
											inputState = INPUT_NEW_SCENE_TIME_AFTER;
										}else{
											anim.sceneForward();
										}
									}else{
										pose.set(Ghostie::HAND_L_ROT,pose.get(Ghostie::HAND_L_ROT) + 1.0);
										pose.set(Ghostie::HAND_R_ROT,pose.get(Ghostie::HAND_R_ROT) + 1.0);
									}
									
									break;
								case sf::Keyboard::D:
									if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
										anim.seqRemove();
										
									}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
										anim.sceneRemove();
										
									}else if(isCtrlDown){
										if(anim.seqCurrent() != NULL){
											anim.seqCurrent()->remove();
										}
									}
									
									break;
								case sf::Keyboard::E:
									transfer = clock.getElapsedTime();
									
									lastExp = currExp;
									currExp = (Expression)((currExp + 1) % EXP_COUNT);
									
									break;
								case sf::Keyboard::W:
									if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
										inputState = INPUT_SET_SCENE_TIME;
									}
									
									break;
								default:
									break;
							}
							
							break;
						case sf::Event::MouseButtonPressed:
							switch(event.mouseButton.button){
								case sf::Mouse::Left:
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
									ghost.setStyle((Ghostie::ParticleStyle)((ghost.getStyle() + 1) % Ghostie::PARTICLE_STYLE_COUNT));
									
									switch(ghost.getStyle()){
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
					
					break;
				case INPUT_NEW_SCENE_TIME_BEFORE:
					if(event.type != sf::Event::TextEntered){
						break;
					}
					
					if(timeInput.interpret(event.text.unicode)){
						anim.sceneAddBefore(strtol(timeInput.buffer(),NULL,10));
						anim.sceneBackward();
						
						timeInput.clear();
						
						inputState = INPUT_DEFAULT;
					}
					
					break;
				case INPUT_NEW_SCENE_TIME_AFTER:
					if(event.type != sf::Event::TextEntered){
						break;
					}
					
					if(timeInput.interpret(event.text.unicode)){
						anim.sceneAddAfter(strtol(timeInput.buffer(),NULL,10));
						anim.sceneForward();
						
						timeInput.clear();
						
						inputState = INPUT_DEFAULT;
					}
					
					break;
				case INPUT_SET_SCENE_TIME:
					if(event.type != sf::Event::TextEntered){
						break;
					}
					
					if(timeInput.interpret(event.text.unicode)){
						anim.sceneSetLength(strtol(timeInput.buffer(),NULL,10));
						timeInput.clear();
						
						inputState = INPUT_DEFAULT;
					}
					
					break;
				case INPUT_NEW_SEQ_BEFORE:
				case INPUT_NEW_SEQ_AFTER:
					if(event.type != sf::Event::KeyPressed){
						break;
					}
					
					switch(event.key.code){
						case sf::Keyboard::Up:
							if(currentCharacter > 0){
								--currentCharacter;
							}
							
							break;
						case sf::Keyboard::Down:
							if(currentCharacter + 1 < chars::count()){
								++currentCharacter;
							}
							
							break;
						case sf::Keyboard::Enter:
							if(inputState == INPUT_NEW_SEQ_BEFORE){
								anim.seqAddBefore(currentCharacter);
							}else{
								anim.seqAddAfter(currentCharacter);
							}
							
							inputState = INPUT_DEFAULT;
							
							break;
						default:
							break;
					}
					
					break;
				case INPUT_COUNT:
				default:
					break;
			}
		}
	}
	
	window.close();
	
	return EXIT_SUCCESS;
}
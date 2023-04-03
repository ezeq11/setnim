#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cctype>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "render.hpp"
#include "setsequence.hpp"
#include "timeView.hpp"
#include "animation.hpp"
#include "textInput.hpp"

#include "renderers/main.hpp"
#include "renderers/ghostie.hpp"

#define PI 3.14159265
#define GOLDEN_RATIO 1.618033

#define WINDOW_SCREEN_HEIGHT_RATIO 0.6
#define WINDOW_SCREEN_WIDTH_RATIO 0.8

#define TIME_DISPLAY_LENGTH 6

#define RENDER_FPS 60

enum InputState{
	INPUT_DEFAULT,
	
	INPUT_NEW_SCENE_TIME_BEFORE,
	INPUT_NEW_SCENE_TIME_AFTER,
	INPUT_SET_SCENE_TIME,
	
	INPUT_NEW_SEQ_BEFORE,
	INPUT_NEW_SEQ_AFTER,
	
	INPUT_PLAY,
	
	INPUT_READ_FILE_ENTER,
	INPUT_READ_FILE_ACKNOWLEDGE,
	
	INPUT_WRITE_FILE_ENTER,
	INPUT_WRITE_FILE_ACKNOWLEDGE,
	
	INPUT_RENDER,
	
	INPUT_COUNT
};

bool inputStateHasLiveDisplay(InputState state){
	switch(state){
		case INPUT_PLAY:
		case INPUT_RENDER:
			return true;
			
		default:
			break;
	}
	
	return false;
}

bool timeInputAllowable(char c){
	return isdigit(c);
}

bool commandLineAllowable(char c){
	return isprint(c);
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
	
	sf::RenderWindow window(sf::VideoMode(windowWidth,windowHeight),"Setnim",sf::Style::Default,glSettings);
	
	// Execution -----------------------------------------
	if(!render::init(window,sf::Vector2u(1920,1080),"share-tech-mono-regular.ttf")){
		window.close();
		
		return EXIT_FAILURE;
	}
	
	timeView::init();
	renderers::init();
	render::resize(windowWidth,windowHeight);
	
	bool run = true;
	bool isCtrlDown,isShiftDown;
	
	sf::Event event;
	sf::Clock clock;
	
	InputState inputState = INPUT_DEFAULT;
	
	unsigned int currentRenderer = 0;
	unsigned int currentProperty = 0;
	float beginTime = 0.0;
	float currentTime;
	unsigned int currentFrame;
	
	TextInput<TIME_DISPLAY_LENGTH,&timeInputAllowable> timeInput;
	TextInput<128,&commandLineAllowable> commandLineInput;
	char pathTemp[128];
	bool successfulIO;
	
	Animation *anim = NULL;
	
	while(window.isOpen() && run){
		sf::Clock frameClock;
		
		render::UI::updateCursorPos(sf::Mouse::getPosition(window).x,sf::Mouse::getPosition(window).y);
		
		if(anim != NULL){
			currentTime = beginTime + clock.getElapsedTime().asSeconds();
			currentTime = currentTime > frames::toFloat(anim->length()) ? frames::toFloat(anim->length()) : currentTime;
		}
		
		// Drawing ---------------------------------------
		window.clear(sf::Color(0,0,0,0xff));
		render::view::clear();
		
		if(anim != NULL){
			switch(inputState){
				case INPUT_PLAY:
					anim->render(renderers::get(),frames::fromFloat(currentTime),false);
					
					break;
				case INPUT_RENDER:
					anim->render(renderers::get(),currentFrame,false);
					
					break;
				default:
					anim->render(renderers::get(),frames::fromFloat(beginTime),true);
					
					break;
			}
		}
		
		render::drawView();
		render::UI::drawBackground();
		
		if(anim != NULL){
			render::UI::markers::drawScroll(timeView::getBegin(),timeView::getEnd());
			
			anim->drawMarkers();
			
			if(anim->sceneAvailable()){
				float playBegin = beginTime / frames::toFloat(anim->length());
				float playEnd = currentTime / frames::toFloat(anim->length());
				
				if(inputState == INPUT_PLAY){
					render::UI::markers::drawScenesPlayTime(playBegin,playEnd);
				}
				
				render::UI::markers::drawScenesCursor(playBegin);
			}
			
			if(anim->seqCurrent() != NULL){
				anim->seqCurrent()->drawMarkers(normalizedUInt16::fromFloat(timeView::getBegin()),normalizedUInt16::fromFloat(timeView::getEnd()));
				
				if(render::UI::bars::insideSeqFrames()){
					render::UI::markers::drawSeqFrame(render::UI::markers::SEQ_FRAME_CURSOR,render::UI::bars::cursorValSeqFrames());
				}
				
				if(anim->seqCurrent()->current() != NULL){
					render::UI::markers::drawSetParameterBackground();
					
					if(render::UI::bars::insideSetParameter()){
						float pX = anim->seqCurrent()->current()->get(currentProperty);
						float cX = render::UI::bars::cursorValSetParameter() * 2.0 - 1.0;
						
						if(fabs(pX) < fabs(cX)){
							render::UI::markers::drawSetParameterValue(cX,true);
							render::UI::markers::drawSetParameterValue(pX,false);
						}else{
							render::UI::markers::drawSetParameterValue(pX,false);
							render::UI::markers::drawSetParameterValue(cX,true);
						}
					}else{
						render::UI::markers::drawSetParameterValue(anim->seqCurrent()->current()->get(currentProperty),false);
					}
					
					render::UI::markers::drawSetParameterZero();
				}
			}
		}
		
		switch(inputState){
			case INPUT_NEW_SCENE_TIME_BEFORE:
			case INPUT_NEW_SCENE_TIME_AFTER:
			case INPUT_SET_SCENE_TIME:
				render::UI::labels::drawTime(timeInput.buffer(),TIME_DISPLAY_LENGTH,'_');
				
				break;
			default:
				if(anim != NULL && anim->sceneAvailable()){
					render::UI::labels::drawTime(anim->sceneGetLength(),TIME_DISPLAY_LENGTH,' ');
				}else{
					render::UI::labels::drawTime("",TIME_DISPLAY_LENGTH,'.');
				}
				
				break;
		}
		
		switch(inputState){
			case INPUT_NEW_SEQ_BEFORE:
			case INPUT_NEW_SEQ_AFTER:
				render::UI::labels::drawRenderers(currentRenderer);
				
				break;
			default:
				break;
		}
		
		if(anim != NULL && anim->seqCurrent() != NULL){
			render::UI::labels::drawSetParameter(anim->seqCurrent()->getRenderIndex(),currentProperty);
			render::UI::labels::drawTransition(anim->seqCurrent()->getTransition(currentProperty));
		}
		
		switch(inputState){
			case INPUT_READ_FILE_ENTER:
				render::UI::labels::drawCommandLine("read: ",commandLineInput.buffer());
				
				break;
			case INPUT_READ_FILE_ACKNOWLEDGE:
				if(successfulIO){
					render::UI::labels::drawCommandLine("successfully read from: ",commandLineInput.buffer());
				}else{
					render::UI::labels::drawCommandLine("unable to read from: ",commandLineInput.buffer());
				}
				
				break;
			case INPUT_WRITE_FILE_ENTER:
				render::UI::labels::drawCommandLine("write: ",commandLineInput.buffer());
				
				break;
			case INPUT_WRITE_FILE_ACKNOWLEDGE:
				if(successfulIO){
					render::UI::labels::drawCommandLine("successfully written to: ",commandLineInput.buffer());
				}else{
					render::UI::labels::drawCommandLine("unable to write to: ",commandLineInput.buffer());
				}
				
				break;
			default:
				break;
		}
		
		window.display();
		
		// Event Handling --------------------------------
		if(inputState == INPUT_RENDER){
			sprintf(pathTemp,"render/%u.png",currentFrame);
			render::view::writeTo(pathTemp);
			
			if(currentFrame >= anim->length()){
				inputState = INPUT_DEFAULT;
			}else{
				currentFrame += frames::perSec / RENDER_FPS;
			}
		}
		
		if(window.pollEvent(event) || (!inputStateHasLiveDisplay(inputState) && window.waitEvent(event))){
			isCtrlDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
			isShiftDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
			
			switch(event.type){
				case sf::Event::Closed:
					run = false;
					
					break;
				case sf::Event::Resized:
					render::resize(event.size.width,event.size.height);
					
					break;
				default:
					break;
			}
			
			switch(inputState){
				case INPUT_DEFAULT:
					switch(event.type){
						case sf::Event::KeyPressed:
							if(anim == NULL && isCtrlDown){
								switch(event.key.code){
									case sf::Keyboard::C:
										anim = new Animation();
										
										break;
									case sf::Keyboard::R:
										inputState = INPUT_READ_FILE_ENTER;
										
										break;
									default:
										break;
								}
							}
							
							break;
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
					
					if(anim == NULL){
						break;
					}
					
					switch(event.type){
						case sf::Event::KeyPressed:
							switch(event.key.code){
								case sf::Keyboard::Up:
									if(isShiftDown){
										if(anim->seqCurrent() != NULL){
											Transition newTransition = (Transition)((TRANSITION_COUNT + anim->seqCurrent()->getTransition(currentProperty) + 1) % TRANSITION_COUNT);
											
											anim->seqCurrent()->setTransition(currentProperty,newTransition);
										}
									}else{
										if(currentProperty + 1 < SET_SIZE){
											++currentProperty;
										}
									}
									
									break;
								case sf::Keyboard::Down:
									if(isShiftDown){
										if(anim->seqCurrent() != NULL){
											Transition newTransition = (Transition)((TRANSITION_COUNT + anim->seqCurrent()->getTransition(currentProperty) - 1) % TRANSITION_COUNT);
											
											anim->seqCurrent()->setTransition(currentProperty,newTransition);
										}
									}else{
										if(currentProperty > 0){
											--currentProperty;
										}
									}
									
									break;
								case sf::Keyboard::Left:
									if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
										if(isCtrlDown){
											inputState = INPUT_NEW_SEQ_BEFORE;
										}else{
											anim->seqBackward();
										}
									}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
										if(isCtrlDown){
											inputState = INPUT_NEW_SCENE_TIME_BEFORE;
										}else{
											anim->sceneBackward();
										}
									}
									
									break;
								case sf::Keyboard::Right:
									if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
										if(isCtrlDown){
											inputState = INPUT_NEW_SEQ_AFTER;
										}else{
											anim->seqForward();
										}
									}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
										if(isCtrlDown){
											inputState = INPUT_NEW_SCENE_TIME_AFTER;
										}else{
											anim->sceneForward();
										}
									}
									
									break;
								case sf::Keyboard::Space:
									if(anim->sceneAvailable()){
										clock.restart();
										
										inputState = INPUT_PLAY;
									}
									
									break;
								case sf::Keyboard::D:
									if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
										anim->seqRemove();
										
									}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
										anim->sceneRemove();
										
									}else if(isCtrlDown){
										if(anim->seqCurrent() != NULL){
											anim->seqCurrent()->remove();
										}
									}
									
									break;
								case sf::Keyboard::R:
									if(isCtrlDown){
										inputState = INPUT_RENDER;
										currentFrame = 0;
									}
									
									break;
								case sf::Keyboard::T:
									if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
										inputState = INPUT_SET_SCENE_TIME;
									}
									
									break;
								case sf::Keyboard::W:
									if(isCtrlDown){
										inputState = INPUT_WRITE_FILE_ENTER;
									}
									
									break;
								case sf::Keyboard::X:
									if(isCtrlDown){
										delete anim;
										
										anim = NULL;
									}
									
									break;
								default:
									break;
							}
							
							break;
						case sf::Event::MouseButtonPressed:
							switch(event.mouseButton.button){
								case sf::Mouse::Left:
									if(render::UI::bars::insideScenes()){
										if(anim->sceneAvailable()){
											beginTime = render::UI::bars::cursorValScenes() * frames::toFloat(anim->length());
										}
									}else if(render::UI::bars::insideSeqFrames()){
										if(anim->seqCurrent() != NULL){
											normalizedUInt16::val seqTime = normalizedUInt16::fromFloat(timeView::getBegin() + render::UI::bars::cursorValSeqFrames() * (timeView::getEnd() - timeView::getBegin()));
											
											if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
												anim->seqCurrent()->add(seqTime);
												
											}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
												anim->seqCurrent()->move(seqTime);
												
											}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
												anim->seqCurrent()->duplicate(seqTime);
												
											}else{
												anim->seqCurrent()->select(seqTime);
											}
										}
									}else if(render::UI::bars::insideSetParameter()){
										if(anim->seqCurrent() != NULL && anim->seqCurrent()->current() != NULL){
											anim->seqCurrent()->current()->set(currentProperty,render::UI::bars::cursorValSetParameter() * 2.0 - 1.0);
										}
									}
									
									break;
								default:
									break;
							}
						default:
							break;
					}
					
					break;
				case INPUT_NEW_SCENE_TIME_BEFORE:
					if(event.type != sf::Event::TextEntered){
						break;
					}
					
					if(timeInput.interpret(event.text.unicode)){
						anim->sceneAddBefore(strtol(timeInput.buffer(),NULL,10));
						anim->sceneBackward();
						
						timeInput.clear();
						
						inputState = INPUT_DEFAULT;
					}
					
					break;
				case INPUT_NEW_SCENE_TIME_AFTER:
					if(event.type != sf::Event::TextEntered){
						break;
					}
					
					if(timeInput.interpret(event.text.unicode)){
						anim->sceneAddAfter(strtol(timeInput.buffer(),NULL,10));
						anim->sceneForward();
						
						timeInput.clear();
						
						inputState = INPUT_DEFAULT;
					}
					
					break;
				case INPUT_SET_SCENE_TIME:
					if(event.type != sf::Event::TextEntered){
						break;
					}
					
					if(timeInput.interpret(event.text.unicode)){
						anim->sceneSetLength(strtol(timeInput.buffer(),NULL,10));
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
							if(currentRenderer > 0){
								--currentRenderer;
							}
							
							break;
						case sf::Keyboard::Down:
							if(currentRenderer + 1 < renderers::count()){
								++currentRenderer;
							}
							
							break;
						case sf::Keyboard::Enter:
							if(inputState == INPUT_NEW_SEQ_BEFORE){
								anim->seqAddBefore(currentRenderer);
							}else{
								anim->seqAddAfter(currentRenderer);
							}
							
							inputState = INPUT_DEFAULT;
							
							break;
						default:
							break;
					}
					
					break;
				case INPUT_PLAY:
					if(event.type != sf::Event::KeyPressed){
						break;
					}
					
					switch(event.key.code){
						case sf::Keyboard::Space:
							inputState = INPUT_DEFAULT;
							
							break;
						default:
							break;
					}
					
					break;
				case INPUT_READ_FILE_ENTER:
					if(event.type != sf::Event::TextEntered){
						break;
					}
					
					if(commandLineInput.interpret(event.text.unicode)){
						FILE *in = fopen(commandLineInput.buffer(),"rb");
						
						if(in == NULL){
							successfulIO = false;
							inputState = INPUT_READ_FILE_ACKNOWLEDGE;
							
							break;
						}
						
						anim = new Animation(in);
						fclose(in);
						
						successfulIO = true;
						inputState = INPUT_READ_FILE_ACKNOWLEDGE;
					}
					
					break;
				case INPUT_WRITE_FILE_ENTER:
					if(event.type != sf::Event::TextEntered){
						break;
					}
					
					if(commandLineInput.interpret(event.text.unicode)){
						FILE *out = fopen(commandLineInput.buffer(),"wb");
						
						if(out == NULL){
							successfulIO = false;
							inputState = INPUT_WRITE_FILE_ACKNOWLEDGE;
							
							break;
						}
						
						anim->write(out);
						fclose(out);
						
						successfulIO = true;
						inputState = INPUT_WRITE_FILE_ACKNOWLEDGE;
					}
					
					break;
				case INPUT_READ_FILE_ACKNOWLEDGE:
				case INPUT_WRITE_FILE_ACKNOWLEDGE:
					if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space){
						commandLineInput.clear();
						inputState = INPUT_DEFAULT;
					}
					
					break;
				case INPUT_COUNT:
				default:
					break;
			}
		}
		
		float frameTime = frameClock.getElapsedTime().asSeconds();
		
		if(inputStateHasLiveDisplay(inputState) && frameTime < 1.0 / frames::perSec){
			sf::sleep(sf::seconds(1.0 / frames::perSec - frameTime));
		}
	}
	
	if(anim != NULL){
		delete anim;
	}
	
	window.close();
	
	return EXIT_SUCCESS;
}
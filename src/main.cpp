#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "render.hpp"
#include "ghostie.hpp"

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

render::ghostiePose expresses[EXP_COUNT];

sf::RenderWindow window;

void resizeContext(float width,float height){
	window.setView(sf::View(sf::Vector2f(0.0,0.0),sf::Vector2f(width,height)));
	render::resize();
}

int main(){
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
	bool run = true;
	sf::Event event;
	
	render::init(window,sf::Vector2u(1920,1080));
	resizeContext(windowWidth,windowHeight);
	
	sf::Clock clock;
	sf::Time transfer = clock.getElapsedTime();
	
	Expression lastExp = EXP_BLANK;
	Expression currExp = EXP_BLANK;
	
	render::ghostiePose pose;
	
	expresses[EXP_ANGRY].lup = 0.3;
	expresses[EXP_ANGRY].lur = -30.0;
	expresses[EXP_ANGRY].rup = 0.3;
	expresses[EXP_ANGRY].rur = 30.0;
	
	expresses[EXP_WORRIED].lup = 0.3;
	expresses[EXP_WORRIED].lur = 30.0;
	expresses[EXP_WORRIED].rup = 0.3;
	expresses[EXP_WORRIED].rur = -30.0;
	
	expresses[EXP_BORED].lup = 0.2;
	expresses[EXP_BORED].rup = 0.2;
	
	expresses[EXP_CHEERFUL].ll = 0.2;
	expresses[EXP_CHEERFUL].rl = 0.2;
	
	while(window.isOpen() && run){
		// Drawing ---------------------------------------
		window.clear(sf::Color(0xff,0,0x0f,255));
		
		float pace = sin(clock.getElapsedTime().asSeconds());
		pose.fd = ((float)sf::Mouse::getPosition(window).x / (float)window.getSize().x) * 2.0 - 1.0;
		pose.fd = pose.fd < -1.0 ? -1.0 : (pose.fd > 1.0 ? 1.0 : pose.fd);
		
		pose.fr = ((float)sf::Mouse::getPosition(window).y / (float)window.getSize().y) * 2.0 - 1.0;
		pose.fr = pose.fr < -1.0 ? -1.0 : (pose.fr > 1.0 ? 1.0 : pose.fr);
		pose.fr *= 90.0;
		
		if(pose.fd > 0.0){
			pose.fr *= -1.0;
		}
		
		float delta = clock.getElapsedTime().asSeconds() - transfer.asSeconds();
		
		if(delta < 0.15){
			pose.ll = expresses[lastExp].ll + ((expresses[currExp].ll - expresses[lastExp].ll) * (delta / 0.15));
			pose.lup = expresses[lastExp].lup + ((expresses[currExp].lup - expresses[lastExp].lup) * (delta / 0.15));
			pose.lur = expresses[lastExp].lur + ((expresses[currExp].lur - expresses[lastExp].lur) * (delta / 0.15));
			
			pose.rl = expresses[lastExp].rl + ((expresses[currExp].rl - expresses[lastExp].rl) * (delta / 0.15));
			pose.rup = expresses[lastExp].rup + ((expresses[currExp].rup - expresses[lastExp].rup) * (delta / 0.15));
			pose.rur = expresses[lastExp].rur + ((expresses[currExp].rur - expresses[lastExp].rur) * (delta / 0.15));
			
			//render::ghostie(300,0,600,pose);
			render::ghostie(0,0,500,pose,clock.getElapsedTime().asSeconds());
			
		}else{
			//render::ghostie(300,0,600,pose);
			render::ghostie(0,0,500,pose,clock.getElapsedTime().asSeconds());
			
		}
		
		// Done
		window.display();
		
		// Event Handling --------------------------------
		if(window.pollEvent(event)){
			// isCtrlDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl);
			// isAltDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt);
			// isShiftDown = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);
			
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
							pose.hlp -= 0.05;
							pose.hrp -= 0.05;
							
							break;
						case sf::Keyboard::Down:
							pose.hlp += 0.05;
							pose.hrp += 0.05;
							
							break;
						case sf::Keyboard::Left:
							pose.hlr -= 1.0;
							pose.hrr -= 1.0;
							
							break;
						case sf::Keyboard::Right:
							pose.hlr += 1.0;
							pose.hrr += 1.0;
							
							break;
						case sf::Keyboard::O:
							// pose.bodyOutline = !pose.bodyOutline;
							
							break;
						default:
							break;
					}
					
					break;
				case sf::Event::MouseButtonPressed:
					switch(event.mouseButton.button){
						case sf::Mouse::Left:
							transfer = clock.getElapsedTime();
							
							lastExp = currExp;
							currExp = (Expression)((currExp + 1) % EXP_COUNT);
							
							break;
						case sf::Mouse::Right:
							pose.bodyStyle = (render::ghostieBody)((pose.bodyStyle + 1) % render::BODY_COUNT);
							
							switch(pose.bodyStyle){
								case render::BODY_NORMAL:
									pose.bodySpeed = 1.0;
									
									break;
								case render::BODY_SOLID:
									pose.bodySpeed = 0.5;
									
									break;
								case render::BODY_POINTY:
									pose.bodySpeed = 2.0;
									
									break;
								case render::BODY_COUNT:
									break;
							}
							
							break;
						default:
							break;
					}
				default:
					break;
			}
		}
	}
	
	window.close();
	
	return EXIT_SUCCESS;
}
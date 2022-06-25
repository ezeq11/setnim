#include <cstdlib>
#include <cmath>

#include "render.hpp"

#define SCROLL_HEIGHT 7

#define SEQUENCE_HEIGHT 20
#define SEQUENCE_BAR_WIDTH 18

#define VIEW_TOP_BOTTOM_PADDING (SCROLL_HEIGHT + SEQUENCE_HEIGHT)

namespace render{
	// Variables --------------------------------------------------
	sf::RenderWindow *window;
	float winWidth,winHeight;
	sf::Vector2u dimension;
	
	sf::RectangleShape scrollBackground,scrollBar;
	sf::RectangleShape sequenceBackground,sequenceBar,sequenceBarHighlighted,sequenceBarCursor;
	
	namespace view{
		sf::RenderTexture view;
		sf::Sprite viewSprite;
		
		sf::RectangleShape viewBounds;
		
		namespace temp{
			sf::RenderTexture viewTemp;
			sf::Sprite viewTempSprite;
		}
	}
	
	// Functions --------------------------------------------------
	bool init(sf::RenderWindow &renderWindow,const sf::Vector2u &innerDimension){
		// Render Containers
		window = &renderWindow;
		dimension = innerDimension;
		
		if(!view::view.create(dimension.x,dimension.y)){
			return false;
		}
		
		if(!view::temp::viewTemp.create(dimension.x,dimension.y)){
			return false;
		}
		
		view::view.setSmooth(true);
		
		sf::View innerView = sf::View(sf::Vector2f(0.0,0.0),sf::Vector2f(dimension));
		
		view::view.setView(innerView);
		view::temp::viewTemp.setView(innerView);
		
		view::viewSprite.setTexture(view::view.getTexture(),true);
		view::viewSprite.setOrigin(sf::Vector2f(view::view.getSize() / 2u));
		
		view::temp::viewTempSprite.setTexture(view::temp::viewTemp.getTexture(),true);
		view::temp::viewTempSprite.setOrigin(sf::Vector2f(view::temp::viewTemp.getSize() / 2u));
		
		resize();
		
		// UI Elements
		sf::Vector2f viewBoundsSize = sf::Vector2f(dimension.x - 10,dimension.y - 10);
		
		view::viewBounds.setSize(viewBoundsSize);
		view::viewBounds.setOrigin(viewBoundsSize / 2.0f);
		
		view::viewBounds.setFillColor(sf::Color(0,0,0,0));
		view::viewBounds.setOutlineColor(sf::Color(0,0,0,0xff));
		view::viewBounds.setOutlineThickness(5.0);
		
		scrollBackground.setFillColor(sf::Color(80,80,80,0xff));
		scrollBar.setFillColor(sf::Color(200,200,200,0xff));
		
		sequenceBackground.setFillColor(sf::Color(150,150,150,0xff));
		
		sequenceBar.setFillColor(sf::Color(100,100,100,0xff));
		sequenceBar.setSize(sf::Vector2f(SEQUENCE_BAR_WIDTH,SEQUENCE_HEIGHT));
		sequenceBar.setOrigin(SEQUENCE_BAR_WIDTH / 2.0,0.0);
		
		sequenceBarHighlighted.setFillColor(sf::Color(160,100,10,0xff));
		sequenceBarHighlighted.setSize(sf::Vector2f(SEQUENCE_BAR_WIDTH,SEQUENCE_HEIGHT));
		sequenceBarHighlighted.setOrigin(SEQUENCE_BAR_WIDTH / 2.0,0.0);
		
		sequenceBarCursor.setFillColor(sf::Color(100,0,150,0xff));
		sequenceBarCursor.setSize(sf::Vector2f(SEQUENCE_BAR_WIDTH,SEQUENCE_HEIGHT / 2.0));
		sequenceBarCursor.setOrigin(SEQUENCE_BAR_WIDTH / 2.0,0.0);
		
		return true;
	}
	
	void resize(){
		winWidth = window->getView().getSize().x;
		winHeight = window->getView().getSize().y;
		
		float scale = (winHeight - VIEW_TOP_BOTTOM_PADDING) / dimension.y;
		view::viewSprite.setScale(sf::Vector2f(scale,scale));
	}
	
	void clear(){
		window->clear(sf::Color(0,0,0,0));
	}
	
	void drawView(){
		window->draw(view::viewSprite);
	}
	
	void drawUIBackground(float scrollBegin,float scrollEnd){
		// Sequence Scroll Bar
		scrollBackground.setSize(sf::Vector2f(winWidth,SCROLL_HEIGHT));
		scrollBackground.setPosition(-winWidth / 2.0,winHeight / 2.0 - SCROLL_HEIGHT);
		window->draw(scrollBackground);
		
		scrollBar.setSize(sf::Vector2f((scrollEnd - scrollBegin) * winWidth,SCROLL_HEIGHT));
		scrollBar.setPosition((scrollBegin - 0.5) * winWidth,winHeight / 2.0 - SCROLL_HEIGHT);
		window->draw(scrollBar);
		
		// Sequence Frames
		sequenceBackground.setSize(sf::Vector2f(winWidth,SEQUENCE_HEIGHT));
		sequenceBackground.setPosition(-winWidth / 2.0,winHeight / 2.0 - SCROLL_HEIGHT - SEQUENCE_HEIGHT);
		window->draw(sequenceBackground);
	}
	
	void drawSequenceBar(SequenceBar type,float x){
		float scrX = (x - 0.5) * winWidth;
		float scrY = winHeight / 2.0 - SCROLL_HEIGHT - SEQUENCE_HEIGHT;
		
		switch(type){
			case SEQ_BAR_NORMAL:
				sequenceBar.setPosition(scrX,scrY);
				window->draw(sequenceBar);
				
				break;
			case SEQ_BAR_HIGHLIGHTED:
				sequenceBarHighlighted.setPosition(scrX,scrY);
				window->draw(sequenceBarHighlighted);
				
				break;
			case SEQ_BAR_CURSOR:
				sequenceBarCursor.setPosition(scrX,scrY);
				window->draw(sequenceBarCursor);
				
				break;
			default:
				break;
		}
	}
	
	namespace view{
		void clear(){
			view.clear(sf::Color(0,0,0,0));
		}
		
		void draw(const sf::Drawable &drawable,const sf::BlendMode &blendMode,const sf::Transform &transform){
			view.draw(drawable,sf::RenderStates(blendMode,transform,NULL,NULL));
		}
		
		void draw(const sf::Drawable &drawable,const sf::Transform &transform){
			view.draw(drawable,sf::RenderStates(transform));
		}
		
		void drawTemp(const sf::Transform &transform){
			draw(temp::viewTempSprite,transform);
		}
		
		void drawBounds(){
			draw(viewBounds,sf::Transform::Identity);
		}
		
		namespace temp{
			void clear(){
				viewTemp.clear(sf::Color(0,0,0,0));
			}
			
			void draw(const sf::Drawable &drawable,const sf::BlendMode &blendMode,const sf::Transform &transform){
				viewTemp.draw(drawable,sf::RenderStates(blendMode,transform,NULL,NULL));
			}
			
			void draw(const sf::Drawable &drawable,const sf::Transform &transform){
				viewTemp.draw(drawable,sf::RenderStates(transform));
			}
		}
	}
}
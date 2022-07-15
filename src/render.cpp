#include <cstdlib>
#include <cmath>

#include "render.hpp"

#define TEXT_SIZE 20

#define SCENE_HEIGHT 20

#define SCENE_MARKER_MARGIN 2
#define SCENE_MARKER_HEIGHT (SCENE_HEIGHT - SCENE_MARKER_MARGIN - SCENE_MARKER_MARGIN)
#define SCENE_MARKER_WIDTH 30

#define SCROLL_HEIGHT 7

#define SEQUENCE_HEIGHT 15
#define SEQUENCE_BAR_WIDTH 18

#define RENDERABLE_HEIGHT 20

#define VIEW_TOP_BOTTOM_PADDING (SCENE_HEIGHT + RENDERABLE_HEIGHT + SCROLL_HEIGHT + SEQUENCE_HEIGHT)

namespace render{
	// Variables --------------------------------------------------
	sf::RenderWindow *window;
	float winWidth,winHeight;
	sf::Vector2u dimensions;
	
	sf::Font font;
	
	sf::RectangleShape sceneBackground,sceneMarker,sceneMarkerHighlighted;
	sf::RectangleShape scrollBackground,scrollBar;
	sf::RectangleShape sequenceBackground,sequenceBar,sequenceBarHighlighted,sequenceBarCursor;
	sf::RectangleShape RenderableBackground;
	
	char timeDisplayBuffer[50];
	
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
	bool init(sf::RenderWindow &renderWindow,const sf::Vector2u &innerDimensions,const char *fontFile){
		window = &renderWindow;
		dimensions = innerDimensions;
		
		if(!font.loadFromFile(std::string(fontFile))){
			return false;
		}
		
		// Render Containers ----------------
		if(!view::view.create(dimensions.x,dimensions.y)){
			return false;
		}
		
		if(!view::temp::viewTemp.create(dimensions.x,dimensions.y)){
			return false;
		}
		
		view::view.setSmooth(true);
		
		sf::View innerView = sf::View(sf::Vector2f(0.0,0.0),sf::Vector2f(dimensions));
		
		view::view.setView(innerView);
		view::temp::viewTemp.setView(innerView);
		
		view::viewSprite.setTexture(view::view.getTexture(),true);
		view::viewSprite.setOrigin(sf::Vector2f(view::view.getSize() / 2u));
		
		view::temp::viewTempSprite.setTexture(view::temp::viewTemp.getTexture(),true);
		view::temp::viewTempSprite.setOrigin(sf::Vector2f(view::temp::viewTemp.getSize() / 2u));
		
		resize();
		
		// UI Elements ----------------------
		// View Bounds
		sf::Vector2f viewBoundsSize = sf::Vector2f(dimensions.x - 10,dimensions.y - 10);
		
		view::viewBounds.setSize(viewBoundsSize);
		view::viewBounds.setOrigin(viewBoundsSize / 2.0f);
		
		view::viewBounds.setFillColor(sf::Color(0,0,0,0));
		view::viewBounds.setOutlineColor(sf::Color(0,0,0,0xff));
		view::viewBounds.setOutlineThickness(5.0);
		
		// Scene
		sceneBackground.setFillColor(sf::Color(50,50,50,0xff));
		
		sceneMarker.setFillColor(sf::Color(160,160,160,0xff));
		sceneMarker.setSize(sf::Vector2f(SCENE_MARKER_WIDTH,SCENE_MARKER_HEIGHT));
		sceneMarker.setOrigin(SCENE_MARKER_WIDTH / 2.0,-SCENE_MARKER_MARGIN);
		
		sceneMarkerHighlighted.setFillColor(sf::Color(160,100,10,0xff));
		sceneMarkerHighlighted.setSize(sf::Vector2f(SCENE_MARKER_WIDTH,SCENE_MARKER_HEIGHT));
		sceneMarkerHighlighted.setOrigin(SCENE_MARKER_WIDTH / 2.0,-SCENE_MARKER_MARGIN);
		
		// Scroll
		scrollBackground.setFillColor(sf::Color(80,80,80,0xff));
		scrollBar.setFillColor(sf::Color(200,200,200,0xff));
		
		// Sequence
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
		
		// Renderables
		RenderableBackground.setFillColor(sf::Color(80,80,80,0xff));
		
		return true;
	}
	
	void resize(){
		winWidth = window->getView().getSize().x;
		winHeight = window->getView().getSize().y;
		
		float scale = (winHeight - VIEW_TOP_BOTTOM_PADDING) / dimensions.y;
		view::viewSprite.setScale(sf::Vector2f(scale,scale));
	}
	
	sf::Vector2u innerDimensions(){
		return dimensions;
	}
	
	void clear(){
		window->clear(sf::Color(0,0,0,0));
	}
	
	void drawView(){
		window->draw(view::viewSprite);
	}
	
	void drawUIBackground(float scrollBegin,float scrollEnd){
		float left = -winWidth / 2.0;
		float top = winHeight / 2.0;
		
		// Scenes
		sceneBackground.setSize(sf::Vector2f(winWidth,SCENE_HEIGHT));
		sceneBackground.setPosition(left,top - SCENE_HEIGHT);
		window->draw(sceneBackground);
		
		// Sequence Scroll Bar
		scrollBackground.setSize(sf::Vector2f(winWidth,SCROLL_HEIGHT));
		scrollBackground.setPosition(left,top- SCENE_HEIGHT - SCROLL_HEIGHT);
		window->draw(scrollBackground);
		
		scrollBar.setSize(sf::Vector2f((scrollEnd - scrollBegin) * winWidth,SCROLL_HEIGHT));
		scrollBar.setPosition((scrollBegin - 0.5) * winWidth,top - SCENE_HEIGHT - SCROLL_HEIGHT);
		window->draw(scrollBar);
		
		// Sequence Frames
		sequenceBackground.setSize(sf::Vector2f(winWidth,SEQUENCE_HEIGHT));
		sequenceBackground.setPosition(left,top - SCENE_HEIGHT - SCROLL_HEIGHT - SEQUENCE_HEIGHT);
		window->draw(sequenceBackground);
		
		// Renderables
		RenderableBackground.setSize(sf::Vector2f(winWidth,RENDERABLE_HEIGHT));
		RenderableBackground.setPosition(left,top - SCENE_HEIGHT - SCROLL_HEIGHT - SEQUENCE_HEIGHT - RENDERABLE_HEIGHT);
		window->draw(RenderableBackground);
	}
	
	void drawSceneMarker(bool highlighted,int x){
		float scrX = x * (SCENE_MARKER_WIDTH + SCENE_MARKER_MARGIN + SCENE_MARKER_MARGIN);
		float scrY = winHeight / 2.0 - SCENE_HEIGHT;
		
		if(highlighted){
			sceneMarkerHighlighted.setPosition(scrX,scrY);
			window->draw(sceneMarkerHighlighted);
		}else{
			sceneMarker.setPosition(scrX,scrY);
			window->draw(sceneMarker);
		}
	}
	
	void drawSequenceBar(SequenceBar type,float x){
		float scrX = (x - 0.5) * winWidth;
		float scrY = winHeight / 2.0 - SCENE_HEIGHT - SCROLL_HEIGHT - SEQUENCE_HEIGHT;
		
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
	
	void drawRenderableMarker(bool highlighted,int x){
		float scrX = x * (SCENE_MARKER_WIDTH + SCENE_MARKER_MARGIN + SCENE_MARKER_MARGIN);
		float scrY = winHeight / 2.0 - SCENE_HEIGHT - SCROLL_HEIGHT - SEQUENCE_HEIGHT - RENDERABLE_HEIGHT;
		
		if(highlighted){
			sceneMarkerHighlighted.setPosition(scrX,scrY);
			window->draw(sceneMarkerHighlighted);
		}else{
			sceneMarker.setPosition(scrX,scrY);
			window->draw(sceneMarker);
		}
	}
	
	void drawTime(temporal::val time){
		sprintf(timeDisplayBuffer,"%3u.%02u",time / 100,time % 100);
		
		sf::Text display(std::string(timeDisplayBuffer),font,TEXT_SIZE);
		display.setPosition(-winWidth / 2.0 + TEXT_SIZE / 2.0,-winHeight / 2.0 + TEXT_SIZE / 2.0);
		
		window->draw(display);
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
#include <cstdlib>
#include <cmath>
#include <cstring>

#include "render.hpp"

#include "characters/main.hpp"

#define TEXT_SIZE 18

// Top UI
#define TIME_DISPLAY_TOP 0
#define TIME_DISPLAY_WIDTH 140
#define TIME_DISPLAY_X_MARGIN 15
#define TIME_DISPLAY_Y_MARGIN 10

#define SCENE_TOP 0
#define SCENE_HEIGHT (TIME_DISPLAY_Y_MARGIN + TEXT_SIZE + TIME_DISPLAY_Y_MARGIN)

#define SEQ_MARKER_TOP (SCENE_HEIGHT)
#define SEQ_MARKER_DIAMETER 20
#define SEQ_MARKER_X_MARGIN 20
#define SEQ_MARKER_Y_MARGIN 10
#define SEQ_MARKER_HEIGHT (SEQ_MARKER_Y_MARGIN + SEQ_MARKER_DIAMETER + SEQ_MARKER_Y_MARGIN)

#define CHARACTER_LABEL_TOP (SEQ_MARKER_TOP + SEQ_MARKER_HEIGHT)
#define CHARACTER_LABEL_WIDTH 200
#define CHARACTER_LABEL_X_MARGIN 15
#define CHARACTER_LABEL_Y_MARGIN 10

// Bottom UI
#define SCROLL_HEIGHT 8
#define SCROLL_TOP SCROLL_HEIGHT

#define SEQ_FRAME_HEIGHT 25
#define SEQ_FRAME_TOP (SCROLL_HEIGHT + SEQ_FRAME_HEIGHT)

#define PARAMETER_X_MARGIN 15
#define PARAMETER_Y_MARGIN 6
#define PARAMETER_HEIGHT (PARAMETER_Y_MARGIN + TEXT_SIZE + PARAMETER_Y_MARGIN)
#define PARAMETER_TOP (SEQ_FRAME_TOP + PARAMETER_HEIGHT)

// UI Misc
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define VIEW_TOP_BOTTOM_PADDING (MAX(SEQ_MARKER_TOP + SEQ_MARKER_HEIGHT,PARAMETER_TOP))

namespace render{
	// Variables --------------------------------------------------
	sf::RenderWindow *window;
	float winWidth,winHeight;
	
	sf::Vector2u dimensions;
	
	class UIBar{
		private:
			bool fromTop;
			unsigned int relTop,height;
			unsigned int lPadding,rPadding;
			
			float top() const{
				return fromTop ? relTop : winHeight - relTop;
			}
			
		public:
			UIBar(bool isFromTop,unsigned int newRelTop,unsigned int newHeight,unsigned int newLPadding,unsigned int newRPadding)
			:fromTop(isFromTop),relTop(newRelTop),height(newHeight),lPadding(newLPadding),rPadding(newRPadding){
				
			}
			
			bool isInside(int x,int y) const{
				return (y > top()) && (y < top() + (int)height) && (x > (int)lPadding) && (x < winWidth - (int)rPadding);
			}
			
			float interpolated(float x)const {
				return (float)(x - lPadding) / (float)(winWidth - lPadding - rPadding);
			}
			
			void drawBackground(const sf::Color &color) const{
				sf::RectangleShape rect(sf::Vector2f(winWidth,height));
				rect.setPosition(0.0,top());
				rect.setFillColor(color);
				
				window->draw(rect);
			}
			
			void drawRectInterval(float begin,float end,const sf::Color &color) const{
				float barWidth = winWidth - lPadding - rPadding;
				
				sf::RectangleShape rect(sf::Vector2f((end - begin) * barWidth,height));
				rect.setPosition(lPadding + begin * barWidth,top());
				rect.setFillColor(color);
				
				window->draw(rect);
			}
			
			void drawRectCentered(float x,unsigned int width,const sf::Color &color) const{
				sf::RectangleShape rect(sf::Vector2f(width,height));
				rect.setOrigin((float)width / 2.0,0.0);
				rect.setPosition(lPadding + x * (winWidth - lPadding - rPadding),top());
				rect.setFillColor(color);
				
				window->draw(rect);
			}
	};
	
	UIBar scenesBar       (true , SCENE_TOP     , SCENE_HEIGHT     , TIME_DISPLAY_WIDTH , 0);
	UIBar scrollBar       (false, SCROLL_TOP    , SCROLL_HEIGHT    , 0                  , 0);
	UIBar seqFramesBar    (false, SEQ_FRAME_TOP , SEQ_FRAME_HEIGHT , 0                  , 0);
	UIBar setParamaterBar (false, PARAMETER_TOP , PARAMETER_HEIGHT , 0                  , 0);
	
	sf::Font font;
	char textBuffer[128];
	
	sf::CircleShape seqMarker;
	sf::RectangleShape characterLabelsBackground,characterLabelsSelection;
	
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
		
		resize(window->getView().getSize().x,window->getView().getSize().y);
		
		// UI Elements ----------------------
		// View Bounds
		sf::Vector2f viewBoundsSize = sf::Vector2f(dimensions.x - 10,dimensions.y - 10);
		
		view::viewBounds.setSize(viewBoundsSize);
		view::viewBounds.setOrigin(viewBoundsSize / 2.0f);
		
		view::viewBounds.setFillColor(sf::Color(0,0,0,0));
		view::viewBounds.setOutlineColor(sf::Color(0,0,0,0xff));
		view::viewBounds.setOutlineThickness(5.0);
		
		// UI
		seqMarker.setRadius(SEQ_MARKER_DIAMETER / 2.0);
		seqMarker.setOrigin(SEQ_MARKER_DIAMETER / 2.0,0.0);
		
		characterLabelsBackground.setFillColor(sf::Color(200,200,200,0xff));
		characterLabelsSelection.setFillColor(sf::Color(161,125,50,0xff));
		
		return true;
	}
	
	void resize(float width,float height){
		winWidth = width;
		winHeight = height;
		
		window->setView(sf::View(sf::Vector2f(width / 2.0,height / 2.0),sf::Vector2f(width,height)));
		
		float scale = (height - VIEW_TOP_BOTTOM_PADDING) / dimensions.y;
		view::viewSprite.setScale(sf::Vector2f(scale,scale));
		view::viewSprite.setPosition(sf::Vector2f(width / 2.0,height / 2.0));
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
		scenesBar.drawBackground(sf::Color(100,100,100,0xff));
		
		scrollBar.drawBackground(sf::Color(80,80,80,0xff));
		scrollBar.drawRectInterval(scrollBegin,scrollEnd,sf::Color(200,200,200,0xff));
		
		seqFramesBar.drawBackground(sf::Color(150,150,150,0xff));
		setParamaterBar.drawBackground(sf::Color(100,100,100,0xff));
	}
	
	void drawSceneMarker(float begin,float end,bool highlighted,bool drawCap){
		scenesBar.drawRectInterval(begin,end,highlighted ? sf::Color(161,125,50,0xff) : sf::Color(160,160,160,0xff));
		
		if(drawCap){
			scenesBar.drawRectCentered(end,4,sf::Color(100,100,100,0xff));
		}
	}
	
	void drawSequenceBar(SequenceBar type,float x){
		switch(type){
			case SEQ_BAR_NORMAL:
				seqFramesBar.drawRectCentered(x,18,sf::Color(100,100,100,0xff));
				
				break;
			case SEQ_BAR_HIGHLIGHTED:
				seqFramesBar.drawRectCentered(x,18,sf::Color(160,100, 10,0xff));
				
				break;
			case SEQ_BAR_CURSOR:
				seqFramesBar.drawRectCentered(x, 9,sf::Color(100,  0,150,0xff));
				
				break;
			default:
				break;
		}
	}
	
	void drawTime(temporal::val time,char background){
		sprintf(textBuffer,"%03u",time);
		
		drawTime(textBuffer,background);
	}
	
	void drawTime(const char *buffer,char background){
		// Get and sanitize length
		unsigned int len = strlen(buffer);
		len = (len > TIME_DISPLAY_LEN ? TIME_DISPLAY_LEN : len);
		
		// Move to a length of TIME_DISPLAY_LEN
		memmove(textBuffer + (TIME_DISPLAY_LEN - len),buffer,len);
		
		// Pad beginning with background
		for(unsigned int i = 0;i < (TIME_DISPLAY_LEN - len);++i){
			textBuffer[i] = background;
		}
		
		// Insert decimal into string and format
		textBuffer[TIME_DISPLAY_LEN + 2] = '\0';
		textBuffer[TIME_DISPLAY_LEN + 1] = 's';
		textBuffer[TIME_DISPLAY_LEN + 0] = textBuffer[TIME_DISPLAY_LEN - 1]; // ten-thousandths
		textBuffer[TIME_DISPLAY_LEN - 1] = textBuffer[TIME_DISPLAY_LEN - 2]; // thousandths
		textBuffer[TIME_DISPLAY_LEN - 2] = textBuffer[TIME_DISPLAY_LEN - 3]; // hundredths
		textBuffer[TIME_DISPLAY_LEN - 3] = textBuffer[TIME_DISPLAY_LEN - 4]; // tens
		textBuffer[TIME_DISPLAY_LEN - 4] = '.';
		
		// Draw
		sf::Text display(std::string(textBuffer),font,TEXT_SIZE);
		display.setPosition(TIME_DISPLAY_X_MARGIN,TIME_DISPLAY_TOP + TIME_DISPLAY_Y_MARGIN);
		
		window->draw(display);
	}
	
	void drawSequenceMarker(bool highlighted,int x){
		seqMarker.setPosition(winWidth / 2.0 + x * (SEQ_MARKER_X_MARGIN + SEQ_MARKER_DIAMETER),SEQ_MARKER_TOP + SEQ_MARKER_Y_MARGIN);
		seqMarker.setFillColor(highlighted ? sf::Color(161,125,50,0xff) : sf::Color(160,160,160,0xff));
		
		window->draw(seqMarker);
	}
	
	void drawCharacterLabels(unsigned int current){
		characterLabelsBackground.setSize(sf::Vector2f(CHARACTER_LABEL_WIDTH,CHARACTER_LABEL_Y_MARGIN + chars::count() * (TEXT_SIZE + CHARACTER_LABEL_Y_MARGIN)));
		characterLabelsBackground.setPosition(0.0,CHARACTER_LABEL_TOP);
		
		window->draw(characterLabelsBackground);
		
		for(unsigned int i = 0;i < chars::count();++i){
			float x = CHARACTER_LABEL_X_MARGIN;
			float y = CHARACTER_LABEL_TOP + CHARACTER_LABEL_Y_MARGIN + i * (CHARACTER_LABEL_Y_MARGIN + TEXT_SIZE);
			
			if(i == current){
				characterLabelsSelection.setSize(sf::Vector2f(CHARACTER_LABEL_WIDTH,TEXT_SIZE + CHARACTER_LABEL_Y_MARGIN));
				characterLabelsSelection.setPosition(0.0,y - CHARACTER_LABEL_Y_MARGIN / 2.0);
				
				window->draw(characterLabelsSelection);
			}
			
			sf::Text label(std::string(chars::label(i)),font,TEXT_SIZE);
			label.setPosition(x,y);
			
			window->draw(label);
		}
	}
	
	void drawSetLabel(unsigned int character,unsigned int property){
		sprintf(textBuffer,"%2u: %s",property,chars::sets::label(character,property));
		
		sf::Text label(std::string(textBuffer),font,TEXT_SIZE);
		label.setPosition(PARAMETER_X_MARGIN,winHeight - PARAMETER_TOP + PARAMETER_Y_MARGIN);
		
		window->draw(label);
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
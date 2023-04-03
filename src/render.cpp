#include <cstdlib>
#include <cmath>
#include <cstring>

#include "render.hpp"

#include "renderers/main.hpp"

#define TEXT_SIZE 18

// Top UI
#define TIME_DISPLAY_TOP 0
#define TIME_DISPLAY_WIDTH 140
#define TIME_DISPLAY_X_MARGIN 15
#define TIME_DISPLAY_Y_MARGIN 10

#define SCENE_TOP 0
#define SCENE_HEIGHT (TIME_DISPLAY_Y_MARGIN + TEXT_SIZE + TIME_DISPLAY_Y_MARGIN)

#define SEQ_MARKER_TOP (SCENE_HEIGHT)
#define SEQ_MARKER_DIAMETER 24
#define SEQ_MARKER_X_MARGIN 25
#define SEQ_MARKER_Y_MARGIN 12
#define SEQ_MARKER_HEIGHT (SEQ_MARKER_Y_MARGIN + SEQ_MARKER_DIAMETER + SEQ_MARKER_Y_MARGIN)

#define RENDERER_LABEL_TOP (SEQ_MARKER_TOP + SEQ_MARKER_HEIGHT)
#define RENDERER_LABEL_WIDTH 200
#define RENDERER_LABEL_X_MARGIN 15
#define RENDERER_LABEL_Y_MARGIN 10

// Bottom UI
#define SCROLL_HEIGHT 8
#define SCROLL_TOP SCROLL_HEIGHT

#define SEQ_FRAME_HEIGHT 27
#define SEQ_FRAME_TOP (SCROLL_HEIGHT + SEQ_FRAME_HEIGHT)

#define PARAMETER_LABEL_WIDTH 275
#define TRANSITION_LABEL_WIDTH 200

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
	
	namespace UI{
		class Bar{
			private:
				bool fromTop;
				unsigned int relTop,height;
				unsigned int lPadding,rPadding;
				
				float top() const{
					return fromTop ? relTop : winHeight - relTop;
				}
				
			public:
				Bar(bool isFromTop,unsigned int newRelTop,unsigned int newHeight,unsigned int newLPadding,unsigned int newRPadding)
				:fromTop(isFromTop),relTop(newRelTop),height(newHeight),lPadding(newLPadding),rPadding(newRPadding){
					
				}
				
				bool isInside(int x,int y) const{
					return (y > top()) && (y < top() + (int)height);
				}
				
				float interpolated(float x)const {
					float val = (float)(x - lPadding) / (float)(winWidth - lPadding - rPadding);
					
					return val < 0.0 ? 0.0 : (val > 1.0 ? 1.0 : val);
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
		
		Bar scenesBar       (true , SCENE_TOP     , SCENE_HEIGHT     , TIME_DISPLAY_WIDTH    , 0                      );
		Bar scrollBar       (false, SCROLL_TOP    , SCROLL_HEIGHT    , 0                     , 0                      );
		Bar seqFramesBar    (false, SEQ_FRAME_TOP , SEQ_FRAME_HEIGHT , 0                     , 0                      );
		Bar setParamaterBar (false, PARAMETER_TOP , PARAMETER_HEIGHT , PARAMETER_LABEL_WIDTH , TRANSITION_LABEL_WIDTH );
		
		sf::Font font;
		char textBuffer[128];
		
		sf::CircleShape seqMarker;
		sf::RectangleShape rendererLabelsBackground,rendererLabelsSelection;
		
		int cursorX,cursorY;
	}
	
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
		
		if(!UI::font.loadFromFile(std::string(fontFile))){
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
		UI::seqMarker.setRadius(SEQ_MARKER_DIAMETER / 2.0);
		UI::seqMarker.setOrigin(SEQ_MARKER_DIAMETER / 2.0,0.0);
		
		UI::rendererLabelsBackground.setFillColor(sf::Color(200,200,200,0xff));
		UI::rendererLabelsSelection.setFillColor(sf::Color(161,125,50,0xff));
		
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
	
	namespace UI{
		void updateCursorPos(int x,int y){
			cursorX = x;
			cursorY = y;
		}
		
		void drawBackground(){
			scenesBar.drawBackground(sf::Color(100,100,100,0xff));
			
			scrollBar.drawBackground(sf::Color(80,80,80,0xff));
			seqFramesBar.drawBackground(sf::Color(150,150,150,0xff));
			setParamaterBar.drawBackground(sf::Color(100,100,100,0xff));
		}
		
		namespace bars{
			bool insideScenes(){
				return scenesBar.isInside(cursorX,cursorY);
			}
			
			bool insideSeqFrames(){
				return seqFramesBar.isInside(cursorX,cursorY);
			}
			
			bool insideSetParameter(){
				return setParamaterBar.isInside(cursorX,cursorY);
			}
			
			float cursorValScenes(){
				return scenesBar.interpolated(cursorX);
			}
			
			float cursorValSeqFrames(){
				return seqFramesBar.interpolated(cursorX);
			}
			
			float cursorValSetParameter(){
				return setParamaterBar.interpolated(cursorX);
			}
		}
		
		namespace markers{
			// Top UI -------------------------
			void drawScene(float begin,float end,bool highlighted,bool drawCap){
				scenesBar.drawRectInterval(begin,end,highlighted ? sf::Color(161,125,50,0xff) : sf::Color(160,160,160,0xff));
				
				if(drawCap){
					scenesBar.drawRectCentered(end,4,sf::Color(100,100,100,0xff));
				}
			}
			
			void drawScenesPlayTime(float begin,float end){
				scenesBar.drawRectInterval(begin,end,sf::Color(100,0,150,120));
			}
			
			void drawScenesCursor(float x){
				scenesBar.drawRectCentered(x,4,sf::Color(100,0,150,0xff));
			}
			
			void drawSequence(bool highlighted,int x){
				seqMarker.setPosition(winWidth / 2.0 + x * (SEQ_MARKER_X_MARGIN + SEQ_MARKER_DIAMETER),SEQ_MARKER_TOP + SEQ_MARKER_Y_MARGIN);
				seqMarker.setFillColor(highlighted ? sf::Color(161,125,50,0xff) : sf::Color(160,160,160,0xff));
				
				window->draw(seqMarker);
			}
			
			// Bottom UI ----------------------
			void drawScroll(float begin,float end){
				scrollBar.drawRectInterval(begin,end,sf::Color(200,200,200,0xff));
			}
			
			void drawSeqFrame(SequenceFrameMarker type,float x){
				switch(type){
					case SEQ_FRAME_NORMAL:
						seqFramesBar.drawRectCentered(x,18,sf::Color(100,100,100,0xff));
						
						break;
					case SEQ_FRAME_HIGHLIGHTED:
						seqFramesBar.drawRectCentered(x,18,sf::Color(160,100, 10,0xff));
						
						break;
					case SEQ_FRAME_CURSOR:
						seqFramesBar.drawRectCentered(x, 9,sf::Color(100,  0,150,0xff));
						
						break;
					default:
						break;
				}
			}
			
			void drawSetParameterBackground(){
				setParamaterBar.drawRectInterval(0.0,1.0,sf::Color(160,160,160,0xff));
			}
			
			void drawSetParameterValue(float val,bool cursor){
				setParamaterBar.drawRectInterval(0.5,(val + 1.0) / 2.0,cursor ? sf::Color(100,  0,150,0xff) : sf::Color(160,100, 10,0xff));
			}
			
			void drawSetParameterZero(){
				setParamaterBar.drawRectCentered(0.5,4,sf::Color(100,100,100,0xff));
			}
		}
		
		namespace labels{
			// Top UI -------------------------
			void padBuffer(const char *str,unsigned int minLen,char background){
				// Ensure minimum length
				unsigned int len = strlen(str);
				minLen = len > minLen ? len : minLen;
				
				// Move to minimum length
				memmove(textBuffer + (minLen - len),str,len + 1);
				
				// Pad beginning with background
				for(unsigned int i = 0;i < (minLen - len);++i){
					textBuffer[i] = background;
				}
			}
			
			void drawTimeBuffer(){
				sf::Text display(std::string(textBuffer),font,TEXT_SIZE);
				display.setPosition(TIME_DISPLAY_X_MARGIN,TIME_DISPLAY_TOP + TIME_DISPLAY_Y_MARGIN);
				
				window->draw(display);
			}
			
			void drawTime(frames::val time,unsigned int minLen,char background){
				textBuffer[0] = '\0';
				frames::str::appendFrames(textBuffer,time);
				
				padBuffer(textBuffer,minLen,background);
				drawTimeBuffer();
			}
			
			void drawTime(const char *str,unsigned int minLen,char background){
				padBuffer(str,minLen,background);
				
				unsigned int len = strlen(textBuffer);
				textBuffer[len + 0] = 'f';
				textBuffer[len + 1] = '\0';
				
				drawTimeBuffer();
			}
			
			void drawRenderers(unsigned int currentRenderer){
				rendererLabelsBackground.setSize(sf::Vector2f(RENDERER_LABEL_WIDTH,RENDERER_LABEL_Y_MARGIN + renderers::count() * (TEXT_SIZE + RENDERER_LABEL_Y_MARGIN)));
				rendererLabelsBackground.setPosition(0.0,RENDERER_LABEL_TOP);
				
				window->draw(rendererLabelsBackground);
				
				for(unsigned int i = 0;i < renderers::count();++i){
					float x = RENDERER_LABEL_X_MARGIN;
					float y = RENDERER_LABEL_TOP + RENDERER_LABEL_Y_MARGIN + i * (RENDERER_LABEL_Y_MARGIN + TEXT_SIZE);
					
					if(i == currentRenderer){
						rendererLabelsSelection.setSize(sf::Vector2f(RENDERER_LABEL_WIDTH,TEXT_SIZE + RENDERER_LABEL_Y_MARGIN));
						rendererLabelsSelection.setPosition(0.0,y - RENDERER_LABEL_Y_MARGIN / 2.0);
						
						window->draw(rendererLabelsSelection);
					}
					
					sf::Text label(std::string(renderers::label(i)),font,TEXT_SIZE);
					label.setPosition(x,y);
					
					window->draw(label);
				}
			}
			
			// Bottom UI ----------------------
			void drawSetParameter(unsigned int renderer,unsigned int property){
				sprintf(textBuffer,"%2u: %s",property,renderers::sets::label(renderer,property));
				
				sf::Text label(std::string(textBuffer),font,TEXT_SIZE);
				label.setPosition(PARAMETER_X_MARGIN,winHeight - PARAMETER_TOP + PARAMETER_Y_MARGIN);
				
				window->draw(label);
			}
			
			void drawTransition(Transition t){
				const char *received = labelTransition(t);
				
				sf::Text label(std::string(received == NULL ? "---" : received),font,TEXT_SIZE);
				label.setPosition(winWidth - TRANSITION_LABEL_WIDTH + PARAMETER_X_MARGIN,winHeight - PARAMETER_TOP + PARAMETER_Y_MARGIN);
				
				window->draw(label);
			}
			
			void drawCommandLine(const char *prefix,const char *str){
				sf::Text label(std::string(prefix) + std::string(str),font,TEXT_SIZE);
				label.setPosition(PARAMETER_X_MARGIN,winHeight - PARAMETER_TOP - PARAMETER_Y_MARGIN - TEXT_SIZE);
				
				window->draw(label);
			}
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
		
		bool writeTo(const char *out){
			return view.getTexture().copyToImage().saveToFile(std::string(out));
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
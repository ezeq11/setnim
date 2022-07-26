#ifndef RENDER_INCLUDED
	#include <SFML/Graphics.hpp>
	
	#include "types.hpp"
	
	namespace render{
		bool init(sf::RenderWindow &renderWindow,const sf::Vector2u &innerDimensions,const char *fontFile);
		
		void resize(float width,float height);
		sf::Vector2u innerDimensions();
		
		void clear();
		void drawView();
		
		namespace UI{
			void updateCursorPos(int x,int y);
			void drawBackground();
			
			namespace bars{
				bool insideScenes();
				bool insideSeqFrames();
				bool insideSetParameter();
				
				float cursorValScenes();
				float cursorValSeqFrames();
				float cursorValSetParameter();
			}
			
			namespace markers{
				// Top UI
				void drawScene(float begin,float end,bool highlighted,bool drawCap);
				void drawSequence(bool highlighted,int x);
				
				// Bottom UI
				enum SequenceFrameMarker{
					SEQ_FRAME_NORMAL,
					SEQ_FRAME_HIGHLIGHTED,
					SEQ_FRAME_CURSOR
				};
				
				void drawScroll(float begin,float end);
				void drawSeqFrame(SequenceFrameMarker type,float x);
				
				void drawSetParameterBackground();
				void drawSetParameterValue(float val,bool cursor);
				void drawSetParameterZero();
			}
			
			namespace labels{
				// Top UI
				void drawTime(temporal::val time,unsigned int minLen,char background);
				void drawTime(const char *buffer,unsigned int minLen,char background);
				
				void drawRenderers(unsigned int currentRenderer);
				
				// Bottom UI
				void drawSetParameter(unsigned int renderer,unsigned int property);
			}
		}
		
		namespace view{
			void clear();
			
			void draw(const sf::Drawable &drawable,const sf::BlendMode &blendMode,const sf::Transform &transform);
			void draw(const sf::Drawable &drawable,const sf::Transform &transform);
			
			void drawTemp(const sf::Transform &transform);
			void drawBounds();
			
			namespace temp{
				void clear();
				
				void draw(const sf::Drawable &drawable,const sf::BlendMode &blendMode,const sf::Transform &transform);
				void draw(const sf::Drawable &drawable,const sf::Transform &transform);
			}
		}
	}
	
	#define RENDER_INCLUDED
#endif
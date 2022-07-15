#ifndef RENDER_INCLUDED
	#include <SFML/Graphics.hpp>
	
	#include "timeView.hpp"
	#include "types.hpp"
	
	namespace render{
		bool init(sf::RenderWindow &renderWindow,const sf::Vector2u &innerDimensions,const char *fontFile);
		
		void resize();
		sf::Vector2u innerDimensions();
		
		void clear();
		
		enum SequenceBar{
			SEQ_BAR_NORMAL,
			SEQ_BAR_HIGHLIGHTED,
			SEQ_BAR_CURSOR
		};
		
		void drawView();
		void drawUIBackground(float scrollBegin,float scrollEnd);
		
		void drawSceneMarker(bool highlighted,int x);
		void drawSequenceBar(SequenceBar type,float x);
		void drawRenderableMarker(bool highlighted,int x);
		
		void drawTime(temporal::val time,char background);
		void drawTime(const char *buffer,char background);
		
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
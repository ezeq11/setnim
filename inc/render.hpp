#ifndef RENDER_INCLUDED
	#include <SFML/Graphics.hpp>
	
	namespace render{
		bool init(sf::RenderWindow &renderWindow,const sf::Vector2u &innerDimension);
		void resize();
		
		void clear();
		
		void drawView();
		
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
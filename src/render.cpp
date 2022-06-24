#include <cstdlib>
#include <cmath>

#include "render.hpp"

#define VIEW_TOP_BOTTOM_PADDING 100

namespace render{
	// Variables --------------------------------------------------
	sf::RenderWindow *window;
	sf::Vector2u dimension;
	
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
		
		sf::Vector2f viewBoundsSize = sf::Vector2f(dimension.x - 10,dimension.y - 10);
		
		view::viewBounds.setSize(viewBoundsSize);
		view::viewBounds.setOrigin(viewBoundsSize / 2.0f);
		
		view::viewBounds.setFillColor(sf::Color(0,0,0,0));
		view::viewBounds.setOutlineColor(sf::Color(0,0,0,0xff));
		view::viewBounds.setOutlineThickness(5.0);
		
		return true;
	}
	
	void resize(){
		float scale = (window->getView().getSize().y - VIEW_TOP_BOTTOM_PADDING) / dimension.y;
		
		view::viewSprite.setScale(sf::Vector2f(scale,scale));
	}
	
	void clear(){
		window->clear(sf::Color(0,0,0,0));
	}
	
	void drawView(){
		window->draw(view::viewSprite);
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
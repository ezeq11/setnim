#include "timeView.hpp"

namespace timeView{
	const float ZOOM_MIN = 1.0;
	const float ZOOM_MAX = 10.0;
	
	const float OFFSET_MIN = 0.0;
	const float OFFSET_MAX = 1.0;
	
	float zoom = ZOOM_MIN;
	float offset = OFFSET_MIN;
	float width;
	
	void checkAndUpdate(){
		zoom = zoom < ZOOM_MIN ? ZOOM_MIN : (zoom > ZOOM_MAX ? ZOOM_MAX : zoom);
		width = 1.0 / zoom;
		
		offset = getBegin() < OFFSET_MIN ? OFFSET_MIN + width / 2.0 : (getEnd() > OFFSET_MAX ? OFFSET_MAX - width / 2.0 : offset);
	}
	
	void init(){
		checkAndUpdate();
	}
	
	void zoomBy(float delta){
		zoom += delta;
		checkAndUpdate();
	}
	
	void translateBy(float delta){
		offset += delta;
		checkAndUpdate();
	}
	
	float getBegin(){
		return offset - width / 2.0;
	}
	
	float getEnd(){
		return offset + width / 2.0;
	}
}
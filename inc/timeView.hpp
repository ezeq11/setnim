#ifndef TIMEVIEW_INCLUDED
	namespace timeView{
		void init();
		
		void zoomBy(float delta);
		void translateBy(float delta);
		
		float getBegin();
		float getEnd();
	}
	
	#define TIMEVIEW_INCLUDED
#endif
#ifndef TIMEVIEW_INCLUDED
	#define TIME_DISPLAY_LEN 6
	
	namespace timeView{
		void init();
		
		void zoomBy(float delta);
		void translateBy(float delta);
		
		float getBegin();
		float getEnd();
	}
	
	#define TIMEVIEW_INCLUDED
#endif
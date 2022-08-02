#ifndef ANIMATION_INCLUDED
	#include "iterable.hpp"
	#include "setsequence.hpp"
	
	class Scene: public PointerIterable<SetSequence>{
		private:
			frames::val length;
			
		public:
			Scene(frames::val newLength);
			Scene(FILE *in);
			void write(FILE *out) const;
			
			frames::val getLength() const;
			void setLength(frames::val newLength);
	};
	
	class Animation{
		private:
			PointerIterable<Scene> scenes;
			
		public:
			Animation();
			Animation(FILE *in);
			void write(FILE *out) const;
			
			
			
			void sceneAddBefore(frames::val length);
			void sceneAddAfter(frames::val length);
			void sceneForward();
			void sceneBackward();
			void sceneRemove();
			void sceneClear();
			
			bool sceneAvailable() const;
			frames::val sceneGetLength() const;
			void sceneSetLength(frames::val newLength);
			
			frames::val length() const;
			
			
			
			void seqAddBefore(unsigned int renderIndex);
			void seqAddAfter(unsigned int renderIndex);
			void seqForward();
			void seqBackward();
			void seqRemove();
			void seqClear();
			
			SetSequence *seqCurrent() const;
			
			
			
			void drawMarkers() const;
			void render(void (**renderers)(const Set &,float),frames::val time,bool stillActiveFrame) const;
	};
	
	#define ANIMATION_INCLUDED
#endif
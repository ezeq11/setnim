#ifndef ANIMATION_INCLUDED
	#include "iterable.hpp"
	#include "setsequence.hpp"
	
	class Scene: public PointerIterable<SetSequence>{
		private:
			temporal::val length;
			
		public:
			Scene(temporal::val newLength);
			Scene(FILE *in);
			void write(FILE *out) const;
			
			temporal::val getLength() const;
			void setLength(temporal::val newLength);
	};
	
	class Animation{
		private:
			PointerIterable<Scene> scenes;
			
		public:
			Animation();
			Animation(FILE *in);
			void write(FILE *out) const;
			
			
			
			void sceneAddBefore(temporal::val length);
			void sceneAddAfter(temporal::val length);
			void sceneForward();
			void sceneBackward();
			void sceneRemove();
			void sceneClear();
			
			bool sceneAvailable() const;
			temporal::val sceneGetLength() const;
			void sceneSetLength(temporal::val newLength);
			
			temporal::val length() const;
			
			
			
			void seqAddBefore(unsigned int renderIndex);
			void seqAddAfter(unsigned int renderIndex);
			void seqForward();
			void seqBackward();
			void seqRemove();
			void seqClear();
			
			SetSequence *seqCurrent() const;
			
			
			
			void drawMarkers() const;
			void render(void (**renderers)(const Set &,float),float time,bool stillActiveFrame) const;
	};
	
	#define ANIMATION_INCLUDED
#endif
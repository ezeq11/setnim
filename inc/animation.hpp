#ifndef ANIMATION_INCLUDED
	#include "iterable.hpp"
	#include "renderable.hpp"
	#include "setsequence.hpp"
	
	class Scene: public PointerIterable<SetSequence>{
		private:
			temporal::val length;
			
		public:
			Scene(temporal::val newLength);
			Scene(FILE *in,Renderable **buffers,unsigned int bufferCount);
			void write(FILE *out,Renderable **buffers,unsigned int bufferCount) const;
			
			temporal::val getLength() const;
			void setLength(temporal::val newLength);
	};
	
	class Animation{
		private:
			PointerIterable<Renderable> renderables;
			PointerIterable<Scene> scenes;
			
		public:
			Animation();
			Animation(FILE *in);
			void write(FILE *out) const;
			
			
			
			void renderablesAddBefore(unsigned int renderIndex);
			void renderablesAddAfter(unsigned int renderIndex);
			void renderablesForward();
			void renderablesBackward();
			void renderablesRemove();
			void renderablesClear();
			
			
			
			void sceneAddBefore(temporal::val length);
			void sceneAddAfter(temporal::val length);
			void sceneForward();
			void sceneBackward();
			void sceneRemove();
			void sceneClear();
			
			temporal::val sceneGetLength() const;
			void sceneSetLength(temporal::val newLength);
			
			temporal::val length() const;
			
			
			
			void seqAddBefore();
			void seqAddAfter();
			void seqForward();
			void seqBackward();
			void seqRemove();
			void seqClear();
			
			SetSequence *seqCurrent() const;
			
			
			
			void drawMarkers() const;
			void render(float time,bool stillActiveFrame) const;
	};
	
	#define ANIMATION_INCLUDED
#endif
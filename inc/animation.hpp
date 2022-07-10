#ifndef ANIMATION_INCLUDED
	#include "iterable.hpp"
	#include "renderable.hpp"
	#include "setsequence.hpp"
	
	class SceneRenderSeq{
		private:
			Renderable *r;
			SetSequence<float> *s;
			
		public:
			SceneRenderSeq(Renderable *newR,SetSequence<float> *newS);
			~SceneRenderSeq();
			
			Renderable *getRenderable() const;
			SetSequence<float> *getSeq() const;
	};
	
	class Scene: public PointerIterable<SceneRenderSeq>{
		private:
			temporal::val time;
			
		public:
			Scene(temporal::val newTime);
			
			temporal::val getTime() const;
			void setTime(temporal::val newTime);
	};
	
	class Animation{
		private:
			PointerIterable<Renderable> renderables;
			PointerIterable<Scene> scenes;
			
		public:
			void renderablesAddBefore(Renderable *newRenderable);
			void renderablesAddAfter(Renderable *newRenderable);
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
			
			SetSequence<float> *seqCurrent() const;
			
			
			
			void drawMarkers() const;
			void render(float time,bool stillActiveFrame) const;
	};
	
	#define ANIMATION_INCLUDED
#endif
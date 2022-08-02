#ifndef TYPES_INCLUDED
	#include <cstdint>
	#include <cstdio>
	
	namespace uInt32{
		void write(FILE *out,uint32_t v);
		uint32_t read(FILE *in);
	}
	
	namespace uInt16{
		void write(FILE *out,uint16_t v);
		uint16_t read(FILE *in);
	}
	
	namespace frames{
		typedef uint32_t val;
		extern unsigned int perSec;
		
		float toFloat(val v);
		val fromFloat(float v);
		
		void write(FILE *out,val v);
		val read(FILE *in);
		
		namespace str{
			void appendFrames(char *str,val v);
			void appendSecondsFrames(char *str,val v);
		}
	}
	
	namespace normalizedUInt16{
		typedef uint16_t val;
		
		float toFloat(val v);
		val fromFloat(float v);
		
		void write(FILE *out,val v);
		val read(FILE *in);
	}
	
	namespace normalizedInt16{
		typedef int16_t val;
		
		float toFloat(val v);
		val fromFloat(float v);
		
		void write(FILE *out,val v);
		val read(FILE *in);
	}
	
	#define TYPES_INCLUDED
#endif
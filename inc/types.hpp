#ifndef TYPES_INCLUDED
	#include <cstdint>
	#include <cstdio>
	
	namespace temporal{
		typedef uint32_t val;
		
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
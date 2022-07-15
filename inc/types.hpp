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
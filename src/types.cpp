#include "types.hpp"

unsigned char get(FILE *in){
	int c = fgetc(in);
	
	return c == EOF ? 0 : c;
}

namespace temporal{
	float toFloat(val v){
		return (float)v / 100;
	}
	
	val fromFloat(float v){
		return v * 100;
	}
	
	void write(FILE *out,val v){
		fputc((v >> 24) & 0xff,out);
		fputc((v >> 16) & 0xff,out);
		fputc((v >> 8 ) & 0xff,out);
		fputc((v >> 0 ) & 0xff,out);
	}
	
	val read(FILE *in){
		uint32_t u1 = get(in);
		uint32_t u2 = get(in);
		uint32_t u3 = get(in);
		uint32_t u4 = get(in);
		
		return (u1 << 24) | (u2 << 16) | (u3 << 8) | u4;
	}
}

namespace normalizedInt16{
	float toFloat(val v){
		return v < 0 ? -(double)v / (double)INT16_MIN : (double)v / (double)INT16_MAX;
	}
	
	val fromFloat(float v){
		return v < 0 ? -(double)v * (double)INT16_MIN : (double)v * (double)INT16_MAX;
	}
	
	void write(FILE *out,val v){
		uint16_t uv = (int32_t)v - (int32_t)INT16_MIN;
		
		fputc((uv >> 8) & 0xff,out);
		fputc((uv >> 0) & 0xff,out);
	}
	
	val read(FILE *in){
		uint16_t u1 = get(in);
		uint16_t u2 = get(in);
		
		uint16_t uv = (u1 << 8) | u2;
		
		return (int32_t)uv + (int32_t)INT16_MIN;
	}
}
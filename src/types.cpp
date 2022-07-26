#include <cstring>

#include "types.hpp"

unsigned char get(FILE *in){
	int c = fgetc(in);
	
	return c == EOF ? 0 : c;
}

namespace uInt32{
	void write(FILE *out,uint32_t v){
		fputc((v >> 24) & 0xff,out);
		fputc((v >> 16) & 0xff,out);
		fputc((v >> 8 ) & 0xff,out);
		fputc((v >> 0 ) & 0xff,out);
	}
	
	uint32_t read(FILE *in){
		uint32_t u1 = get(in);
		uint32_t u2 = get(in);
		uint32_t u3 = get(in);
		uint32_t u4 = get(in);
		
		return (u1 << 24) | (u2 << 16) | (u3 << 8) | u4;
	}
}

namespace uInt16{
	void write(FILE *out,uint16_t v){
		fputc((v >> 8) & 0xff,out);
		fputc((v >> 0) & 0xff,out);
	}
	
	uint16_t read(FILE *in){
		uint16_t u1 = get(in);
		uint16_t u2 = get(in);
		
		return (u1 << 8) | u2;
	}
}

namespace temporal{
	float toFloat(val v){
		return (float)v / 10000;
	}
	
	val fromFloat(float v){
		return v * 10000;
	}
	
	void write(FILE *out,val v){
		uInt32::write(out,v);
	}
	
	val read(FILE *in){
		return uInt32::read(in);
	}
	
	namespace str{
		void appendDigits(char *str,val v){
			sprintf(str + strlen(str),"%05u",v);
		}
		
		void punctuate(char *str){
			unsigned int len = strlen(str);
			
			str[len + 1] = '\0';
			str[len + 0] = str[len - 1]; // ten-thousandths
			str[len - 1] = str[len - 2]; // thousandths
			str[len - 2] = str[len - 3]; // hundredths
			str[len - 3] = str[len - 4]; // tens
			str[len - 4] = '.';
		}
		
		void appendUnit(char *str){
			unsigned int len = strlen(str);
			
			str[len + 1] = '\0';
			str[len + 0] = 's';
		}
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
		
		uInt16::write(out,uv);
	}
	
	val read(FILE *in){
		uint16_t uv = uInt16::read(in);
		
		return (int32_t)uv + (int32_t)INT16_MIN;
	}
}
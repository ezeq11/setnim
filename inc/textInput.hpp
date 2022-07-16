#ifndef TEXT_INPUT_INCLUDED
	template <unsigned int size,bool (*allowable)(char)>
	class TextInput{
		private:
			char chars[size + 1];
			unsigned int current;
			
		public:
			TextInput()
			:chars{},current(0){
				clear();
			}
			
			void clear(){
				current = 0;
				chars[current] = '\0';
			}
			
			bool interpret(char c){
				if(c == '\b'){
					if(current > 0){
						--current;
					}
					
					chars[current] = '\0';
					
					return false;
					
				}else if(c == '\n' || c == '\r'){
					chars[current] = '\0';
					
					return true;
					
				}else if(allowable(c)){
					if(current < size){
						chars[current] = c;
						
						++current;
						chars[current] = '\0';
					}
					
					return false;
				}
				
				return false;
			}
			
			const char *buffer() const{
				return chars;
			}
	};
	
	#define TEXT_INPUT_INCLUDED
#endif
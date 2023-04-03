EXEC_NAME := ghst

C_FLAGS := -Wall -g -DSFML_STATIC -DGLEW_STATIC $(if $(findstring Windows_NT,$(OS)),) -static-libgcc -static-libstdc++

SRCDIR := src
INCDIR := inc
OBJDIR := obj
BINDIR := bin

LOCAL_LIB_NAMES :=

INCS := -I$(INCDIR) $(LOCAL_LIB_NAMES:%=-I../libs/%/inc) -IC:/C/SFML-2.5.1/include $(if $(findstring Windows_NT,$(OS)),-IC:/C/glfw-3.3.bin.WIN32/include -IC:/C/glew-2.1.0/include)
LIBS := $(LOCAL_LIB_NAMES:%=-L../libs/%/bin) $(LOCAL_LIB_NAMES:%=-l%_$(if $(findstring Windows_NT,$(OS)),w,l)) -LC:/C/SFML-2.5.1/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s
LIB_DEPENDS := $(if $(findstring Windows_NT,$(OS)),-LC:/C/glfw-3.3.bin.WIN32/lib-mingw -LC:/C/glew-2.1.0/lib/Release/Win32 -lfreetype -lws2_32 -lglfw3 -lglew32s -lgdi32 -lwinmm -lglu32 -lopengl32,-lm -lglfw3 -lGLEW -lGL -lGLU -lX11 -lXxf86vm -lXrandr -lpthread -lXi -ldl -lXinerama -lXcursor)

SOURCES := $(wildcard $(SRCDIR)/*.cpp $(SRCDIR)/renderers/*.cpp)
OBJECTS := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(BINDIR)/$(EXEC_NAME)

.PHONY: clean

$(BINDIR)/$(EXEC_NAME): $(OBJECTS)
	g++ $(C_FLAGS) $^ -o $@ $(LIBS) $(LIB_DEPENDS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(INCDIR)/%.hpp
	g++ -c $(C_FLAGS) $< -o $@ $(INCS)

clean:
ifeq ($(OS),Windows_NT)
	-del $(subst /,\,$(OBJECTS))
	-del $(BINDIR)\$(EXEC_NAME).exe
else
	-rm $(OBJECTS)
	-rm $(BINDIR)/$(EXEC_NAME)
endif
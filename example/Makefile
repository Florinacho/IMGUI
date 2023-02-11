ifeq ($(OS),Windows_NT)
	CC=mingw32-g++.exe
	OUTPUT=../bin/main.exe
else
	CC=g++
	OUTPUT=../bin/main.out
endif

INCLUDE=../source ../source/WindowCanvas/source
LIB_DIRS=../lib ../source/WindowCanvas/lib
LIB_FILES=wcanvas imgui
C_FLAGS=-O3 -g3 -Wall
L_FLAGS=
C_FILES=main.cpp

C_FLAGS+=$(addprefix -I, $(INCLUDE))
L_FLAGS+=$(addprefix -L, $(LIB_DIRS)) $(addprefix -l, $(LIB_FILES)) 
L_FILES=$(C_FILES:.cpp=.o)

build: $(OUTPUT)

clean:
	rm -rf $(L_FILES) $(OUTPUT)

rebuild: clean build

libwcanvas.a:
	make -C ../source/WindowCanvas

%.o: %.cpp
	$(CC) $(C_FLAGS) -c $< -o $@

%.out: libwcanvas.a $(L_FILES)
	$(CC) $(L_FILES) $(L_FLAGS) -o $(OUTPUT)
	chmod +xr $(OUTPUT)
	
%.exe: libwcanvas.a $(L_FILES)
	$(CC) $(L_FILES) $(L_FLAGS) -o $(OUTPUT)
	
.PHONY:
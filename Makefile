CC=g++
OUTPUT=main.out

INCLUDE=./ ./WindowCanvas/source
LIB_DIRS=WindowCanvas/lib
LIB_FILES=wcanvas
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
	make -C WindowCanvas

%.o: %.cpp
	$(CC) $(C_FLAGS) -c $< -o $@

%.out: libwcanvas.a $(L_FILES)
	$(CC) $(L_FILES) $(L_FLAGS) -o $(OUTPUT)
	chmod +xr $(OUTPUT)
	
%.exe: libwcanvas.a $(L_FILES)
	$(CC) $(L_FILES) $(L_FLAGS) -o $(OUTPUT)
	
.PHONY:

# Compiler:
CC = g++-4.9

# Optimization:
CFLAGS = -O3

# Libs and Frameworks:
FRAMEWORKS = -lglew -lglfw3 -lAntTweakBar -lpng -framework Opengl -framework Cocoa -framework IOKit -framework CoreVideo -std=c++11

# Location for libs:
LIBFOLD = -L"/usr/local/lib"
# Location for includes:
INCFOLD = -I"/usr/local/include"

# Files:
FILES = $(wildcard src/*.cpp) $(wildcard shaders/*.cpp) $(wildcard src/utils/*.cpp) $(wildcard src/utils/*.c)

# Binary folder:
BINFOLD = bin/

# Binary name:
BINNAME = Fur


all: compile
.PHONY: all

compile: $(FILES)
	$(CC) $(CFLAGS) $(FILES) -o $(BINFOLD)$(BINNAME) $(LIBFOLD) $(INCFOLD) $(FRAMEWORKS)
.PHONY: compile

run:
	./$(BINFOLD)$(BINNAME)
.PHONY: run

clean:
	rm -f $(BINFOLD)*
.PHONY: clean

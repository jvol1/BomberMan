# game code directory name
GAMEDIR := src

# binary output directory name
OUTPUTDIR := bin

# name for the output(binary) of the game code
GAMENAME := game

# if you want debug information about memory e.g. memory leaks use -fsanitize=address
# add C flags, like: CFLAGS := -Wall -Werror -Wconversion -Wextra
CFLAGS :=  -Wall -O2 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# add load flags, others like -pthread
LDLIB := -lm 

CC := gcc -std=c17
RM := rm -f
MK := mkdir -p

EXT := c

GAMESOURCES := $(shell find $(GAMEDIR) -type f -name *.$(EXT))
GAMEOBJS := $(subst .$(EXT),.o,$(GAMESOURCES))


all: mkdirs buildGAME clean runGAME
build: mkdirs buildGAME clean
run: runGAME

buildGAME: $(GAMEOBJS) 
	$(CC) -o $(OUTPUTDIR)/$(GAMENAME) $(GAMEOBJS) $(LDLIB) $(CFLAGS)

%.o : %.$(EXT)	
	$(CC) -c $< -o $@ $(CFLAGS)

mkdirs:
	$(MK) $(OUTPUTDIR)

clean:
	$(RM) $(GAMEOBJS) 

runGAME:
	./$(OUTPUTDIR)/$(GAMENAME)

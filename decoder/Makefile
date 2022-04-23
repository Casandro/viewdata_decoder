#OBJS specifies which files to compile as part of the project
OBJS = *.c 

#CC specifies which compiler we're using
CC = gcc

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -Wall -lncursesw

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lncursesw


#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = viewdata_ncurses

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

clean :
	rm *.o $(OBJ_NAME)

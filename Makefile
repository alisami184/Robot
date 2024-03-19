# Compiler configuration
USER_DEFINES=-DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -DDO_NOT_USE_SHARED_MEMORY
CC=gcc -O3 $(USER_DEFINES) #-03 is optimisation of the compilation
CC_C = gcc -O3 $(USER_DEFINES) # C compiler files
CFLAG = -lm # .c files compilation flag
CCFLAG = -lpthread -lrt
# MODIFY Set VREP path for include files and 
# required C files in remoteApi folder: extApi.c, extApiPlatform.c
PATH_VREP=/home/td2/Téléchargements/CoppeliaSim_Edu_V4_5_1_rev4_Ubuntu22_04
INCLUDE1 = $(PATH_VREP)/programming/include
INCLUDE2 = $(PATH_VREP)/programming/legacyRemoteApi/remoteApi
INCLUDE = -I$(INCLUDE1) -I$(INCLUDE2)

OUTDIR = output#output directory
BINDIR = bin

#C++ Objects
OBJS = $(OUTDIR)/extApi.o $(OUTDIR)/extApiPlatform.o $(OUTDIR)/main.o #name, that the object files that need to be compiled before, are  going to be saved

#C Objects
C_OBJS = $(OUTDIR)/clientJoy.o $(OUTDIR)/test_retard.o 

# Targets Syntax
# target: prerequisite
# 		rule
# the target is the c 
# All outputs generates           
all: 		server client retard

server: $(OBJS)
	mkdir -p $(OUTDIR) 
	mkdir -p $(BINDIR)
	g++ $? -o $(BINDIR)/$@ $(CCFLAG)

client: $(OUTDIR)/clientJoy.o 
	mkdir -p $(BINDIR) 
	$(CC_C) $? -o $(BINDIR)/$@ $(CFLAG)

retard: $(OUTDIR)/test_retard.o
	mkdir -p $(BINDIR) 
	$(CC_C) $? -o $(BINDIR)/$@ $(CFLAG)

$(OUTDIR)/extApi.o: 	$(INCLUDE2)/extApi.c
			mkdir -p $(OUTDIR)
			$(CC) -c $(INCLUDE) $(INCLUDE2)/extApi.c -o $@
		
$(OUTDIR)/extApiPlatform.o:	$(INCLUDE2)/extApiPlatform.c
			mkdir -p $(OUTDIR)
			$(CC) -c $(INCLUDE) $(INCLUDE2)/extApiPlatform.c -o $@
	
$(OUTDIR)/main.o: main.cpp
			mkdir -p $(OUTDIR)
			$(CC) -c $(INCLUDE) main.cpp -o $@

$(OUTDIR)/clientJoy.o: clientJoy.c
			mkdir -p $(OUTDIR)
			$(CC_C) -c $< -o $@

$(OUTDIR)/test_retard.o: test_retard.c
			mkdir -p $(OUTDIR)
			$(CC_C) -c $< -o $@

clean:
			rm -rf $(OUTDIR) $(BINDIR)

# o $@ directs the output to the target file name 
# $< is an automatic variable that stands for the first prerequisite
# The -L$(variable) is to include a path to the search

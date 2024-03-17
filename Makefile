# Compiler configuration
USER_DEFINES=-DNON_MATLAB_PARSING -DMAX_EXT_API_CONNECTIONS=255 -DDO_NOT_USE_SHARED_MEMORY
CC=gcc -O3 $(USER_DEFINES) #-03 is optimisation of the compilation

# MODIFY Set VREP path for include files and 
# required C files in remoteApi folder: extApi.c, extApiPlatform.c
PATH_VREP=/home/laura/Coppelia/CoppeliaSim_Edu_V4_5_1_rev4_Ubuntu22_04#folder containing the headfiles and source files of Coppelia's Remote API
INCLUDE1 = $(PATH_VREP)/programming/include
INCLUDE2 = $(PATH_VREP)/programming/legacyRemoteApi/remoteApi
INCLUDE = -I$(INCLUDE1) -I$(INCLUDE2)

OUTDIR = output#output directory
BINDIR = bin

EXE = $(BINDIR)/server#Name of the output
OBJS = $(OUTDIR)/extApi.o $(OUTDIR)/extApiPlatform.o $(OUTDIR)/main.o#name, that the object files that need to be compiled before, are  going to be saved

# Targets Syntax
# target: prerequisite
# 		rule
# the target is the c               
all: 		$(OBJS)
			mkdir -p $(OUTDIR)
			mkdir -p $(BINDIR)
			g++ $? -o $(EXE) -lpthread -lrt

$(OUTDIR)/extApi.o: 	$(INCLUDE2)/extApi.c
			mkdir -p $(OUTDIR)
			$(CC) -c $(INCLUDE) $(INCLUDE2)/extApi.c -o $@
			
$(OUTDIR)/extApiPlatform.o:	$(INCLUDE2)/extApiPlatform.c
			mkdir -p $(OUTDIR)
			$(CC) -c $(INCLUDE) $(INCLUDE2)/extApiPlatform.c -o $@
			
$(OUTDIR)/main.o: main.cpp
			mkdir -p $(OUTDIR)
			$(CC) -c $(INCLUDE) main.cpp -o $@
clean:
			rm -rf $(OUTDIR) $(BINDIR)

# o $@ directs the output to the target file name 
# $< is an automatic variable that stands for the first prerequisite
# The -L$(variable) is to include a path to the search
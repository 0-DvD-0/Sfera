CC = g++
CFLAGS = -Wall -c -g -fpermissive 
MYDIR=../build

INCLUDES = -I. -I.. -I$(ROOTSYS)/include -I$(ROOFIT_INCLUDE)/ 

ROOTSYS ?= ERROR_RootSysIsNotDefined

ROOTFLAG = `${ROOTSYS}/bin/root-config --cflags --libs`

EXTRALIBS := -L$(ROOTSYS)/lib -L$(ROOFIT_LIBDIR)/ -lHtml -lMathCore -lGenVector -lMinuit -lEG  -lPhysics -lz -lTreePlayer -lRooFitCore -lRooFit


asciiToTree: asciiToTree_Folder.cpp 
	mkdir -p $(MYDIR)
	$(CC) -Wall $(INCLUDES) -o ../build/asciiToTree asciiToTree_Folder.cpp $(ROOTFLAG) $(EXTRALIBS)

measToTree: measToTree_folder.cpp 
	mkdir -p $(MYDIR)
	$(CC) -Wall $(INCLUDES) -o ../build/measToTree measToTree_folder.cpp $(ROOTFLAG) $(EXTRALIBS)
EverythingToTree: EverythingToTree.cpp
	mkdir -p $(MYDIR)
	$(CC) -Wall $(INCLUDES) -o ../build/EverythingToTree EverythingToTree.cpp $(ROOTFLAG) $(EXTRALIBS)
FilterTiming: FilterTiming.cpp
	mkdir -p $(MYDIR)
	$(CC) -Wall $(INCLUDES) -o ../build/FilterTiming FilterTiming.cpp $(ROOTFLAG) $(EXTRALIBS)
all: asciiToTree measToTree EverythingToTree FilterCobalto


clean:
	rm ../build/asciiToTree ../build/measToTree ../build/EverythingToTree ../build/FilterCobalto



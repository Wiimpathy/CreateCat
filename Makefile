#------------------------------------------------------------------------------
#
#                  createcat Makefile
#
#------------------------------------------------------------------------------

TARGET = createcat


#------------------------------------------------------------------------------
# Defines
#------------------------------------------------------------------------------
#platform = WIN32
#crosscompile = 1

ifndef platform
	platform = UNIX
endif

VERSION_MAJOR = 0
VERSION_MINOR = 2
VERSION_STR = $(VERSION_MAJOR).$(VERSION_MINOR)

ifeq ($(platform), UNIX)
	FINAL_TARGET = $(TARGET)
else
	FINAL_TARGET = $(TARGET).exe
endif
EXTRA_TARGETS = $(FINAL_TARGET)

OBJ = src

#------------------------------------------------------------------------------
# Utilities
#------------------------------------------------------------------------------

ifeq ($(platform), WIN32)
	ifeq ($(crosscompile), 1)
		AR = i686-w64-mingw32-ar
		CC = i686-w64-mingw32-gcc
		CXX = i686-w64-mingw32-g++
		LD = i686-w64-mingw32-g++
		MD = mkdir
		RM = rm
	else
		AR = ar
		CC = gcc
		LD = gcc
		MD = mkdir.exe
		RM = rm.exe
	endif
else ifeq ($(platform), UNIX)
	AR = /usr/bin/x86_64-linux-gnu-ar
	CC = /usr/bin/x86_64-linux-gnu-gcc
	CXX = /usr/bin/x86_64-linux-gnu-g++-8
	LD = /usr/bin/x86_64-linux-gnu-g++-8
	MD = mkdir
	RM = rm
endif

#------------------------------------------------------------------------------
# File include path
#------------------------------------------------------------------------------

INCDIR = \
	src

ifeq ($(crosscompile), 1)
INCDIR += src/include
endif

INCDIR += src/include

#------------------------------------------------------------------------------
# Object Directory
#------------------------------------------------------------------------------

OBJDIRS = \
	$(OBJ)


#------------------------------------------------------------------------------
# Object Files
#------------------------------------------------------------------------------

OBJS = \
	$(OBJ)/categories.o \
	$(OBJ)/common.o \
	$(OBJ)/main.o

#------------------------------------------------------------------------------
# Compiler Defines
#------------------------------------------------------------------------------

ifeq ($(platform), WIN32)
CDEFS = \
	-DCRLF=3 \
	-DWINVER=0x0400 \
	-D_WIN32_WINNT=0x0500 \
	-DWIN32 \
	-D_WINDOWS \
	-DINLINE='static __inline' \
	-Dinline=__inline \
	-D__inline__=__inline \
	-DVERSION_STR='"$(VERSION_STR)"' \
	$(addprefix -I,$(INCDIR))
else
CDEFS = \
	-DCRLF=3 \
	-DINLINE='static __inline' \
	-Dinline=__inline \
	-D__inline__=__inline \
	-DVERSION_STR='"$(VERSION_STR)"' \
	$(addprefix -I,$(INCDIR))
endif

ifeq ($(platform), UNIX)
CDEFS += -DUNIX=1
endif

#------------------------------------------------------------------------------
# Compiler Flags
#------------------------------------------------------------------------------
 
CFLAGS = -Ofast $(CDEFS)
CXXFLAGS = $(CFLAGS) -std=c++17 


#---------------------------------------------------------------------
# Linker Flags
#---------------------------------------------------------------------

LDFLAGS =  -s

ifeq ($(platform), WIN32)
	ifeq ($(crosscompile), 1)
		LDFLAGS += -mconsole
	else
		LDFLAGS += -mconsole
	endif
endif


#------------------------------------------------------------------------------
# Library
#------------------------------------------------------------------------------

ifeq ($(platform), WIN32)
	ifeq ($(crosscompile), 1)
		LIBS = -luser32 -lcomdlg32 -lshell32 -static-libgcc -static-libstdc++ -lstdc++fs
	else
		LIBS = -lstdc++ -luser32 -lcomdlg32 -lshell32 -lstdc++fs
	endif
else
	LIBS = -lstdc++ -lstdc++fs
endif


#------------------------------------------------------------------------------
# Rules to make libraries
#------------------------------------------------------------------------------

all: $(EXTRA_TARGETS)

$(FINAL_TARGET): $(OBJS)
	@echo Linking $@...
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $@


#------------------------------------------------------------------------------
# Rules to manage files
#------------------------------------------------------------------------------

$(OBJ)/%.o: src/%.c
	@echo Compiling $<...
	@$(CC) $(CDEFS) $(CFLAGS) -c $< -o$@

$(OBJ)/%.o: src/%.cpp
	@echo Compiling $<...
	@$(CXX) $(CDEFS) $(CXXFLAGS) -c $< -o$@

clean:
	@$(RM) -rf $(OBJS)
	@$(RM) -f $(FINAL_TARGET)


# Makefile for building the C++ program
# This program has the following dependencies:
# - libjpeg
# - zlib
# - libpng
# The libraries are built from source and linked statically.
# This makefile is tested on Windows and WSL2

# Toolchain
CC			= gcc
CXX			= g++
AR			= ar rc
RANLIB		= ranlib

# Directories
SRC_DIR		= src
LIB_DIR		= lib
BIN_DIR		= bin
LIBJPEG_DIR	= $(LIB_DIR)/libjpeg
LIBZ_DIR	= $(LIB_DIR)/zlib
LIBPNG_DIR	= $(LIB_DIR)/libpng
GIFENCODER_DIR 	= $(LIB_DIR)/GifEncoder
GIFLIB_DIR	= $(GIFENCODER_DIR)/giflib

# Executable and library directory
TARGET		= $(BIN_DIR)/main
LIBJPEG		= $(LIBJPEG_DIR)/libjpeg.a
LIBZ		= $(LIBZ_DIR)/libz.a
LIBPNG		= $(LIBPNG_DIR)/libpng.a
GIFENCODER	= $(GIFENCODER_DIR)/libgifencoder.a

# Source files
SRC_FILES	= $(wildcard src/*.cpp)
LIBJPEG_SRC = $(addprefix $(LIBJPEG_DIR)/, jcapimin.c jcapistd.c jccoefct.c jccolor.c jcdctmgr.c jchuff.c \
        jcinit.c jcmainct.c jcmarker.c jcmaster.c jcomapi.c jcparam.c \
        jcphuff.c jcprepct.c jcsample.c jctrans.c jdapimin.c jdapistd.c \
        jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c jddctmgr.c jdhuff.c \
        jdinput.c jdmainct.c jdmarker.c jdmaster.c jdmerge.c jdphuff.c \
        jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c jfdctfst.c \
        jfdctint.c jidctflt.c jidctfst.c jidctint.c jidctred.c jquant1.c \
        jquant2.c jutils.c jmemmgr.c jmemnobs.c)
LIBZ_SRC	= $(addprefix $(LIBZ_DIR)/, adler32.c compress.c crc32.c deflate.c gzclose.c gzlib.c gzread.c gzwrite.c \
		infback.c inffast.c inflate.c inftrees.c trees.c uncompr.c zutil.c)
LIBPNG_SRC = $(addprefix $(LIBPNG_DIR)/, png.c pngerror.c pngget.c pngmem.c pngpread.c \
       pngread.c pngrio.c pngrtran.c pngrutil.c pngset.c \
       pngtrans.c pngwio.c pngwrite.c pngwtran.c pngwutil.c)
GIFENCODER_SRC = $(addprefix $(GIFENCODER_DIR)/, GifEncoder.cpp algorithm/NeuQuant.cpp)
GIFLIB_SRC = $(addprefix $(GIFLIB_DIR)/, dgif_lib.cpp egif_lib.cpp gif_err.cpp gif_hash.cpp gifalloc.cpp openbsd-reallocarray.cpp)

# Compiler flags
CPPFLAGS			= -I$(SRC_DIR) -I$(LIB_DIR) -I$(LIBJPEG_DIR) -I$(LIBZ_DIR) -I$(LIBPNG_DIR) -I$(GIFENCODER_DIR)
LIBJPEG_CPPFLAGS		= -I$(LIBJPEG_DIR)
LIBZ_CPPFLAGS			= -I$(LIBZ_DIR) -O3 -D_LARGEFILE64_SOURCE=1 -DHAVE_HIDDEN
# LIBPNG flags
NOHWOPT			= -DPNG_ARM_NEON_OPT=0 -DPNG_MIPS_MSA_OPT=0 \
                  -DPNG_POWERPC_VSX_OPT=0 -DPNG_INTEL_SSE_OPT=0
STDC			= -pedantic-errors # -std=c99
WARN			= -Wall -Wextra -Wundef
WARNMORE		= -Wcast-align -Wconversion -Wshadow -Wpointer-arith -Wwrite-strings \
                  -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes
LOCAL_CPPFLAGS	= $(NOHWOPT)
LIBPNG_CPPFLAGS	= -I$(LIBZ_DIR) -I$(LIBPNG_DIR) # -DPNG_DEBUG=5
ALL_CPPFLAGS	= $(LOCAL_CPPFLAGS) $(LIBPNG_CPPFLAGS)
LOCAL_CFLAGS	= $(STDC) $(WARN) # $(WARNMORE)
LIBPNG_CFLAGS	= $(ALL_CPPFLAGS) $(LOCAL_CFLAGS) -O2 # -g
LIBPNG_FLAGS	= $(LIBPNG_CPPFLAGS) $(LIBPNG_CFLAGS) 

# Linker flags. Order of -l matters.
LDFLAGS			= -L$(LIBJPEG_DIR) -L$(LIBZ_DIR) -L$(LIBPNG_DIR) -L$(GIFENCODER_DIR) -lpng -ljpeg -lz -lgifencoder 

all: lib build run

build: $(SRC_FILES)
	$(CXX) -o $(TARGET) $(SRC_FILES) $(CPPFLAGS) $(LDFLAGS)

lib: jpeg z png gifencoder

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -f src/*.o
	rm -f lib/*.o
	rm -f bin/*.o

# --- Library builds ---
# libjpeg.a
# Modification:
#	jconfig.h		-> added typedef unsigned char boolean
#	jmoreconfig.h	-> changed into 'typedef signed int INT32' due to conflict with windows type definition
jpeg:
	@for src in $(LIBJPEG_SRC); do \
        $(CC) -c $$src $(LIBJPEG_CPPFLAGS) -o $${src%.c}.o; \
    done;
	@$(AR) $(LIBJPEG) $(LIBJPEG_SRC:.c=.o)
	@$(RANLIB) $(LIBJPEG)
	@echo "libjpeg built successfully"

# libz.a
# Ignore test programs (excludes minigzip for example)
# There is a warning when compiling zlib on Windows. Ignore it with -w
z:
	@for src in $(LIBZ_SRC); do \
		$(CC) -c $$src $(LIBZ_CPPFLAGS) -w -o $${src%.c}.o; \
	done;
	@$(AR) $(LIBZ) $(LIBZ_SRC:.c=.o)
	@$(RANLIB) $(LIBZ)
	@echo "zlib built successfully"

# libpng.a
# libz.a is not included here so the final program must be linked against it
# Ignore test programs (excludes pngtest, pngvalid, pngcp, pngcrush, pngfixup)
png:
	@for src in $(LIBPNG_SRC); do \
		$(CC) -c $$src $(LIBPNG_FLAGS)  -o $${src%.c}.o; \
	done;
	@$(AR) $(LIBPNG) $(LIBPNG_SRC:.c=.o)
	@$(RANLIB) $(LIBPNG)
	@echo "libpng built successfully"

# libgifencoder.a
# Includes a distribution of giflib
gifencoder:
	@for src in $(GIFENCODER_SRC); do \
		$(CXX) -c $$src $(CPPFLAGS) -o $${src%.cpp}.o; \
	done;
	@for src in $(GIFLIB_SRC); do \
		$(CXX) -c $$src $(CPPFLAGS) -o $${src%.cpp}.o; \
	done;
	@$(AR) $(GIFENCODER) $(GIFENCODER_SRC:.cpp=.o) $(GIFLIB_SRC:.cpp=.o)
	@$(RANLIB) $(GIFENCODER)
	@echo "GifEncoder built successfully"
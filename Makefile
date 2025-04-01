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

# Executable and library directory
TARGET		= $(BIN_DIR)/main
LIBJPEG		= $(LIBJPEG_DIR)/libjpeg.a
LIBZ		= $(LIBZ_DIR)/libz.a

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

# Compiler flags
CFLAGS				= -I$(SRC_DIR) -I$(LIB_DIR) -I$(LIBJPEG_DIR)
LIBJPEG_CFLAGS		= -I$(LIBJPEG_DIR)
LIBZ_CFLAGS			= -I$(LIBZ_DIR) -O3 -D_LARGEFILE64_SOURCE=1 -DHAVE_HIDDEN -Wall

# Linker flags
LDFLAGS			= -L$(LIBJPEG_DIR) -ljpeg

build: jpeg $(SRC_FILES)
	$(CXX) -o $(TARGET) $(SRC_FILES) $(CFLAGS) $(LDFLAGS)

run:
	./$(TARGET)

all: build run

clean:
	rm -f $(TARGET)
	rm -f src/*.o
	rm -f lib/*.o
	rm -f bin/*.o

# Library builds
# libjpeg.a
# Modification:
#	jconfig.h		-> added typedef unsigned char boolean
#	jmoreconfig.h	-> changed into 'typedef signed int INT32' due to conflict with windows type definition
jpeg:
	for src in $(LIBJPEG_SRC); do \
        $(CC) -c $$src $(LIBJPEG_CFLAGS) -o $${src%.c}.o; \
    done; \
	$(AR) $(LIBJPEG) $(LIBJPEG_SRC:.c=.o)
	$(RANLIB) $(LIBJPEG)

# libz.a
# Ignore test programs (excludes minigzip, example)
z:
	for src in $(LIBZ_SRC); do \
		$(CC) -c $$src -o $${src%.c}.o; \
	done; \
	$(AR) $(LIBZ) $(LIBZ_SRC:.c=.o)
	$(RANLIB) $(LIBZ)
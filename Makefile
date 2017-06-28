# Path for core gat headers
INCLUDES = -Iinclude/gat

# Path for core gat source
GAT_SRC_PATH = src/gat

# Path for masm85 frotnend source
SRC_PATH = src/masm85

# Target output path
TARGET_PATH = bin

# C compiler
CC = cc

# C compiler flags
CFLAGS = -Wall

# Gat objects
GAT_OBJS = \
    gat_conv.o \
    gat_core.o \
    gat_io.o \
    gat_lexer.o \
    gat_parser.o \
    gat_str.o \
    gat_sysutils.o \
    gat_table.o \
    gat_tokenizer.o

# masm85 objects
MASM85_OBJS = \
    masm85_arch.o \
    masm85_callbacks.o \
    masm85_cmdline.o \
    masm85_emit_bin.o \
    masm85_emit_dbg.o \
    masm85_emit_hex.o \
    masm85_filter.o \
    masm85_main.o \
    masm85_table.o
    
# Targets

default: masm85

masm85: $(GAT_OBJS) $(MASM85_OBJS)
	$(CC) -o $(TARGET_PATH)/masm85 $(GAT_OBJS) $(MASM85_OBJS)
	
clean_objs:
	rm -f *.o
 
clean: clean_objs
	rm -f $(TARGET_PATH)/*

gat_conv.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(GAT_SRC_PATH)/gat_conv.c
gat_core.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(GAT_SRC_PATH)/gat_core.c
gat_io.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(GAT_SRC_PATH)/gat_io.c
gat_lexer.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(GAT_SRC_PATH)/gat_lexer.c
gat_parser.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(GAT_SRC_PATH)/gat_parser.c
gat_str.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(GAT_SRC_PATH)/gat_str.c
gat_sysutils.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(GAT_SRC_PATH)/gat_sysutils.c
gat_table.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(GAT_SRC_PATH)/gat_table.c
gat_tokenizer.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(GAT_SRC_PATH)/gat_tokenizer.c
    
masm85_arch.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(SRC_PATH)/masm85_arch.c
masm85_callbacks.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(SRC_PATH)/masm85_callbacks.c
masm85_cmdline.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(SRC_PATH)/masm85_cmdline.c
masm85_emit_bin.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(SRC_PATH)/masm85_emit_bin.c
masm85_emit_dbg.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(SRC_PATH)/masm85_emit_dbg.c
masm85_emit_hex.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(SRC_PATH)/masm85_emit_hex.c
masm85_filter.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(SRC_PATH)/masm85_filter.c
masm85_main.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(SRC_PATH)/masm85_main.c
masm85_table.o:
	$(CC) -c $(CFLAGS) $(INCLUDES) $(SRC_PATH)/masm85_table.c

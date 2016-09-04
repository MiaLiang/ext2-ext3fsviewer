LINKER = gcc -o
COMPILER = gcc -c

CMP_FLAGS = -g
LINK_FLAGS = 
TARGET = check_extfs

RM = rm -f

OBJS = \
fs/BlockIO.o \
fs/dir.o \
fs/file.o \
cmd/cmd.o \
cmd/operate.o \
main.o \



$(TARGET):$(OBJS)
	$(LINKER) $(TARGET) $(OBJS) $(CMP_FLAGS) $(LINK_FLAGS)
	
$(OBJS):%.o:%.c
	$(COMPILER) $(CMP_FLAGS) $< -o $@
	
clean:
	$(RM) $(TARGET) $(OBJS)
	
		



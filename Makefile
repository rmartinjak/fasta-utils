CC = clang
CFLAGS += -std=c89 -pedantic
CFLAGS += -Wall -Wextra
CFLAGS += -g

_TARGETS = cat shuffle
TARGETS = $(addprefix fasta-, $(_TARGETS)) 

SRCDIR = src
OBJDIR = obj

_OBJ = fasta
OBJ = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(_OBJ)))


default : $(TARGETS) fasta-head

$(OBJDIR)/%.o : $(SRCDIR)/%.c | $(OBJDIR)
	@echo CC -c $^
	@$(CC) $(CFLAGS) -c -o $@ $^
	
fasta-head :: $(SRCDIR)/cat.c $(OBJ)
	@echo CC -o $@
	@$(CC) $(CFLAGS) -DFASTA_HEAD -o $@ $^

$(TARGETS) :: fasta-% : $(SRCDIR)/%.c $(OBJ)
	@echo CC -o $@
	@$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR) :
	mkdir $(OBJDIR)

clean :
	rm -f $(TARGETS) fasta-head
	rm -rf $(OBJDIR)

.PHONY : clean

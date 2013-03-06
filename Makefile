CC = clang
CFLAGS += -std=c89 -pedantic
CFLAGS += -Wall -Wextra
CFLAGS += -g

_TARGETS = cat head shuffle split
TARGETS = $(addprefix fasta-, $(_TARGETS)) 

SRCDIR = src
OBJDIR = obj

_OBJ = fasta main
OBJ = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(_OBJ)))


default : $(TARGETS)

$(OBJDIR)/%.o : $(SRCDIR)/%.c | $(OBJDIR)
	@echo CC -c $^
	@$(CC) $(CFLAGS) -c -o $@ $^

$(TARGETS) :: fasta-% : $(SRCDIR)/%.c $(OBJ)
	@echo CC -o $@
	@$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR) :
	mkdir $(OBJDIR)

clean :
	rm -f $(TARGETS)
	rm -rf $(OBJDIR)

.PHONY : clean

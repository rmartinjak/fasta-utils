DESTDIR ?= ~

CC = cc
CFLAGS += -std=c89 -pedantic
CFLAGS += -Wall -Wextra
#CFLAGS += -g
CFLAGS += -O2

_TARGETS = cat head shuffle split lengths comment
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
	@mkdir $(OBJDIR)


install : $(TARGETS)
	@echo installing executables to $(DESTDIR)/bin
	@install -m 755 -t $(DESTDIR)/bin $^

uninstall :
	@echo uninstalling executables from $(DESTDIR)/bin
	@rm -f $(addprefix $(DESTDIR)/bin/,$(TARGETS))


clean :
	@echo cleaning
	@rm -f $(TARGETS)
	@rm -rf $(OBJDIR)

.PHONY : clean install uninstall

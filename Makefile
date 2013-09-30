DESTDIR ?= ~

CC = cc
CFLAGS += -std=c89 -pedantic
CFLAGS += -Wall -Wextra
CFLAGS += -O2
#CFLAGS += -O0 -g


CPPFLAGS += -DHAVE_ZLIB
CPPFLAGS += -D_XOPEN_SOURCE
LIBS += -lz

_TARGETS = cat head shuffle split lengths comment grep chop
TARGETS = $(addprefix fasta-, $(_TARGETS))

SRCDIR = src
OBJDIR = obj

_OBJ = fasta main
OBJ = $(addprefix $(OBJDIR)/,$(addsuffix .o,$(_OBJ)))


default : $(TARGETS)

$(OBJDIR)/%.o : $(SRCDIR)/%.c | $(OBJDIR) $(SRCDIR)/fasta.h
	@echo CC -c $^
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^ $(LIBS)

$(TARGETS) :: fasta-% : $(SRCDIR)/%.c $(OBJ)
	@echo CC -o $@
	@$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ $(LIBS)

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

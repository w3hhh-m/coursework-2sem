CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lpng -lm

SRCDIR = src
INCDIR = include
BUILDDIR = build
DOCSDIR = docs
LATEXDIR = $(DOCSDIR)/latex
AWESOME_CSS_DIR = $(DOCSDIR)/doxygen-awesome-css
AWESOME_FILES = doxygen-awesome.css doxygen-awesome-sidebar-only.css doxygen-awesome-sidebar-only-darkmode-toggle.css doxygen-awesome-darkmode-toggle.js header.html

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SOURCES))
EXECUTABLE = cw

.PHONY: all clean docs

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

$(AWESOME_CSS_DIR)/%:
	mkdir -p $(AWESOME_CSS_DIR)
	wget -O $@ https://raw.githubusercontent.com/w3hhh-m/COURSEWORK-2SEM-ADDONS/main/$(notdir $@)

Doxyfile:
	wget -O $@ https://raw.githubusercontent.com/w3hhh-m/COURSEWORK-2SEM-ADDONS/main/Doxyfile

docs: Doxyfile $(AWESOME_FILES:%=$(AWESOME_CSS_DIR)/%)
	doxygen $<
	cd $(LATEXDIR) && \
	rm -f refman.tex && \
	wget -O refman.tex https://raw.githubusercontent.com/w3hhh-m/COURSEWORK-2SEM-ADDONS/main/refman.tex && \
	make && \
	mv refman.pdf ../docs.pdf && \
	cd .. && \
	rm -rf latex

clean:
	rm -f $(EXECUTABLE)
	rm -rf $(BUILDDIR)
	rm -rf $(DOCSDIR)
	rm -f Doxyfile

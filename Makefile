CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lpng -lm

SRCDIR = src
INCDIR = include
BUILDDIR = build
DOCSDIR = docs
LATEXDIR = $(DOCSDIR)/latex
HTMLDIR = $(DOCSDIR)/html

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

addons:
	mkdir -p $(DOCSDIR)/doxygen-awesome-css
	git clone https://github.com/w3hhh-m/COURSEWORK-2SEM-ADDONS $(DOCSDIR)/doxygen-awesome-css
	mv $(DOCSDIR)/doxygen-awesome-css/Doxyfile .

docs: addons
	doxygen Doxyfile
	cd $(LATEXDIR) && \
	rm -f refman.tex && \
	cp ../doxygen-awesome-css/refman.tex . && \
	make && \
	mv refman.pdf ../docs.pdf && \
	cd .. && \
	rm -rf latex

clean:
	rm -f $(EXECUTABLE)
	rm -rf $(BUILDDIR)
	rm -rf $(DOCSDIR)
	rm -f Doxyfile

MF=	Makefile

LC=	pdflatex

LFLAGS=

PDF=	abstract.pdf

SRC= \
	abstract.tex
	

.SUFFIXES:
.SUFFIXES: .tex .pdf

TOC=	abstract.toc

IMG=	

TMP=	$(SRC:.tex=.tex~) core abstract.aux abstract.log

all:    $(PDF)

$(PDF):	$(TOC)
	$(LC) $(LFLAGS) $(SRC) $@ 

$(TOC):	$(MF) $(IMG)

clean:
	rm -f $(PDF) $(TOC) $(TMP)

thesis.toc  : $(SRC)
	$(LC) $(LFLAGS) $(SRC)

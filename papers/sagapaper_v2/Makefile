
NAME = sagapaper

again: all

all:
	latex    $(NAME)
	bibtex   $(NAME)
	latex    $(NAME)
	latex    $(NAME)

clean:
	rm -f *.aux
	rm -f *.bbl
	rm -f *.blg
	rm -f *.log
	rm -f *.out

distclean: clean
	rm -f $(NAME).pdf

xpdf: all
	xpdf -z page $(NAME).pdf &

vi:
	gvim -o Makefile *.tex *.bib &


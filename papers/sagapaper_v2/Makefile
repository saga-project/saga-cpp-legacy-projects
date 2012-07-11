
NAME = sagapaper
XDVI = xdvi -s 4 -geometry 812x1068+0+0

again: all

all:
	latex    $(NAME)
	bibtex   $(NAME)
	latex    $(NAME)
	latex    $(NAME)
	pdflatex $(NAME)
	test -e .xdvi_lock || ( touch .xdvi_lock; $(XDVI) $(NAME).dvi; rm -f .xdvi_lock ) &

clean:
	rm -f *.aux
	rm -f *.bbl
	rm -f *.blg
	rm -f *.log
	rm -f *.out

distclean: clean
	rm -f $(NAME).pdf
	rm -f .xdvi_lock

xpdf: all
	xpdf -z page $(NAME).pdf &

vi:
	gvim -o Makefile *.tex *.bib &


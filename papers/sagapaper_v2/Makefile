
BASE   = sagapaper
ENV    = env XEDITOR=$(HOME)/bin/xdvi_edit DVIFILE=$(BASE).dvi DVINAME=XDVI-$(BASE)
XDVI   = $(ENV) xdvi -s 4 -geometry 812x1068+0+0
EDITOR = $(ENV) gvim      -geometry 119x58-0+0 --servername XDVI-$(BASE)

all: show

again: 
	 $(MAKE) -W $(BASE).tex $(filter-out again, $(MAKECMDGOALS))

$(BASE).dvi : $(BASE).tex
	latex    $(BASE)
	bibtex   $(BASE)

$(BASE).pdf : $(BASE).tex
	pdflatex $(BASE)
	bibtex   $(BASE)

show: xdvi
	
xdvi: $(BASE).dvi
	test -e .xdvi_lock || ( touch .xdvi_lock; $(XDVI) $(BASE).dvi; rm -f .xdvi_lock ) &
	-@gvim --servername XDVI-$(BASE) --remote-send "<ESC>:call XDVI_jump()<CR>i"

xpdf: $(BASE).pdf
	xpdf -z page $(BASE).pdf &

clean:
	rm -f *.aux
	rm -f *.bbl
	rm -f *.blg
	rm -f *.log
	rm -f *.out
	rm -f *.toc
	rm -f .xdvi_lock

distclean: clean
	rm -f $(BASE).pdf
	rm -f $(BASE).dvi
	rm -f $(BASE).ps
	rm -f .xdvi_lock

edit:
	$(EDITOR) *.tex *.bib Makefile &


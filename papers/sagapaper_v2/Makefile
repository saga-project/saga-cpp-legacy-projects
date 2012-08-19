
BASE   = sagapaper
ENV    = env XEDITOR=$(HOME)/bin/xdvi_edit DVIFILE=$(BASE).dvi DVINAME=XDVI-$(BASE)
XDVI   = $(ENV) xdvi -s 4 -geometry 812x1068+0+0
EDITOR = $(ENV) gvim      -geometry 119x58-0+0 --servername XDVI-$(BASE)



again: show

all:
	latex    $(BASE)
	bibtex   $(BASE)
	latex    $(BASE)
	latex    $(BASE)
	pdflatex $(BASE)
	-@gvim --servername XDVI-$(BASE) --remote-send "<ESC>:call XDVI_jump()<CR>i"

show: all
	test -e .xdvi_lock || ( touch .xdvi_lock; $(XDVI) $(BASE).dvi; rm -f .xdvi_lock ) &

clean:
	rm -f *.aux
	rm -f *.bbl
	rm -f *.blg
	rm -f *.log
	rm -f *.out
	rm -f .xdvi_lock

distclean: clean
	rm -f $(BASE).pdf
	rm -f .xdvi_lock

xpdf: all
	xpdf -z page $(BASE).pdf &

edit:
	$(EDITOR) *.tex *.bib Makefile &


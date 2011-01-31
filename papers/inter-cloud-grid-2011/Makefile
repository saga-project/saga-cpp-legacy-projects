
TEX=pdflatex
BIBTEX=bibtex
VIEW=evince

SOURCE:=$(shell egrep -l '^[^%]*\\begin\{document\}' *.tex)
PDFS=$(SOURCE:.tex=.pdf)

all:
	for item in $(PDFS) ; do \
	echo $$item ; \
	make $$item ; \
	done

%.pdf: %.tex
	echo $*
	$(TEX) $*
	
	@if(grep "There were undefined references" $*.log > /dev/null);\
	then \
		$(BIBTEX) $*; \
		$(TEX) $*; \
	fi
	
	@if(grep "Rerun" $*.log > /dev/null);\
	then \
		$(TEX) $*;\
	fi

show: 
	$(VIEW) $(PDFS)

show-mac: all
	open $(PDFS)

clean:
	@rm -f *.pdf *.aux *.bbl *.blg *.log *.dvi *.png \
		*.idx *.ilg *.ind *.toc *.lot *.lof *.out

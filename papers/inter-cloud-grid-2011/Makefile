
again: pdf

pdf:
#	@cd figures && ./make_figures.sh
	@pdflatex inter-cloud-grid-2011

bib: pdf
	@bibtex   inter-cloud-grid-2011
	@pdflatex inter-cloud-grid-2011 | grep undef || true

clean:
	@rm -f makelog
	@rm -f inter-cloud-grid-2011.aux
	@rm -f inter-cloud-grid-2011.bbl
	@rm -f inter-cloud-grid-2011.blg
	@rm -f inter-cloud-grid-2011.dvi
	@rm -f inter-cloud-grid-2011.log
	@rm -f inter-cloud-grid-2011.out
	@rm -f inter-cloud-grid-2011.pdfsync

distclean: clean
	@rm -f inter-cloud-grid-2011.pdf



again: pdf

pdf:
#	@cd figures && ./make_figures.sh
	@pdflatex egi-white-paper
	@bibtex   egi-white-paper
	@pdflatex egi-white-paper | grep undef || true

clean:
	@rm -f makelog
	@rm -f egi-white-paper.aux
	@rm -f egi-white-paper.bbl
	@rm -f egi-white-paper.blg
	@rm -f egi-white-paper.dvi
	@rm -f egi-white-paper.log
	@rm -f egi-white-paper.out
	@rm -f egi-white-paper.pdfsync

distclean: clean
	@rm -f egi-white-paper.pdf


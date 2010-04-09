
again: pdf

pdf:
	@cd figures && ./make_figures.sh
	@pdflatex 2010_fgcs_mapreduce
	@bibtex   2010_fgcs_mapreduce
	@pdflatex 2010_fgcs_mapreduce | grep undef

clean:
	@rm -f 2010_fgcs_mapreduce.aux
	@rm -f 2010_fgcs_mapreduce.bbl
	@rm -f 2010_fgcs_mapreduce.blg
	@rm -f 2010_fgcs_mapreduce.dvi
	@rm -f 2010_fgcs_mapreduce.log
	@rm -f 2010_fgcs_mapreduce.pdfsync

distclean:
	@rm -f 2010_fgcs_mapreduce.pdf


INPUTFILE = saga_tg08
LATEX = pdflatex
BIBTEX = bibtex
DVIPS = dvips
USPDF = dvipdfm -p letter
PDF = dvipdfm  -e -p letter 
RM = rm -f

TEXFILES = $(INPUTFILE).tex 

SECTIONS = $(TEXFILES:.tex=)

BIBFILES =  saga_tg08.bib

all: $(INPUTFILE).dvi $(TEXFILES) saga_tg08.bbl 
	$(LATEX) $(TEXFILES)
	$(LATEX) $(TEXFILES)

pdf:
	dvips -Pdownload35 -G0  $(INPUTFILE).dvi -o $(INPUTFILE).ps
	ps2pdf -dEmbedAllFonts=true $(INPUTFILE).ps $(INPUTFILE).pdf

altpdf: 
	$(PDF) $(INPUTFILE).dvi

uspdf: 
	$(USPDF) $(INPUTFILE).dvi

ps: $(INPUTFILE).ps $(PATHTEXFILES).ps

$(INPUTFILE).pdf: $(TEXFILES) saga_tg08.bbl
	$(PDF) $(INPUTFILE)	

$(INPUTFILE).ps: $(INPUTFILE).dvi
	$(DVIPS) -o $@ $(INPUTFILE).dvi

$(INPUTFILE).dvi: $(TEXFILES)
	$(LATEX) $(INPUTFILE)

$(INPUTFILE).aux: $(TEXFILES)
	$(LATEX) $(INPUTFILE)

saga_tg08.bbl: $(BIBFILES) $(INPUTFILE).aux
	$(BIBTEX) $(INPUTFILE)

clean: 
	$(RM) *.dvi *.log *.aux *.bbl *.ps *.pdf 

zip: 
	zip saga_gin.ziip  saga_tg08.tex saga_tg08.bib Makefile IEEEtran.bst IEEEtran.cls *.eps


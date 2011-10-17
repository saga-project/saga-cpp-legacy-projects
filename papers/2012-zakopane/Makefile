
#####################################################################
# project name

BASE        = 2012-zakopane

#####################################################################
# executeables

TEX         = latex
PDFTEX      = pdflatex
SPELL       = ispell # -d deutsch
DVIPS       = dvips
BIBTEX      = bibtex
FIG2DEV     = fig2dev
XDVI        = xdvi 
XDVI_LOCK   = .xdvi_lock
ACRO        = xpdf
ACRO_LOCK   = .acro_lock
RM          = rm -f
TOUCH       = touch
LPR         = lpr
GVIM        = vim -g
EDITOR      = $(GVIM) -o --servername XDVI-$(BASE)
SHELL       = /bin/sh


#####################################################################
# files

TEXDIR      = .
BIBDIR      = .
PICDIR      = .
FIGDIR      = .

TEXFILES    = $(wildcard $(TEXDIR)/*.tex)
BIBFILES    = $(wildcard $(BIBDIR)/*.bib)
AUXS        = $(wildcard *.aux)
PICS        = $(wildcard $(PICDIR)/*.{eps,ps,jpeg,jpg,gif,png,pdf})
FIGS        = $(wildcard $(PICDIR)/*.fig)

TEX_FIGS    = $(addsuffix .eps,$(basename $(FIGS)))
PDF_FIGS    = $(addsuffix .pdf,$(basename $(FIGS)))

#####################################################################
# special targets

.SECONDARY:   $(TEX_FIGS) $(PDF_FIGS)

.SUFFIXES:    .tex .dvi .eps .ps  .pdf .bib .bbl .fig 
              
.PHONY:       clean distclean figclean \
              show   s                 \
              again  a                 \
              spell                    \
              edit   e                 \
              bib    b                 \
              ps pdf dvi
              
#####################################################################
# implicit rules
              
%.dvi:        %.tex $(TEXFILES) $(PICS) $(TEX_FIGS)
	            $(TEX) $<
              
%.pdf:        %.tex $(TEXFILES) $(PICS) $(PDF_FIGS)
	            $(PDFTEX) $<
              
%.ps:         %.dvi
	            $(DVIPS) $< -o $@
              
%.bbl:        %.bib
	            $(if $(wildcard $(BASE).aux), $(BIBTEX) $*)
              
%.eps:        %.fig
	            $(FIG2DEV) -Leps $< $@
              
%.ps:         %.fig
	            $(FIG2DEV) -Lps $< $@
              
%.pdf:        %.fig
	            $(FIG2DEV) -Lpdf $< $@
              

#####################################################################
# short targets

s:            show
d:            slides
b:            bib
a:            again
e:            edit

ps:           $(BASE).ps
pdf:          $(BASE).pdf
dvi:          $(BASE).dvi
tex:          $(BASE).tex
bbl:          $(BASE).bbl
aux:          $(BASE).aux
              
#####################################################################
# targets

spell:        
	            @$(SPELL) $(TEXFILES)
              
edit:         
	            @$(EDITOR) $(TEXFILES) $(BIBFILES) &
              
bib: 
	            @$(BIBTEX) $(BASE)

#--------------------------------------------------
# if an xdvi starts, it creates an XDVI_LOCK file.
# if this exists, it does not start a new process.
# if xdvi finishes, it removes the XDVI_LOCK file.
# 
show:         $(BASE).dvi
	            @if [ ! -r $(XDVI_LOCK) ]; then \
	              ($(TOUCH) $(XDVI_LOCK) ;      \
	               $(XDVI)  $(BASE).dvi  ;      \
	               $(RM)    $(XDVI_LOCK) ;      \
	              )&                            \
	            fi
              
again:
	            $(MAKE) -W $(BASE).tex $(filter-out again, $(MAKECMDGOALS))
	            
clean:        
	            @$(MAKE) figclean
	            @$(RM) $(BASE).m*
	            @$(RM) *.{aux,blg,bbl,log,toc,err,idx,bak,ilg,ind,lof,lot,out}
	            @$(RM) *.{bmt,loe}
	            @$(RM) .*.sw[po] *~ makelog
              
figclean:     
	            @$(RM) $(TEX_FIGS) $(PDF_FIGS)
              
distclean:    clean figclean
	            @$(RM) $(BASE).{bak,dvi,ps,pdf,tex.bak}
	            @$(RM) $(XDVI_LOCK) $(ACRO_LOCK)
              

#####################################################################
# dependencies

$(BASE).aux:  $(BASE).dvi
$(BASE).log:  $(BASE).dvi
$(BASE).toc:  $(BASE).dvi
              
test:
	@echo PDF_PICS: $(PDF_FIGS)


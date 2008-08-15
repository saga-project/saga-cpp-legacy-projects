
#####################################################################
# project name

BASE        = cca08

#####################################################################
# executeables

TEX         = latex
PDFTEX      = pdflatex
SPELL       = ispell # -d deutsch
PSBOOK      = psbook
DVIPS       = dvips
BIBTEX      = bibtex
MAKEINDEX   = makeindex
MAKE_IND    = $(MAKEINDEX) -s gind.ist 
PS2SORT     = ps2sort
FIG2DEV     = fig2dev
PS2EPSI     = ps2epsi
EPSTOPDF    = epstopdf
XDVI        = xdvi 
XDVI_LOCK   = .xdvi_lock
ACRO        = xpdf
ACRO_LOCK   = .acro_lock
RM          = rm -f
TOUCH       = touch
LPR         = lpr
GVIM        = /usr/bin/vim -g
EDITOR      = $(GVIM) -o --servername XDVI-$(BASE)


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

.SUFFIXES:    .tex .dvi .eps .ps  .pdf \
              .bib .bbl .fig .idx .ind
              
.PHONY:       clean distclean figclean \
              show   s                 \
              again  a                 \
              print  p                 \
              print2 p2                \
              spell                    \
              touch  t                 \
              edit   e                 \
              Edit   E                 \
              fine                     \
              bib bbl ind              \
              ps pdf dvi
              
#####################################################################
# implicit rules
              
%.dvi:        %.tex
%.dvi:        %.tex $(TEXFILES) $(PICS) $(TEX_FIGS)
	            $(TEX) $<
              
%.pdf:        %.tex
%.pdf:        %.tex $(TEXFILES) $(PICS) $(PDF_FIGS)
	            $(PDFTEX) $<
              
%.ps:         %.dvi
%.ps:         %.dvi
	            $(DVIPS) $< -o $@
              
%.bbl:        %.bib
%.bbl:        %.bib
	            $(if $(wildcard $(BASE).aux), $(BIBTEX) $* )
              
%.ind:        %.idx
%.ind:        %.idx
	            $(MAKE_IND) -o $@ $<
              
%.eps:        %.fig
%.eps:        %.fig
	            $(FIG2DEV) -Leps $< $@
              
%.ps:         %.fig
%.ps:         %.fig
	            $(FIG2DEV) -Lps $< $@
              
# %.pdf:        %.fig
# %.pdf:        %.fig
# 	            $(FIG2DEV) -Lpdf $< $@
              
%.eps:        %.ps
%.eps:        %.ps
	            $(PS2EPSI) $< $@
              
%.pdf:        %.eps
%.pdf:        %.eps
	            $(EPSTOPDF) $<

#####################################################################
# short targets

s:            show
d:            slides
b:            bib
a:            again
t:            touch
e:            edit
E:            Edit
p:            print
p2:           print2

ps:           $(BASE).ps
pdf:          $(BASE).pdf
dvi:          $(BASE).dvi
tex:          $(BASE).tex
ind:          $(BASE).ind
bbl:          $(BASE).bbl
aux:          $(BASE).aux
              
#####################################################################
# targets

spell:        
	            @$(SPELL) $(TEXFILES)
              
print:        $(BASE).ps
	            @$(LPR) $(BASE).ps
              
touch:        
	            @$(TOUCH) $(TEXFILES) $(BIBFILES)
              
edit:         
	            @$(EDITOR) $(TEXFILES) $(BIBFILES) &
              
Edit:         
	            @$(EDITOR) $(TEXFILES) $(BIBFILES) \
	            $(wildcard GNUmakefile* makefile* Makefile*) &
              
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
              
slides:       $(BASE).pdf
	            @if [ ! -r $(ACRO_LOCK) ]; then \
	              ($(TOUCH) $(ACRO_LOCK) &&     \
	               $(ACRO)  $(BASE).pdf  &&     \
	               $(RM)    $(ACRO_LOCK) ;      \
	              )&                            \
	            fi
              
again:
	            $(MAKE) -W $(BASE).tex $(filter-out again, $(MAKECMDGOALS))
	            
fine:         
	            @$(MAKE) distclean
	            @$(MAKE) dvi again
	            @$(MAKE) ind
	            @$(MAKE) bib
	            @$(MAKE) again dvi
	            @$(MAKE) again dvi
	            -@$(MAKE) again pdf
	            @$(MAKE) again pdf
	            @$(MAKE) ps
              
clean:        
	            @$(MAKE) figclean
	            @$(RM) $(BASE).m* \
	              *.{aux,blg,bbl,log,toc,err,idx,bak,ilg,ind,lof,lot,out} \
	              *.{bmt,loe} \
                .*.sw[po] *~ makelog
              
distclean:    
	            @$(MAKE) clean
	            @$(RM) $(BASE).{bak,dvi,ps,pdf,tex.bak}
	            @$(RM) $(XDVI_LOCK) $(ACRO_LOCK)
              
figclean:     
	            @$(RM) $(TEX_FIGS) $(PDF_FIGS)
              
bib: 
	            -@for file in $(AUXS) ; do $(BIBTEX) $$file ; done

#####################################################################
# dependencies

$(BASE).aux:  $(BASE).dvi
$(BASE).log:  $(BASE).dvi
$(BASE).toc:  $(BASE).dvi
$(BASE).idx:  $(BASE).dvi
              
test:
	@echo PDF_PICS: $(PDF_FIGS)

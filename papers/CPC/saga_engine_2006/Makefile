
#####################################################################
# project name

BASE        = saga_engine

#####################################################################
# executeables

TEX         = latex
COL         = | cat | awk -f col_latex.awk
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
XDVI_EDITOR = "xdvi_edit XDVI-$(BASE) %f %l"
XDVI_OPTS   = +statusline -hushstdout -nopostscript -geometry 830x1175-0+0
XDVI_OPTS   = +statusline -hushstdout -geometry 830x1175-0+0
XDVI        = xdvi -editor $(XDVI_EDITOR) $(XDVI_OPTS)
XDVI_LOCK   = .xdvi_lock
ACRO        = xpdf
ACRO_LOCK   = .acro_lock
RM          = rm -f
TOUCH       = touch
LPR         = lpr
ENV         = /bin/env DVIFILE=$(BASE).dvi DVINAME=XDVI-$(BASE)
EDITOR      = gvim -o -geometry 82x55+92+0 --servername XDVI-$(BASE)

#####################################################################
# files

TEXDIR      = .
BIBDIR      = .
PICDIR      = images
FIGDIR      = images

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
              
# %.dvi:        %.tex
%.dvi:        %.tex $(TEXFILES) $(PICS) $(TEX_FIGS)
	            $(TEX) $< $(COL)
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
	            $(FIG2DEV) -Leps -l dummy $< $@
              
%.ps:         %.fig
%.ps:         %.fig
	            $(FIG2DEV) -Lps -l dummy $< $@
              
%.png:        %.fig
%.png:        %.fig
	            $(FIG2DEV) -Lpng -l dummy $< $@
              
# %.pdf:        %.fig
# %.pdf:        %.fig
# 	            $(FIG2DEV) -Lpdf -l dummy $< $@
              
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
	            @$(ENV) $(EDITOR) `cat FILES` $(BIBFILES) &
	            @echo "name: $(EDITOR)"
              
Edit:         
	            @$(ENV) $(EDITOR) $(TEXFILES) $(BIBFILES) \
	            $(wildcard GNUmakefile* makefile* Makefile*) &
              
#--------------------------------------------------
# if an xdvi starts, it creates an XDVI_LOCK file.
# if this exists, it does not start a new process.
# if xdvi finishes, it removes the XDVI_LOCK file.
# 
show:         $(BASE).dvi
	            @if [ ! -r $(XDVI_LOCK) ]; then   \
	              ($(TOUCH) $(XDVI_LOCK) ;        \
	               $(ENV)   $(XDVI) $(BASE).dvi ; \
	               $(RM)    $(XDVI_LOCK) ;        \
	              )&                              \
	            fi
	            -@gvim --servername XDVI-$(BASE) --remote-send "<ESC>:call XDVI_jump()<CR>i"
              
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
	              *.{bmt,loe,glo} \
                .*.sw[po] *~
              
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

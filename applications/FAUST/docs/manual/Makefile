BASE        = .
RM	    = rm -f
MAKE	    = make
XDVI_LOCK   = .xdvi_lock
ACRO_LOCK   = .acro_lock

clean: 
	@$(RM) $(BASE).m* \
        *.{aux,blg,bbl,log,toc,err,idx,bak,ilg,ind,lof,lot,out,lod} \
        *.{bmt,loe,glo} .*.sw[po] *~

distclean:
	@$(MAKE) clean
	@$(RM) $(BASE).{bak,dvi,ps,pdf,tex.bak}
	@$(RM) $(XDVI_LOCK) $(ACRO_LOCK) makelog
	@$(RM) *.tex.final


all:
	(cd cfitsio; ./configure; make; cp libcfitsio.a ../..; cp *.h ../../include)
	(cd cmd; make; make install)
	(cd coord; make; make install)
	(cd mtbl; make; make install)
	(cd joblist; make; make install)
	(cd svc; make; make install)
	(cd boundaries; make; make install)
	(cd pixbounds; make; make install)
	(cd wcstools/libwcs; make; make install)
	(cd two_plane; make; make install)
	(cd jpeg; ./configure; make; cp libjpeg.a ../..; cp jpeglib.h jconfig.h jmorecfg.h ../../include)
	(cd ..; ranlib *.a)
	chmod 644 ../include/*

clean:
	rm -f ../include/* ../*.a
	(cd cfitsio; make clean; rm -f config.log config.cache config.status)
	(cd cmd; make clean)
	(cd coord; make clean)
	(cd mtbl; make clean)
	(cd joblist; make clean)
	(cd svc; make clean)
	(cd boundaries; make clean)
	(cd pixbounds; make clean)
	(cd wcstools/libwcs; make clean)
	(cd jpeg; rm -f *.a *.o cjpeg djpeg jpegtran rdjpgcom wrjpgcom config.log config.status )

	(cd two_plane; make clean)

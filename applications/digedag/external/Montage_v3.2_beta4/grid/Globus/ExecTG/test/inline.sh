#!/bin/sh

time ../mExec -d 1 \
-h "\
SIMPLE  =                    T  \n\
BITPIX  =                  -64  \n\
NAXIS   =                    2  \n\
NAXIS1  =                  720  \n\
NAXIS2  =                  720  \n\
CTYPE1  = 'RA---TAN'            \n\
CTYPE2  = 'DEC--TAN'            \n\
CRVAL1  =           274.700730  \n\
CRVAL2  =           -13.807230  \n\
CRPIX1  =           360.500000  \n\
CRPIX2  =           360.500000  \n\
CDELT1  =      -0.000277777778  \n\
CDELT2  =       0.000277777778  \n\
CROTA2  =                0.000  \n\
EQUINOX =              2000.00  \n\
END" \
2mass j \
INLINE

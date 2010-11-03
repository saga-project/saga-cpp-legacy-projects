
SAGA_CXXFLAGS += -Ipngwriter
SAGA_LDFLAGS  += -Lpngwriter -lpngwriter -lpng

SAGA_CXXFLAGS += `freetype-config --cflags` 
SAGA_LDFLAGS  += `freetype-config --libs`


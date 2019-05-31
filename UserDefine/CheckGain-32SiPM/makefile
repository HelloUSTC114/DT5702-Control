LIB=FitSiPM
FULLLIB=lib${LIB}.so

SRC=FitSiPMSPE.cpp
HEADER=FitSiPMSPE.h


FULLLIB: ${SRC} ${HEADER} Link.cxx
	`root-config --cxx --cflags` -fPIC -shared -o ${FULLLIB} ${SRC} `root-config --libs`
Link.cxx: ${HEADER}
	rootcling -f Link.cxx ${HEADER}

clean:
	-@rm -rf *.so *.pcm
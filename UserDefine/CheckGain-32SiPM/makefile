LIB=FitSiPM
FULLLIB=lib${LIB}.so

SRC=FitSiPMSPE.cpp
HEADER=FitSiPMSPE.h


${FULLLIB}: ${SRC} ${HEADER} Link.cxx
	`root-config --cxx --cflags` -fPIC -shared -o ${FULLLIB} ${SRC} `root-config --libs` -lSpectrum
Link.cxx: ${HEADER} Fit_LinkDef.h
	rootcling -f Link.cxx -c -p ${HEADER} Fit_LinkDef.h

clean:
	-@rm -rf *.so *.pcm Link.cxx

test:	ReadSpectra.cpp ${FULLLIB}
	`root-config --cxx --cflags` -fPIC -o $@ $< -L. -l${LIB} `root-config --libs`
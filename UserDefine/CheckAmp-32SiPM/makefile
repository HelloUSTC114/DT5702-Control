all: CalcAmp Fit

CalcAmp: CalcAmp.cpp
	-`root-config --cxx --cflags` -fPIC -o $@ $< -L. -lFitSiPM `root-config --libs`
Fit: Fit.cpp
	-`root-config --cxx --cflags` -fPIC -o $@ $< -L. -lFitSiPM `root-config --libs`

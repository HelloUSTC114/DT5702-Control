all: CalcBias Fit
CalcBias: CalcBias.cpp
	-`root-config --cxx --cflags` -fPIC -o $@ $< -L. -lFitSiPM `root-config --libs`
Fit: Fit.cpp
	-`root-config --cxx --cflags` -fPIC -o $@ $< -L. -lFitSiPM `root-config --libs`

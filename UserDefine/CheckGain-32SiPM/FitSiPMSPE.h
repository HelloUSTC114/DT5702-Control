#ifndef FITSIPMSPE_H
#define FITSIPMSPE_H
#include <iostream>
#include <string>
#include "TF1.h"
#include "TH1.h"
#include "TFile.h"
#include "TSystem.h"

class MultiGauss
{
public:
    MultiGauss(int nPeak);
    void SetNPeak(int nPeak);
    int GetNPeak();
    double operator()(double *x, double *par);
private:
    int fNPeak;
};

#endif
#ifndef FITSIPMSPE_H
#define FITSIPMSPE_H
#include <iostream>
#include <string>
#include "TF1.h"
#include "TH1.h"
#include "TFile.h"
#include "TSystem.h"
#include "TMath.h"
#include "TSpectrum.h"

#include <map>

class MultiGauss
{
public:
    MultiGauss(int nPeak);
    void SetNPeak(int nPeak);
    int GetNPeak() const;
    double operator()(double *x, double *par) const;
private:
    int fNPeak;
};

class FitSpectrum
{
public:
    FitSpectrum(UInt_t bins = 4100);
    ~FitSpectrum();
    bool Fit(TH1* h, UInt_t nGauss = 4);
    TH1* GetOrigin() const {return fHOrigin;}
    TH1* GetBackground() const{return fHBackGround;}
    TH1* GetGauss() const{return fHGauss;}
    TH1* GetSum() const {return fHAdd;}
    TF1* GetFunction() const{return fFunction;}
    double GetGain() const;


private:
    bool fHistFlag = 0;
    bool fFitFlag = 0;
    TH1 *fHOrigin = NULL;
    TF1 *fFunction = NULL;
    TSpectrum * fSpectrum = NULL;
    TH1 *fHBackGround = NULL;
    TH1 *fHGauss = NULL;
    TH1 *fHAdd = NULL;

    double fFirstPeakMeanStartLimit = 350;
    double fFirstPeakMeanEndLimit = 500;
    double fFirstPeakStartFitPoint = 350;
    double fFirstPeakSigma = 80;

    double fGainGuess = 300;

    double fBinsX = 4100;

    bool Clear();
    bool SetHist(TH1* h);
    bool FitHist(UInt_t nGauss);

    double EstimateGain();
};

#endif
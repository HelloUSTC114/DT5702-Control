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
#include "TCanvas.h"
#include <string>
using namespace std;

#include <map>

struct FitResult
{
    double gain;
    double peak0;
};

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
    FitResult GetGain() const;

    void Save(string filename = "");

    double GetGainGuess() const{return fGainGuess;}

private:
    bool fHistFlag = 0;
    bool fFitFlag = 0;
    bool fFitSuccess = 0;
    int fNPeak = 0;
    TH1 *fHOrigin = NULL;
    TF1 *fFunction = NULL;
    TSpectrum * fSpectrum = NULL;
    TH1 *fHBackGround = NULL;
    TH1 *fHGauss = NULL;
    TH1 *fHAdd = NULL;

    double fFirstPeakHeightGuess = 1000;
    double fFirstPeakFitGuess = 350;
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

FitResult ReadSpectra(string sFile = "Bias-0Ch-12.root", string sHist = "h12", string fileNamePre = "", bool saveFlag = 0);

FitResult ReadSpectra(string sFile, string sHist, int PeakNum); // PeakNum means how many peaks should be fit




#endif
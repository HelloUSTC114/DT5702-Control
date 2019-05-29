#include "FitSiPMSPE.h"
#include <iostream>
#include <string>
#include "TFile.h"
#include "TCanvas.h"
using namespace std;

void ReadSpectra(string sFile="Bias-0Ch-11.root", string sHist="h11")
{
    auto file = new TFile(sFile.c_str());
    auto hist = (TH1F*) file -> Get(sHist.c_str());
    FitSpectrum sFitSpectrum(820);
    sFitSpectrum.Fit(hist, 3);
    TCanvas c("c","c", 1);
    sFitSpectrum.GetGauss() -> Draw();
    c.SaveAs("test.pdf");
    sFitSpectrum.GetOrigin() -> Draw();
    sFitSpectrum.GetSum()->Draw("same");
    c.SaveAs("Test2.pdf");
}
#include "FitSiPMSPE.h"
#include <iostream>
#include <string>
#include "TFile.h"
#include "TCanvas.h"
#include <fstream>
using namespace std;

#define VERBOSE


FitResult ReadSpectra(string sFile="Bias-0Ch-12.root", string sHist="h12")
{
    auto file = new TFile(sFile.c_str());
    auto hist = (TH1F*) file -> Get(sHist.c_str());
    FitSpectrum sFitSpectrum(820);
    if(file -> IsOpen() == false)
    {
        return {-1,-1};
    }
    sFitSpectrum.Fit(hist, 3);

    auto result = sFitSpectrum . GetGain();
    // cout << "Gain guess: " << sFitSpectrum.GetGainGuess() << endl;

    auto c = new TCanvas("c","c", 1);
    sFitSpectrum.GetGauss() -> Draw();
    c -> SaveAs(Form("%s.pdf", sHist.c_str()));

    auto c2 = new TCanvas("c2", "c2" , 1);
    sFitSpectrum.GetOrigin() -> Draw();
    sFitSpectrum.GetSum() -> Draw("same");
    c2 -> SaveAs(Form("Origin%s.pdf", sHist.c_str()));
    delete c;
    delete c2;

    file -> Close();
    file -> Delete();
    return result;
}

int main()
{
    ofstream fout("test.txt");
    // fout << "Gain\tPeak" << endl;
    for(int i = 0; i < 32; i++)
    {
        string file = (string)"Bias-0Ch-" + to_string(i) + ".root";
        string hist = (string)"h" + to_string(i);
        auto result = ReadSpectra(file, hist);
        fout << result.gain << "\t" << result.peak0 << endl;
    }
    fout . close();
    return 0;
}
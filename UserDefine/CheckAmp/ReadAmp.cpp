#include "TFile.h"
#include "TH1.h"
#include <iostream>
#include <string>
#include <fstream>
#include "TSystem.h"
#include "TPad.h"
#include "TVirtualPad.h"
#include "TCanvas.h"

using namespace std;
double ReadAmp(int channel, double & cut, TFile *f = NULL)
{
    string sChannel = to_string(channel);
    string sOFFileName = (string)"HVOF0Ch-" + sChannel + ".root";
    string sONFileName = (string)"HVON0Ch-" + sChannel + ".root";

    auto file1 = new TFile(sOFFileName.c_str());
    auto file2 = new TFile(sONFileName.c_str());

    string sH = (string)"h" + sChannel;

    auto h1 = (TH1F*) file1 -> Get(sH.c_str());
    h1 -> SetName(Form("Noiseh%d", channel));
    auto h2 = (TH1F*) file2 -> Get(sH.c_str());
    h2 -> SetName(Form("Originalh%d", channel));

    gSystem -> Exec("mkdir root pdf");
    
    auto c1 = new TCanvas("c1", "c1", 1);
    h1 -> Draw();
    c1 -> SetLogy();
    c1 -> SaveAs(Form("pdf/HVOF-ch%d.pdf", channel));
    h2 -> Draw();

    Double_t q1[1];
    const Double_t q2[1]{0.99};
    h1 -> GetQuantiles(1, q1, q2);
    cut = q1[0];
    cout << "Histogram cut: " << cut << endl;

    auto h3 = new TH1F(Form("Cuth%d", channel), Form("%d Signal Histo after cut", channel), 820, 0, 4100);
    // h3 -> SetDirectory(0);
    for(int i = 0; i < 820; i++)
    {
        if(h3->GetBinCenter(i) > cut)
            h3 -> SetBinContent(i, h2 -> GetBinContent(i));
        // cout << i << '\t' << h2 -> GetBinContent(i) << endl;
    }
    double sMean = h3 -> GetMean();
    cout << "Signal Mean: " << sMean << endl;
    h3 -> Draw();
    c1 -> SetLogy();
    c1 -> SaveAs(Form("pdf/HVON-ch%d.pdf", channel));
    delete c1;

    if(f)
    {
        h1 -> SetDirectory(0);
        h2 -> SetDirectory(0);
        h3 -> SetDirectory(0);

        f -> cd("Noise");
        h1 -> Write();
        cout << h1 -> GetDirectory() << endl;
        f -> cd("Original");
        h2 -> Write();
        f -> cd("Cut-Signal");
        h3 -> Write();

        delete h1;
        delete h2;
        delete h3;
    }

    file1 -> Close();
    file2 -> Close();
    return sMean;
}

void ReadAmp()
{
    ofstream fout("Result.txt");
    auto file = new TFile("AllChannelSignal.root", "recreate");
    file -> mkdir("Noise");
    file -> mkdir("Original");
    file -> mkdir("Cut-Signal");
    for(int i = 0; i < 32; i++)
    {
        double cut = 0;
        auto h = ReadAmp(i, cut, file);
        double mean = h;
        fout << i << "\t" << cut << "\t" << mean << endl;
        file -> cd();
    }
    file -> Close();
}
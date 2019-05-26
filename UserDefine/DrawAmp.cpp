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

void DrawAmp(TDirectory *sdir, string hName)
{
    auto dir = sdir;
    auto c1 = new TCanvas("c1", "c1", 4000, 3500);
    auto c2 = new TCanvas("c2", "c2", 4000, 3500);
    auto c3 = new TCanvas("c3", "c3", 4000, 3500);
    auto c4 = new TCanvas("c4", "c4", 4000, 3500);
    c1 -> Divide(4,2);
    c2 -> Divide(4,2);
    c3 -> Divide(4,2);
    c4 -> Divide(4,2);

    for(int i = 0; i < 2; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            cout << i * 4 + j << endl;

            auto h1 = dir -> Get(Form("%s%d", hName.c_str(), i*4+j));
            auto h2 = dir -> Get(Form("%s%d", hName.c_str(), 8 + i*4+j));
            auto h3 = dir -> Get(Form("%s%d", hName.c_str(), 16 + i*4+j));
            auto h4 = dir -> Get(Form("%s%d", hName.c_str(), 24 + i*4+j));
            c1 -> cd(i * 4 + j + 1) -> SetLogy();
            h1 -> Draw();
            c2 -> cd(i * 4 + j + 1) -> SetLogy();
            h2 -> Draw();
            c3 -> cd(i * 4 + j + 1) -> SetLogy();
            h3 -> Draw();
            c4 -> cd(i * 4 + j + 1) -> SetLogy();
            h4 -> Draw();

        }
    }

    c1 -> SaveAs(Form("%s1.pdf", hName.c_str()));
    c2 -> SaveAs(Form("%s2.pdf", hName.c_str()));
    c3 -> SaveAs(Form("%s3.pdf", hName.c_str()));
    c4 -> SaveAs(Form("%s4.pdf", hName.c_str()));

}

void DrawAmp()
{
    auto file = new TFile("AllChannelSignal.root");
    auto dir = file -> GetDirectory("Original");
    DrawAmp(dir, "Originalh");
    dir = file -> GetDirectory("Noise");
    DrawAmp(dir, "Noiseh");
    dir = file -> GetDirectory("Cut-Signal");
    DrawAmp(dir, "Cuth");
}
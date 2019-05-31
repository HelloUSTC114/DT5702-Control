#include "TFile.h"
#include "TCanvas.h"
#include "TH1.h"
void DrawAllCh()
{
    auto c = new TCanvas("c", "c", 3000, 2500);
    c -> Divide(4, 8);
    TH1F *hArray[32]{0};
    for(int i = 2; i < 32; i++)
    {
        auto file = new TFile(Form("Bias-0Ch-%d.root", i));

        auto h = (TH1F*)file -> Get(Form("h%d", i));
        h -> SetDirectory(NULL);
        hArray[i] = h;
        delete file;
    }
    for(int i = 2; i < 32; i++)
    {
        c -> cd(i - 1);
        hArray[i] -> Draw();
    }
    c -> SaveAs("Test.pdf");
}
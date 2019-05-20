#include <string>
#include "iostream"
#include "fstream"
#include "TFile.h"
#include "TSystem.h"
#include "TH1.h"
#include "TCanvas.h"

using namespace std;
void ReadGain(int ch)
{
    string sCh = to_string(ch);
    string sBias;
    auto file = new TFile();
    auto c = new TCanvas("c1", "c1", 1);

    string cmd = (string)"mkdir Ch-" + sCh;
    gSystem -> Exec(cmd.c_str());

    ofstream fout;
    fout . open("mean.txt");
    for(int bias:{0, 30, 60, 90, 120, 150, 180, 210, 240})
    {
        sBias = to_string(bias);
        string FileName = (string)"Bias-" + sBias + "Ch-" + sCh + ".root";
        file -> Open(FileName.c_str());

        string HistName = (string)"h" + sCh;
        auto h_temp = (TH1F*) file -> Get(HistName.c_str);
        c -> cd();
        h_temp -> Draw();
        string Path = (string)"Ch-" + sCh + "/";

        string PdfName = (string)"Bias" + sBias + ".pdf";
        c -> SaveAs((Path+PdfName).c_str());

        fout << bias << '\t' << h_temp -> GetMean() << endl;

        file -> Close();
    }
    delete file;
    delete c;
}
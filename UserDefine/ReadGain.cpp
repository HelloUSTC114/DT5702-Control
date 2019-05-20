/*
        Get mean value from each channel test
*/
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
    auto c = new TCanvas("c1", "c1", 1);

    string cmd = (string)"mkdir Ch-" + sCh;
    gSystem -> Exec(cmd.c_str());

    ofstream fout;
    string OutFile = (string)"Ch-" + sCh + "/" + "mean.txt";
    fout.open(OutFile.c_str());
    for(int bias:{0, 30, 60, 90, 120, 150, 180, 210, 240})
    {
        sBias = to_string(bias);
        string FileName = (string)"Bias-" + sBias + "Ch-" + sCh + ".root";
	auto file = new TFile(FileName.c_str());
	cout << "File Name: " << FileName << endl;
	if(!file->IsOpen()){cout << "Error, file not open" << endl;continue;}

        string HistName = (string)"h" + sCh;
        auto h_temp = (TH1F*) file -> Get(HistName.c_str());
<<<<<<< HEAD
        h_temp -> SetBinContent(1, 0);
=======
        h_temp -> SetBinContent(1,0);
>>>>>>> 4b82a6eb28e7612ddc56ccae97477360932f6349
	cout << "HistName: " << HistName << endl;
        c -> cd();
	if(!h_temp) continue;
        h_temp -> Draw();
        string Path = (string)"Ch-" + sCh + "/";

        string PdfName = (string)"Bias" + sBias + ".pdf";
        c -> SaveAs((Path+PdfName).c_str());

        fout << bias << '\t' << h_temp -> GetMean() << endl;

        file -> Close();
    	delete file;
    }
    delete c;
}

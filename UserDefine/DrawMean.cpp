// Draw mean value from gain test
#include "TStyle.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TF1.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

void DrawMean()
{
    auto c = new TCanvas("c", "c", 1);
    auto mg = new TMultiGraph();
    ifstream fin;
    ofstream fout("TestResult.txt");
    for(int i = 0; i < 32; i++)
    {
        string sCh = to_string(i);
        string sPath = (string)"Ch-" + sCh + "/";
        string filename = sPath + "mean.txt";
        auto tg = new TGraph();

        fin.open(filename);
        for(int i = 0; fin.is_open() && fin.eof() == false; i++)
        {
            int bias = 0;
            double mean = 0;
            fin >> bias;
            fin >> mean;
            if(mean != 0)
                tg -> SetPoint(i, bias, mean);
        }
        fin.close();

        auto f = new TF1("Line", "pol1", 0, 300);
        tg -> Fit(f, "qw", "", 0, 300);
        if(f->GetParameter(1) < 0.5)
        {
            cout << "There's something wrong with channel " << i << " bias DAC." << endl;
            fout << "There's something wrong with channel " << i << " bias DAC." << endl;
        }

        tg -> SetName(Form("Ch%s", sCh.c_str()));
        mg -> Add(tg);
    }
    c -> cd();
    mg -> Draw("PMC PLC ALP*");
    mg -> SetName("mg");
    mg -> SaveAs("Result.root");
    c -> BuildLegend();
}
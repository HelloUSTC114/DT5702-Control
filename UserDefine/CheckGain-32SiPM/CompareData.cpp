#include "TMultiGraph.h"
#include "TGraph.h"
#include "TCanvas.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

void CompareData()
{
    auto mg = new TMultiGraph();
    TGraph *arrTg[32];
    ifstream fin;

    for(int i = 0; i < 32; i++)
    {
        arrTg[i] = new TGraph();
        arrTg[i] -> SetName(Form("tg%d", i));
        arrTg[i] -> SetTitle(Form("Channel Grap %d;Measurement;Gain", i));
        if(i < 29)
            mg -> Add(arrTg[i]);
    }
    mg -> SetTitle("Graphs;Measurement;Gain");
    cout << "File\tChannel\tGain" << endl;
    for(int i = 1; i <= 5; i++)
    {
        string filename = (string)"RUN" + to_string(i) + ".txt";
        fin.open(filename);
        for(int ch = 0; ch < 32; ch++)
        {
            double gain;
            double peak;
            fin >> gain;
            fin >> peak;

            arrTg[ch] -> SetPoint(i - 1, i, gain);
            cout << i << '\t' << ch << '\t' << gain << '\t' << endl;
        }
        fin.close();
    }

    auto c = new TCanvas("c", "c", 1);
    c -> cd();
    mg -> Draw("PMC PLC AZ*L");
    c -> BuildLegend();
    mg -> SaveAs("TGraph.root");


}
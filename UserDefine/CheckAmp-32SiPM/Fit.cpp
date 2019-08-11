#include "TGraph.h"

#include <iostream>
#include <fstream>

int Fit()
{
    auto tg1 = new TGraph();
    auto tg2 = new TGraph();
    tg1->SetName("tg1");
    tg2->SetName("tg2");

    ifstream fin("h2.txt");
    int numberCounter = 0;
    for (; fin.good() && fin.is_open();)
    {
        int ampDAC;
        double Gain;
        fin >> ampDAC;
        fin >> Gain;
        if (Gain < 0)
        {
            continue;
        }
        // cout << numberCounter << endl;
        tg1->SetPoint(numberCounter, (63 - ampDAC), 1.0 / Gain);
        tg2->SetPoint(numberCounter, 1.0 / (63 - ampDAC), Gain);

        numberCounter++;
    }

    auto c1 = new TCanvas("c1", "c1", 1);
    auto c2 = new TCanvas("c2", "c2", 1);
    c1->cd();
    tg1->Draw("AZ*L");
    c2->cd();
    tg2->Draw("AZ*L");

    tg1->SetTitle("Amplifier Calibration;63-DAC;1/Gain");
    tg2->SetTitle("Amplifier Calibration;1/(63-DAC);Gain");

    tg1 -> Fit("pol1");
    tg2 -> Fit("pol1");

    tg1->SaveAs("tg1.root");
    tg2->SaveAs("tg2.root");
    // cout << "Test" << endl;
    return 0;
}
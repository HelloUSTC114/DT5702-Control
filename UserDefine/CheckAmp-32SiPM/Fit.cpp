#include "TGraph.h"
#include "TCanvas.h"
#include "TF1.h"

#include <iostream>
#include <fstream>
using namespace std;
int Fit(int ch)
{
    auto tg1 = new TGraph();
    auto tg2 = new TGraph();
    tg1->SetName("tg1");
    tg2->SetName("tg2");

    string sCh = to_string(ch);
    ifstream fin((string) "h" + sCh + ".txt");
    int numberCounter = 0;

    bool Found30Flag = 0;
    double Amp30Gain = 0;

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
        tg1->SetPoint(numberCounter, ampDAC, 1.0 / Gain);

        if (ampDAC == 30)
        {
            Found30Flag = 1;
            Amp30Gain = Gain;
        }

        numberCounter++;
    }

    auto c1 = new TCanvas("c1", "c1", 1);
    auto c2 = new TCanvas("c2", "c2", 1);
    c1->cd();
    tg1->Draw("AZ*L");

    tg1->SetTitle("Amplifier Calibration;Amp DAC;1/Gain");
    auto fun1 = new TF1("fitfun", "pol1", 0, 63);
    tg1->Fit(fun1, "", "", 0, 63);

    tg1->SaveAs(Form("tgCh%d.root", ch));
    // cout << "Test" << endl;

    auto tg3 = new TGraph();
    tg3->SetName("tg3");
    if (Found30Flag == 0)
    {
        Amp30Gain = 1 / (fun1 -> Eval(30));
    }

    for (int i = 0; i < tg1->GetN(); i++)
    {
        double x = 0, y = 0;
        tg1 -> GetPoint(i, x, y);
        tg3 -> SetPoint(i, x, (y * Amp30Gain));
    }

    tg3 -> Fit(fun1, "", "", 0, 63);
    c2 -> cd();
    tg3 -> Draw("AZ*L");
    tg3 -> SetTitle("Amplifier Calibration;DAC;Gain of DAC=30 divide this gain");
    tg3 -> SaveAs(Form("CaliResultCh%d", ch));

    return 0;
}
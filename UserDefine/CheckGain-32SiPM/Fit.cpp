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


    for (; fin.good() && fin.is_open();)
    {
        int bias;
        double Gain;
        fin >> bias;
        fin >> Gain;
        if (Gain < 0)
        {
            continue;
        }
        // cout << numberCounter << endl;
        tg1->SetPoint(numberCounter, bias, Gain);


        numberCounter++;
    }

    auto c1 = new TCanvas("c1", "c1", 1);
    c1->cd();

    tg1->SetTitle("Amplifier Calibration;Bias DAC;Gain");
    auto fun1 = new TF1("fitfun", "pol1", 0, 255);
    tg1->Fit(fun1, "", "", 0, 255);
    tg1->Draw("AZ*L");

    tg1->SaveAs(Form("BiastgCh%d.root", ch));
    fun1 -> SaveAs(Form("BiasCh%dFun.root", ch));
    // cout << "Test" << endl;


    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        cout << "Please input arguments like: [group No.]" << endl;
        return 0;
    }
    int group = 0;
    string sGroup;
    if (argc > 1)
    {
        sGroup = argv[1];
        try
        {
            group = stoi(sGroup);
        }
        catch (const std::invalid_argument &err)
        {
            cerr << "Error! wrong input format, please input like [group No.]" << endl;
            return 0;
        }
    }

    int ch1 = group * 2;
    int ch2 = ch1 + 1;

    Fit(ch1);
    Fit(ch2);
    return 0;
}
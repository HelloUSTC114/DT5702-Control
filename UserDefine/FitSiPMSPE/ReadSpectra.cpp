#define VERBOSE
#include "FitSiPMSPE.h"
#include <iostream>
#include <string>
#include "TFile.h"
#include "TCanvas.h"
#include "TStyle.h"
#include <fstream>

#include "TMultiGraph.h"
#include "TGraph.h"
using namespace std;


int main(int argc, char **argv)
{
    gStyle->SetOptFit(1111);
    int bias = 0;
    int ch = 0;
    ofstream fout("test.txt", ios::app);

    if (argc == 3)
    {
        bias = atoi(argv[1]);
        ch = atoi(argv[2]);

        // string file = (string)"Bias-0Ch-" + to_string(i) + ".root";
        string file = (string) "Bias-" + to_string(bias) + "Ch-" + to_string(ch) + ".root";
        // string hist = (string)"h" + to_string(i);
        string hist = (string) "h" + to_string(ch);
        auto result = ReadSpectra(file, hist, "", 1);
        fout << "Bias: " << bias << '\t' << "Channel: " << ch << endl;
        fout << result.gain << "\t" << result.peak0 << endl;
        fout.close();

        return 1;
    }

    // fout << "Gain\tPeak" << endl;
    // for(int i = 0; i < 32; i++)
    auto mg = new TMultiGraph();
    auto c = new TCanvas("c", "c", 1);
    for (int ch = 0; ch < 32; ch++)
    {
        if (ch == 16)
            continue;
        if (ch == 17)
            continue;
        auto tg = new TGraph();
        for (int bias = 0, counter = 0; bias <= 240; bias += 30)
        {
            cout << "Channel: " << ch << '\t' << "bias: " << bias << endl;
            // string file = (string)"Bias-0Ch-" + to_string(i) + ".root";
            string file = (string) "Bias-" + to_string(bias) + "Ch-" + to_string(ch) + ".root";
            // string hist = (string)"h" + to_string(i);
            string hist = (string) "h" + to_string(ch);
            string savefileName = (string) "Bias-" + to_string(bias) + "Ch-" + to_string(ch);
            auto result = ReadSpectra(file, hist, savefileName);
            fout << "Bias: " << bias << '\t' << "Channel: " << ch << endl;
            fout << result.gain << "\t" << result.peak0 << endl;

            // if(result.gain < 0 || result.gain > 500)
            // {
            //     cout << "Error!" << endl;
            //     // continue;
            // }
            if (result.gain == -1)
            {
                // cout << "Channel: " << ch << "\t Bias: " << bias << endl;
                cout << "Fit Error" << endl;
                continue;
            }
            cout << "Test: " << endl;
            tg->SetPoint(counter, bias, result.gain);
            counter++;

            cout << endl;
        }
        if (tg->GetN() != 0)
        {
            tg->Fit("pol1", "RQ", "", 0, 240);
            auto fun = tg->GetFunction("pol1");
            auto slope = fun->GetParameter(1);
            auto chi2 = fun->GetChisquare();
            // if(slope < 0 || slope >  )
            cout << "channel: " << ch << endl;
            cout << "slope: " << slope << endl;
            cout << "chi2: " << chi2 << endl
                 << endl;
            c->cd();
            tg->Draw("AZC*");
            c->BuildLegend();
            c->SaveAs(Form("Ch-%s.pdf", to_string(ch).c_str()));

            mg->Add(tg);
        }
    }

    c->cd();
    mg->GetYaxis()->SetRangeUser(0, 500);
    mg->Draw("PLC PMC AZL*");

    c->SaveAs("mg.pdf");
    mg->SaveAs("mg.root");
    fout.close();
    return 2;
}
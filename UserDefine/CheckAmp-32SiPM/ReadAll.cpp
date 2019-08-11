#include "FitSiPMSPE.h"

#include <iostream>
#include "TFile.h"
#include <string>
#include "TStyle.h"

#include <fstream>

int main(int argc, char ** argv)
{
    string sGroup;
    if(argc > 1)
    {
        sGroup = argv[1];
    }

    int group = 1;
    try
    {
        group = stoi(sGroup);
    }
    catch (invalid_argument err)
    {
        cerr << "Error! Invalid input group number." << endl;
    }

    int ch1 = 2 * group;
    int ch2 = ch1 + 1;
    
    gStyle -> SetOptFit(1111);
    using namespace std;

    ofstream fout1((string)"h" + to_string(ch1) + ".txt");
    ofstream fout2((string)"h" + to_string(ch2) + ".txt");

    for(int ampGain:{20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49})
    {
        string sGain = (string)"Gain-" + to_string(ampGain);
        string sBias = (string)"Bias-0-";
        string sCh1 = "Ch-" + to_string(ch1);
        string sCh2 = "Ch-" + to_string(ch2);

        string sName1 = sBias + sGain + sCh1 + ".root";
        string sName2 = sBias + sGain + sCh2 + ".root";
        string sHist1 = (string)"h" + to_string(ch1);
        string sHist2 = (string)"h" + to_string(ch2);

        auto result1 = ReadSpectra(sName1, sHist1, sGain+sHist1);
        auto result2 = ReadSpectra(sName2, sHist2, sGain+sHist2);

        fout1 << ampGain << '\t' << result1.gain << endl;
        fout2 << ampGain << '\t' << result2.gain << endl;
    }

    fout1.close();
    fout2.close();
    return 0;
}
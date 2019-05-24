#include "FEBDAQMULT.C"
#include <string>
#include <iostream>
using namespace std;

int fInit = 0;

void CheckGain2(int group);
void CheckGain2Loop(int startIndex)
{
    if(!fInit)
    {
        FEBDAQMULT("enp5s0f0");
        UpdateConfig();
        fInit = 1;
    }

    for(int group = startIndex; group < 16; group++)
    {
        CheckGain2(group);
    }
}

void CheckGain2(int group)
{
    if(!fInit)
    {
        FEBDAQMULT("enp5s0f0");
        UpdateConfig();
        fInit = 1;
    }

        string sTemp;
        HVOF();
        gSystem -> ProcessEvents();
        cout << "Measuring group: " << group << " Please check your connection, and press enter to continue" << endl;
        cin >> sTemp;
        HVON();
        fChanEnaTrig[32] -> SetOn(kFALSE);
        // fChanEnaTrig[32] -> SetOn(kFALSE);
        Reset();
        SendConfig();
        gSystem -> ProcessEvents();
        gSystem->Sleep(1000);

        for(int bias:{0, 30, 60, 90, 120, 150, 180, 210, 240})      // Set different bias
        {
            int ch1 = group * 2;
            int ch2 = ch1 + 1;

            for (int i = 0; i < 32; i++)    // Set bias and set amplifier
            {
                fChanBias[i]->SetNumber(0);
                fChanGain[i]->SetNumber(50);
                fChanEnaTrig[i] -> SetOn(kFALSE);
            }
            fChanBias[ch1] -> SetNumber(bias);
            fChanBias[ch2] -> SetNumber(bias);

            SendConfig();
            gSystem -> ProcessEvents();
            gSystem->Sleep(1000);
            fChanEnaTrig[ch1] -> SetOn();
            fChanEnaTrig[ch2] -> SetOn();

            if(group != 0)
            {
                fChanEnaTrig[ch1 - 2] -> SetOn(kFALSE);
                fChanEnaTrig[ch2 - 2] -> SetOn(kFALSE);
            }
            Reset();
            SendConfig();
            gSystem -> ProcessEvents();
            gSystem->Sleep(1000);

            StartDAQ(10000);

            string filename1 = (string) "Bias-" + to_string(bias) + "Ch-" + to_string(ch1);
            string filename2 = (string) "Bias-" + to_string(bias) + "Ch-" + to_string(ch2);

            c -> cd(ch1) -> SaveAs((filename1 + ".pdf").c_str());
            c -> cd(ch2) -> SaveAs((filename2 + ".pdf").c_str());
            hst[ch1]->SaveAs((filename1 + ".root").c_str());
            hst[ch2]->SaveAs((filename2 + ".root").c_str());
        }

}
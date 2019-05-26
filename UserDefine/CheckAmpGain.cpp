#include "FEBDAQMULT.C"
#include <string>
#include <iostream>
using namespace std;

int fInit = 0;

void CheckGain2(int channel);
void CheckGain2Loop(int startIndex)
{
    if(!fInit)
    {
        FEBDAQMULT("enp5s0f0");
        UpdateConfig();
        fInit = 1;
    }

    for(int channel = startIndex; channel < 32; channel++)
    {
        CheckGain2(channel);
    }
}

void CheckGain2(int channel)
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
        cout << "Measuring channel: " << channel << " Please check your connection, and press enter to continue" << endl;
        cin >> sTemp;
        // HVON();
        fChanEnaTrig[32] -> SetOn();
        // fChanEnaTrig[32] -> SetOn(kFALSE);
        Reset();
        SendConfig();
        gSystem -> ProcessEvents();
        gSystem->Sleep(1000);

        for(int bias:{0})      // Set different bias
        {
            int ch1 = channel;

            for (int i = 0; i < 32; i++)    // Set bias and set amplifier
            {
                fChanBias[i]->SetNumber(0);
                fChanGain[i]->SetNumber(52);
                fChanEnaTrig[i] -> SetOn(kFALSE);
            }
            fChanBias[ch1] -> SetNumber(bias);

            SendConfig();
            gSystem -> ProcessEvents();
            gSystem->Sleep(1000);
            fChanEnaTrig[ch1] -> SetOn();

            if(channel != 0)
            {
                fChanEnaTrig[ch1 - 1] -> SetOn(kFALSE);
            }
            Reset();
            SendConfig();
            gSystem -> ProcessEvents();
            gSystem->Sleep(1000);

            string filename1 = (string) "HVOF" + to_string(bias) + "Ch-" + to_string(ch1);


            SetThresholdDAC1(250);
            SetThresholdDAC2(250);
            StartDAQ(10000);
            c -> cd(channel) -> SaveAs((filename1 + ".pdf").c_str());
            hst[ch1]->SaveAs((filename1 + ".root").c_str());

            HVON();
            SetThresholdDAC1(280);
            SetThresholdDAC2(280);
            StartDAQ(50000);
            filename1 = (string) "HVON" + to_string(bias) + "Ch-" + to_string(ch1);
            c -> cd(channel) -> SaveAs((filename1 + ".pdf").c_str());
            hst[ch1]->SaveAs((filename1 + ".root").c_str());

            HVOF();


        }

}
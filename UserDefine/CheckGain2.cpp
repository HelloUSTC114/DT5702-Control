#include "../FEBDAQMULT.C"
#include <string>
#include <iostream>
using namespace std;

void CheckGain2(const char * iface = "enp5s0f0")
{
    FEBDAQMULT(iface);
    UpdateConfig();

    for(int channel = 0; channel < 32; channel++)
    {
        HVOF();
        cout << "Measuring channel: " << channel << " Please check your connection, and press enter to continue" << endl;
        cin.get();
        HVON();
        fChanEnaTrig[32] -> SetOn();
        // fChanEnaTrig[32] -> SetOn(kFALSE);
        Reset();
        SendConfig();
        gSystem -> ProcessEvents();
        gSystem->Sleep(1000);
        for (int i = 0; i < 32; i++)    // Set bias and set amplifier
        {
            fChanBias[i]->SetNumber(0);
            fChanGain[i]->SetNumber(64);
            fChanEnaTrig[i] -> SetOn(kFALSE);
        }
        SendConfig();
        gSystem -> ProcessEvents();
        gSystem->Sleep(1000);

        for(int bias:{0, 30, 60, 90, 120, 150, 180, 210, 240})      // Set different bias
        {
            int ch1 = channel;

            fChanEnaTrig[ch1] -> SetOn();
            if(channel != 0)
            {
                fChanEnaTrig[ch1 - 1] -> SetOn(kFALSE);
            }
            Reset();
            SendConfig();
            gSystem -> ProcessEvents();
            gSystem->Sleep(1000);

            StartDAQ(10000);

            string filename1 = (string) "Bias-" + to_string(bias) + "Ch-" + to_string(ch1);

            c -> cd(channel) -> SaveAs((filename1 + ".pdf").c_str());
            hst[ch1]->SaveAs((filename1 + ".root").c_str());
        }
    }
}
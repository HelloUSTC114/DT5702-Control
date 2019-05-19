#include "../FEBDAQMULT.C"
#include <string>
using namespace std;

void CheckGain(const char * iface = "enp5s0f0")
{
    UpdateConfig();

    for(int bias:{0, 30, 60, 90, 120, 150, 180, 210, 240})      // Set different bias
    {
        fChanEnaTrig[32] -> SetOn();
        fChanEnaTrig[32] -> SetOn(kFALSE);
        Reset();
        SendConfig();
        for (int i = 0; i < 32; i++)    // Set bias and set amplifier
        {
            fChanBias[i]->SetNumber(bias);
            fChanGain[i]->SetNumber(30);
            fChanEnaTrig[i] -> SetOn(kFALSE);
        }
        SendConfig();

        for(int group = 0; group < 16; group++)
        {
            int ch1 = group * 2;
            int ch2 = group * 2 + 1;

            fChanEnaTrig[ch1] -> SetOn();
            fChanEnaTrig[ch2] -> SetOn();
            if(group != 0)
            {
                fChanEnaTrig[ch1 - 2] -> SetOn(kFALSE);
                fChanEnaTrig[ch2 - 2] -> SetOn(kFALSE);
            }
            Reset();
            SendConfig();

            StartDAQ(10000);

            string filename1 = (string) "Bias-" + to_string(bias) + "Ch-" + to_string(ch1);
            string filename2 = (string) "Bias-" + to_string(bias) + "Ch-" + to_string(ch2);

            hst[ch1]->SaveAs((filename1 + ".pdf").c_str);
            hst[ch1]->SaveAs((filename1 + ".root").c_str);
            hst[ch2]->SaveAs((filename2 + ".pdf").c_str);
            hst[ch2]->SaveAs((filename2 + ".root").c_str);
        }
    }
}
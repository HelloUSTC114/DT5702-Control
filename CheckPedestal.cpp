/* This file is used as checking all SiPM gains before using.
And Can be used to fine tune common bias after getting the largest gain channel.
*/

#include "FEBDAQMULT.C"
#include <string>
#include <iostream>
using namespace std;

int fInit = 0;
int fDAQNum = 50000;

void ExecAfterMeasure();


void CheckPedestal()
{
    if (!fInit)
    {
        FEBDAQMULT("enp5s0f0");
        UpdateConfig();
        fInit = 1;
    }

    string sTemp;
    HVOF();
    gSystem->ProcessEvents();

    int dac1 = 250;
    SetThresholdDAC1(dac1);
    SetThresholdDAC2(dac1);

    fChanEnaTrig[32]->SetOn(kFALSE);

    Reset();
    SendConfig();
    gSystem->ProcessEvents();
    gSystem->Sleep(1000);

    for (int bias : {0}) // Set different bias
    {

        for (int i = 0; i < 32; i++) // Set bias and set amplifier
        {
            fChanBias[i]->SetNumber(0);
            fChanGain[i]->SetNumber(0);
            fChanEnaTrig[i]->SetOn(kFALSE);
        }

        SendConfig();
        gSystem->ProcessEvents();
        gSystem->Sleep(1000);


        for (int ampGain : {0})
        {

            Reset();
            gSystem->ProcessEvents();
            gSystem->Sleep(1000);

            StartDAQ(fDAQNum);

        }
    }

    ExecAfterMeasure();
}

void ExecAfterMeasure()
{
    for(int i = 0; i < 32; i++)
    {
        hst[i] -> SaveAs(Form("Pedestal-Ch%d.root", i));
    }
    gSystem -> Exec("mkdir root");
    gSystem -> Exec("mv *.root root");
    gSystem -> Exec(Form("mkdir Pedestal-Board%d", mac5));
    gSystem -> Exec(Form("mv root Pedestal-Board%d", mac5));
}
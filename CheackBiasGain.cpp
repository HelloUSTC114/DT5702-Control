/* This file is used as checking all SiPM gains before using.
And Can be used to fine tune common bias after getting the largest gain channel.
*/

#include "FEBDAQMULT.C"
#include <string>
#include <iostream>
using namespace std;

int fInit = 0;
int fDAQNum = 100000;

void CheckGain2(int group);
void ExecAfterMeasure(int group);

void CheckGain2Loop(int startIndex = 1)
{
    if (!fInit)
    {
        FEBDAQMULT("enp5s0f0");
        UpdateConfig();
        fInit = 1;
    }

    for (int group = startIndex; group < 16; group++)
    {
        CheckGain2(group);
    }
}

void CheckGain2(int group)
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

    HVON();
    fChanEnaTrig[32]->SetOn(kFALSE);

    Reset();
    SendConfig();
    gSystem->ProcessEvents();
    gSystem->Sleep(1000);

    for (int bias : {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 255}) // Set different bias
    {
        int ch1 = group * 2;
        int ch2 = ch1 + 1;

        for (int i = 0; i < 32; i++) // Set bias and set amplifier
        {
            fChanBias[i]->SetNumber(0);
            fChanGain[i]->SetNumber(0);
            fChanEnaTrig[i]->SetOn(kFALSE);
        }
        fChanBias[ch1]->SetNumber(bias);
        fChanBias[ch2]->SetNumber(bias);

        SendConfig();
        gSystem->ProcessEvents();
        gSystem->Sleep(1000);

        for (int i = 0; i < 32; i++)
        {
            fChanEnaTrig[i]->SetOn(kFALSE);
            fChanGain[i]->SetNumber(0);
            fChanEnaTrig[i]->SetOn(kFALSE);
            fChanGain[i]->SetNumber(0);
        }

        // for (int ampGain : {20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49})
        for (int ampGain : {30})
        {
            fChanEnaTrig[ch1]->SetOn();
            fChanGain[ch1]->SetNumber(ampGain);
            fChanEnaTrig[ch2]->SetOn();
            fChanGain[ch2]->SetNumber(ampGain);

            Reset();
            SendConfig();
            gSystem->ProcessEvents();
            gSystem->Sleep(1000);

            StartDAQ(fDAQNum);

            string filename1 = (string) "Bias-" + to_string(bias) + "-Gain-" + to_string(ampGain) + "Ch-" + to_string(ch1);
            string filename2 = (string) "Bias-" + to_string(bias) + "-Gain-" + to_string(ampGain) + "Ch-" + to_string(ch2);

            c->SaveAs((filename1 + ".pdf").c_str());
            c->SaveAs((filename2 + ".pdf").c_str());
            hst[ch1]->SaveAs((filename1 + ".root").c_str());
            hst[ch2]->SaveAs((filename2 + ".root").c_str());
        }
    }

    ExecAfterMeasure(group);
}

void ExecAfterMeasure(int group)
{
    gSystem->Exec("mkdir pdf root");
    gSystem->Exec("mv *.pdf pdf");
    gSystem->Exec("mv *.root root");
    gSystem->Exec("cp UserDefine/CheckGain-32SiPM/CalcBias UserDefine/FitSiPMSPE/test2 UserDefine/CheckGain-32SiPM/Fit.cpp  root/");
    gSystem->Exec(Form("mkdir Group%d-Bias", group));
    gSystem->Exec(Form("mv pdf root Group%d-Bias", group));
}
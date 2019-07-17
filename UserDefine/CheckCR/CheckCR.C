/* This file is used as checking all SiPM gains before using.
And Can be used to fine tune common bias after getting the largest gain channel.
*/

#include "FEBDAQMULT.C"
#include <string>
#include <iostream>
using namespace std;

int fInit = 0;
int fDAQNum = 6000;

void SendConfigArraytoBoard(int feb, UChar_t *bufSCR)
{
  SetDstMacByIndex(feb);
  t->SendCMD(t->dstmac, FEB_WR_SCR, 0x0000, bufSCR);
  t->SendCMD(t->dstmac, FEB_WR_PMR, 0x0000, bufPMR);

}

void SendMaskArraytoBoard(int feb, uint32_t trigmask)
{
  uint8_t bufFIL[256]{0};
  *((uint32_t *) (& (bufFIL[0]))) = trigmask;
  SetDstMacByIndex(feb);
  t -> SendCMD(t->dstmac, FEB_WR_FIL, 0x0000, bufFIL);
}

uint32_t EnableChannelMask(int ch, bool Enable, uint32_t& PreMask)
{
  if(Enable)
    PreMask = PreMask | (0x1 << ch);
  else
    PreMask = PreMask & ~(0x1 << ch);
  return PreMask;
}

void UpdateSCConfigtoArray(int feb, UChar_t*bufSCR)    // Control update config from file for specific board
{
  char bsname[32];

  //t->ReadBitStream("CITIROC_SCbitstream_TESTS.txt",bufSCR);
  // for (int feb = 0; feb < t->nclients; feb++)
  {
    SetDstMacByIndex(feb);
    //t->dstmac[5]=0xff; //Broadcast
    sprintf(bsname, "CITIROC_SC_SN%03d.txt", t->dstmac[5]);
    //if(!(t->ReadBitStream(bsname,bufSCR))) t->ReadBitStream("CITIROC_SC_DEFAULT.txt",bufSCR);
    if (!(t->ReadBitStream(bsname, bufSCR)))
      t->ReadBitStream("CITIROC_SC_PROFILE1.txt", bufSCR);

  }
  //fNumberEntry755->SetNumber();
}

UChar_t *EnaChTrig(int ch, bool Enable, UChar_t *bufSCR)
{
  ConfigSetBit(bufSCR, 1144, 265+ch, Enable);
  return bufSCR;
}

void EnableCh(int ch, bool Enable, UChar_t * bufSCR, uint32_t& trigmask)
{
  EnableChannelMask(ch, Enable, trigmask);
  EnaChTrig(ch, Enable, bufSCR);
}



void CheckCR()
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
  // fChanEnaTrig[32]->SetOn(kFALSE);
  UChar_t bufscr1[1500];
  UChar_t bufscr2[1500];
  uint32_t trigmask1 = 0x0;
  uint32_t trigmask2 = 0x0;

  UpdateSCConfigtoArray(1,bufscr1);
  UpdateSCConfigtoArray(2,bufscr2);

  fChanEnaTrig[32] -> SetOn();
  SendConfig();
  gSystem -> Sleep(1);

  for(int i = 0; i < 32; i++)
  {
    EnableCh(i, 1, bufscr1, trigmask1);
    if(i == 10 && i == 11)
      EnableCh(i, 1, bufscr2, trigmask2);
  }

  ConfigSetBit(bufscr1, 1144, 1139, 1);
  ConfigSetBit(bufscr2, 1144, 1139, 1);
  SendConfigArraytoBoard(1,bufscr1);
  SendConfigArraytoBoard(2,bufscr2);
  SendMaskArraytoBoard(1, trigmask1);
  SendMaskArraytoBoard(2, trigmask2);

  cout << "John" << endl;

  ConfigSetBit(bufscr1, 1144, 1139, 0);
  ConfigSetBit(bufscr2, 1144, 1139, 0);
  SendConfigArraytoBoard(1, bufscr1);
  SendConfigArraytoBoard(2, bufscr2);
  SendMaskArraytoBoard(1, trigmask1);
  SendMaskArraytoBoard(2, trigmask2);

  // StartDAQ(fDAQNum);





  Reset();
  gSystem->ProcessEvents();


}
#include "FitSiPMSPE.h"
using namespace std;

MultiGauss::MultiGauss(int nPeak)
{
    SetNPeak(nPeak);
}

void MultiGauss::SetNPeak(int nPeak)
{
    if (nPeak < 0)
    {
        cerr << "Error! Wrong peak number." << endl;
        fNPeak = 0;
    }
    else
    {
        fNPeak = nPeak;
    }
}

double MultiGauss::operator()(double* x, double *par) const
{
    double y = 0;
    for(int i = 0; i < fNPeak; i++)
    {
        y += par[i * 3 + 0] * TMath::Gaus(x[0], par[i * 3 + 1], par[i * 3 + 2]);
    }
    return y;
}

int MultiGauss::GetNPeak() const
{
    return fNPeak;
}

FitSpectrum::FitSpectrum(UInt_t bins)
{
    fBinsX = bins;
    fHGauss = new TH1D("PureGauss", "Pure Gauss Spectrum", fBinsX, 0, 4100);
    fHAdd = new TH1D("Sum", "Add Fit Fun & Background", fBinsX, 0, 4100);
    fSpectrum = new TSpectrum();

}

bool FitSpectrum::Clear()
{
    if(fHistFlag)
    {
        fHBackGround ->Delete();
        fHBackGround = NULL;
        fHistFlag = false;
    }
    if(fFitFlag)
    {
        delete fFunction;
        fFunction = NULL;

        fFitFlag = false;
    }
    return true;
}

FitSpectrum::~FitSpectrum()
{
    Clear();
    fHGauss -> Delete();
    fHGauss = NULL;
    fHAdd->Delete();
    fHAdd = NULL;
    fSpectrum->Delete();
    fSpectrum = NULL;
}

bool FitSpectrum::SetHist(TH1 *h)
{
    if(!h)
    {
        cerr << "Error! Input invalid histogram." << endl;
        return false;
    }
    Clear();

    fHOrigin = h;
    fHBackGround = fSpectrum -> Background(fHOrigin);
    fHGauss ->Add(fHOrigin, fHBackGround, 1, -1);
    cout << "fHOrigin: " << fHOrigin -> GetNbinsX() << endl;
    cout << "fHBackGround: " << fHBackGround -> GetNbinsX() << endl;
    cout << "fHGauss: " << fHGauss -> GetNbinsX() << endl;

    fHistFlag = 1;
    return true;
}

bool FitSpectrum::FitHist(UInt_t nGauss)
{
    if(!fHistFlag)
    {
        return false;
    }
    if(fFitFlag)
    {
        return true;
    }

    fFitFlag = 1;

    UInt_t sNGauss = nGauss;
    MultiGauss sMultiGauss(0);
    TF1* sFitFunArray[nGauss];

    // Start to fit
    for(int peakIndex = 0; peakIndex < nGauss; peakIndex++)
    {
        // First to create a function with i peaks;
        int peakNumTemp = peakIndex + 1;
        sMultiGauss.SetNPeak(peakNumTemp);

        sFitFunArray[peakIndex] = new TF1(Form("FitFun%d", peakIndex), sMultiGauss, 0, 4100, 3 * peakNumTemp);
        for(int i =0 ; i < 3 * peakNumTemp; i++)
        {
            // Set limits of fit function;
            sFitFunArray[peakIndex] -> SetParLimits(i, 0, 20000);   // Set all parameters larger than 0
        }

        // Judge if this time is the first time to fit. If yes, initialize function
        if(peakIndex == 0)
        {
            // Set first Peak limits
            sFitFunArray[peakIndex] -> SetParLimits(1, fFirstPeakMeanStartLimit, fFirstPeakMeanEndLimit);
            sFitFunArray[peakIndex] -> SetParameter(0, 60);
            sFitFunArray[peakIndex] -> SetParameter(1, 300);
            sFitFunArray[peakIndex] -> SetParameter(2, 20);

            // Start position should be given and fixed in further fit
            fHGauss -> Fit(sFitFunArray[peakIndex], "-", "", fFirstPeakStartFitPoint, fFirstPeakStartFitPoint + 2 * fFirstPeakSigma);
        }
        else
        {
            // First, get the mean position of last peak
            double sLastPeakMean = sFitFunArray[peakIndex - 1] -> GetParameter(3 * (peakIndex - 1) + 1);
            // Define Start position, in which case, I set it at first peak minus 5 sigma
            double start = sFitFunArray[0]->GetParameter(1) - 5 * sFitFunArray[0]->GetParameter(2);
            // Define end position, I set it at the last fit peak plus 160
            double end = sLastPeakMean + 2 * fGainGuess;

            // pass all parameter of the last fit function to the new function just created
            for (int par_index = 0; par_index < 3 * peakIndex; par_index++)
            {
                sFitFunArray[peakIndex]->SetParameter(par_index, sFitFunArray[peakIndex - 1]->GetParameter(par_index));
            }

            // Set Initial value of parameters
            sFitFunArray[peakIndex] -> SetParameter(3 * peakIndex + 1, sLastPeakMean + fGainGuess);
            sFitFunArray[peakIndex] -> SetParLimits(3 * peakIndex + 1, sLastPeakMean, sLastPeakMean + 2 * fGainGuess);
            sFitFunArray[peakIndex] -> SetParameter(3 * peakIndex + 2, 10);
            sFitFunArray[peakIndex] -> SetParLimits(3 * peakIndex + 2, 5, 40);

            fHGauss -> Fit(sFitFunArray[peakIndex], "-", "", start, end);
        }
    }

    // Add fit function to background to get the final fit result.
    fFunction = sFitFunArray[sNGauss - 1];
    fHAdd -> Reset();
    fHAdd -> Add(fHBackGround);
    fHAdd -> Add(fFunction);
    cout << "fHAdd: " << fHAdd -> GetNbinsX() << endl;

    for(int i = 0; i < sNGauss - 1; i++)
    {
        sFitFunArray[i] -> Delete();
    }

    return true;

}

bool FitSpectrum::Fit(TH1 *h, UInt_t nGauss)
{
    return SetHist(h) && FitHist(nGauss);
}
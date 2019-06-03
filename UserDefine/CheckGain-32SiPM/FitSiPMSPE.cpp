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
    
    fHGauss -> SetDirectory(0);
    fHAdd -> SetDirectory(0);
    fSpectrum = new TSpectrum();

}

bool FitSpectrum::Clear()
{
    fNPeak = 0;
    if(fHistFlag)
    {
        // fHBackGround -> Delete();
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
    fSpectrum->Delete();
    fSpectrum = NULL;
    fHGauss -> Delete();
    fHGauss = NULL;
    fHAdd->Delete();
    fHAdd = NULL;
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
    fNPeak = nGauss;

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
            fHGauss -> Fit(sFitFunArray[peakIndex], "Q", "", fFirstPeakStartFitPoint, fFirstPeakStartFitPoint + 2 * fFirstPeakSigma);
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

            fHGauss -> Fit(sFitFunArray[peakIndex], "Q", "", start, end);
        }
    }

    // Add fit function to background to get the final fit result.
    fFunction = sFitFunArray[sNGauss - 1];
    fHAdd -> Reset();
    fHAdd -> Add(fHBackGround);
    fHAdd -> Add(fFunction);

    for(int i = 0; i < sNGauss - 1; i++)
    {
        sFitFunArray[i] -> Delete();
    }

    return true;

}

bool FitSpectrum::Fit(TH1 *h, UInt_t nGauss)
{
    auto a = SetHist(h);
    EstimateGain();
    auto b = FitHist(nGauss);
    return a&&b;// && FitHist(nGauss);
}

double FitSpectrum::EstimateGain()
{
    if(!fHistFlag)
    {
        cerr << "Error, Histogram not set" << endl;
        return -1;
    }

    fSpectrum -> Search(fHOrigin, 5, "", 0.001);

    int peaks = fSpectrum -> GetNPeaks();
    auto peaksX = fSpectrum -> GetPositionX();
    auto peaksY = fSpectrum -> GetPositionY();



    // Estimate gain
    // Put all points into a map, sort all peaks Y
    map<double, double> Points;
    map<double, double> mapPeakX;

    for(int i = 0; i < peaks; i++)
    {
        mapPeakX.insert(pair<double, double>(peaksX[i], peaksY[i]));
    }

    // Getkl Peak position info
    map<double, double>::iterator iter;

    for(iter = mapPeakX.begin(); iter->first < 150; iter++) ;   // if peak0 is at position smaller than 150, continue
    
    auto peak0 = iter -> first;

    fFirstPeakStartFitPoint = fFirstPeakMeanStartLimit = peak0 - 20;
    fFirstPeakMeanEndLimit = peak0 + 20;

    // Get gain
    /*
    double largestPeakX = peaksX[0];
    double largestPeakY = peaksY[0];

    double subLargestPeakX = peaksX[1];
    double subLargestPeakY = peaksY[1];

    double subsubLargestPeakY = 0;
    double subsubLargestPeakX = 0;
    if(subLargestPeakX < largestPeakX)
    {
        subsubLargestPeakY = peaksY[2];
        subsubLargestPeakX = peaksY[2];
    }

    if(subsubLargestPeakX == 0)
    {
        double gain = 0;
        gain = subLargestPeakX - largestPeakX;
        fGainGuess = gain;
    }
    else
    {
        double gain1 = abs(subLargestPeakX - largestPeakX);
        double gain2 = abs(subsubLargestPeakX - subLargestPeakX);
        if(abs(gain1 - gain2) < 15)
        {
            fGainGuess = (gain1 + gain2) / 2.0;
        }
        else if(abs(gain1 / gain2 - 0.5) < 0.1)
        {
            fGainGuess = (gain2 / 2.0 + gain1) / 2.0;
        }
        else if(abs(gain2 / gain1 - 0.5) < 0.1)
        {
            fGainGuess = (gain1 / 2.0 + gain2) / 2.0;
        }
    }
    */


    iter ++;
    double peak1 = iter -> first;
    // iter ++;
    // double peak2 = iter -> first;
    // iter ++;
    // double peak3 = iter -> first;
    // fGainGuess = (peak2 + peak3) / 4- (peak0 + peak1) / 4;
    fGainGuess = (peak1 - peak0);
    cout << "Guess gain in estimation: " << fGainGuess << endl;
    return fGainGuess;


}

FitResult FitSpectrum::GetGain() const
{
    if(!fFitFlag)
    {
        return {-1,-1};
    }

    vector<double> vecPeak;
    for(int i = 0; i < fNPeak; i++)
    {
        double peak = fFunction -> GetParameter(3 * i + 1);
        vecPeak . push_back(peak);
    }

    vector<double> vecVari;
    for(int i = 0; i < fNPeak - 1; i++)
    {
        vecVari . push_back(vecPeak[i + 1] - vecPeak[i]);
    }
    double sum = 0;
    for(int i = 0; i < fNPeak - 1; i++)
    {
        sum += vecVari[i];

#ifdef VERBOSE
        cout << i << "\t" << "Gain: " << vecVari[i] << endl;
#endif
    }
    double gain = sum / (fNPeak - 1);
    double peak0 = vecPeak[0] - gain;
    
#ifdef VERBOSE
    cout << "Gain: " << gain << endl;
#endif

    FitResult result{gain, peak0};
    return result;
}


#include "FitSiPMSPE.h"
using namespace std;
#define VERBOSE

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

double MultiGauss::operator()(double *x, double *par) const
{
    double y = 0;
    for (int i = 0; i < fNPeak; i++)
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

    fHGauss->SetDirectory(0);
    fHAdd->SetDirectory(0);
    fSpectrum = new TSpectrum();
}

bool FitSpectrum::Clear()
{
    fNPeak = 0;
    if (fHistFlag)
    {
        // fHBackGround -> Delete();
        fHBackGround = NULL;
        fHistFlag = false;
    }
    if (fFitFlag)
    {
        delete fFunction;
        fFunction = NULL;

        fFitFlag = false;
    }
    fFitSuccess = false;
    return true;
}

FitSpectrum::~FitSpectrum()
{
    Clear();
    fSpectrum->Delete();
    fSpectrum = NULL;
    fHGauss->Delete();
    fHGauss = NULL;
    fHAdd->Delete();
    fHAdd = NULL;
}

bool FitSpectrum::SetHist(TH1 *h)
{
    if (!h)
    {
        cerr << "Error! Input invalid histogram." << endl;
        return false;
    }
    Clear();

    fHOrigin = h;
    fHBackGround = fSpectrum->Background(fHOrigin);
    fHGauss->Add(fHOrigin, fHBackGround, 1, -1);

    fHistFlag = 1;
    return true;
}

bool FitSpectrum::FitHist(UInt_t nGauss)
{
    if (!fHistFlag)
    {
        return false;
    }
    if (fFitFlag)
    {
        return true;
    }
    fNPeak = nGauss;

    fFitFlag = 1;

    UInt_t sNGauss = nGauss;
    MultiGauss sMultiGauss(0);
    TF1 *sFitFunArray[nGauss];

    // Start to fit
    for (int peakIndex = 0; peakIndex < nGauss; peakIndex++)
    {
        // First to create a function with i peaks;
        int peakNumTemp = peakIndex + 1;
        sMultiGauss.SetNPeak(peakNumTemp);

        sFitFunArray[peakIndex] = new TF1(Form("FitFun%d", peakIndex), sMultiGauss, 0, 4100, 3 * peakNumTemp);
        for (int i = 0; i < 3 * peakNumTemp; i++)
        {
            // Set limits of fit function;
            sFitFunArray[peakIndex]->SetParLimits(i, 0, 20000); // Set all parameters larger than 0
        }

        // Judge if this time is the first time to fit. If yes, initialize function
        if (peakIndex == 0)
        {
            // Set first Peak limits
            sFitFunArray[peakIndex]->SetParLimits(1, fFirstPeakMeanStartLimit, fFirstPeakMeanEndLimit);
            sFitFunArray[peakIndex]->SetParameter(0, 60);
            sFitFunArray[peakIndex]->SetParameter(1, fFirstPeakFitGuess);
            sFitFunArray[peakIndex]->SetParameter(2, 0.05 * fGainGuess);

            // Start position should be given and fixed in further fit
#ifdef VERBOSE
            cout << "First peak fit Range: " << fFirstPeakStartFitPoint << "~" << fFirstPeakMeanEndLimit << endl;
#endif
            fHGauss->Fit(sFitFunArray[peakIndex], "Q", "", fFirstPeakStartFitPoint, fFirstPeakMeanEndLimit);
        }
        else
        {
            // First, get the mean position of last peak
            double sLastPeakMean = sFitFunArray[peakIndex - 1]->GetParameter(3 * (peakIndex - 1) + 1);
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
            sFitFunArray[peakIndex]->SetParameter(3 * peakIndex + 1, sLastPeakMean + fGainGuess);
            sFitFunArray[peakIndex]->SetParLimits(3 * peakIndex + 1, sLastPeakMean, sLastPeakMean + 2 * fGainGuess);
            sFitFunArray[peakIndex]->SetParameter(3 * peakIndex + 2, fFirstPeakSigma);
            sFitFunArray[peakIndex]->SetParLimits(3 * peakIndex + 2, 0.001 * fGainGuess, 0.2 * fGainGuess);

            fHGauss->Fit(sFitFunArray[peakIndex], "Q", "", start, end);

            if (peakIndex > 0)
                ;
            auto tFitFun = sFitFunArray[peakIndex];
            int peakx1 = tFitFun->GetParameter(3 * peakIndex - 2);
            int peakx2 = tFitFun->GetParameter(3 * peakIndex + 1);
            int gaintest = peakx1 - peakx2;
            if (TMath::Abs(TMath::Abs(peakx1 - peakx2) - fGainGuess) > 0.2 * fGainGuess)
            {
#ifdef VERBOSE
                fHOrigin->SaveAs("ErrorOrigin.root");
                fHGauss->SaveAs("ErrorGauss.root");

                cout << "Peak1: " << peakx1 << endl;
                cout << "Peak2: " << peakx2 << endl;
                cout << "Sigma Guess: " << fFirstPeakSigma << endl;
                cout << "Sigma: " << tFitFun->GetParameter(3 * peakIndex + 2) << endl;
                cout << "Fit gain: " << TMath::Abs(peakx1 - peakx2) << endl;
                cout << "Guess gain: " << fGainGuess << endl;
#endif
                fFitSuccess = false;
                return false;
            }
        }
    }

    // Add fit function to background to get the final fit result.
    fFunction = sFitFunArray[sNGauss - 1];
    fHAdd->Reset();
    fHAdd->Add(fHBackGround);
    fHAdd->Add(fFunction);

    for (int i = 0; i < sNGauss - 1; i++)
    {
        sFitFunArray[i]->Delete();
    }

    fFitSuccess = true;

    return true;
}

bool FitSpectrum::Fit(TH1 *h, UInt_t nGauss)
{
    auto a = SetHist(h);
#ifdef VERBOSE
    cout << "Histogram has set." << endl;
#endif
    EstimateGain();
#ifdef VERBOSE
    cout << "Estimation done." << endl;
#endif

    auto b = FitHist(nGauss);
#ifdef VERBOSE
    if (b)
    {
        cout << "Fit success." << endl;
    }
    else
    {
        cout << "Fit fail." << endl;
    }

#endif

    return a && b; // && FitHist(nGauss);
}

double FitSpectrum::EstimateGain()
{
    if (!fHistFlag)
    {
        cerr << "Error, Histogram not set" << endl;
        return -1;
    }

    fSpectrum->Search(fHOrigin, 5, "", 0.001);

    int peaks = fSpectrum->GetNPeaks();
    auto peaksX = fSpectrum->GetPositionX();
    auto peaksY = fSpectrum->GetPositionY();

    if (peaks < 2)
    {
        // delete []peaksX;
        // delete []peaksY;
        fSpectrum->Search(fHOrigin, 1, "", 0.01);
        peaks = fSpectrum->GetNPeaks();
        peaksX = fSpectrum->GetPositionX();
        peaksY = fSpectrum->GetPositionY();
    }

    // Estimate gain
    // Put all points into a map, sort all peaks Y
    map<double, double> Points;
    map<double, double> mapPeakX;

    double maxPeakX = peaksX[0];
    double maxPeakY = peaksY[0];
#ifdef VERBOSE
    cout << "MaxPeak X: " << maxPeakX << '\t' << "MaxPeak Y: " << maxPeakY << endl;
#endif

    for (int i = 0; i < peaks; i++)
    {
#ifdef VERBOSE
        cout << "Peak " << i << '\t' << peaksX[i] << '\t' << peaksY[i] << endl;
#endif
        if (peaksY[i] > 0.03 * maxPeakY) // Filter some small peaks;
            mapPeakX.insert(pair<double, double>(peaksX[i], peaksY[i]));
    }

    // Getkl Peak position info
    map<double, double>::iterator iter;

    // It seems none sense here

    for (iter = mapPeakX.begin(); iter->first < 150 && iter != mapPeakX.end(); iter++)
        ; // if peak0 is at position smaller than 150, continue

    if (iter == mapPeakX.end())
    {
        iter = mapPeakX.begin();
        for (iter = mapPeakX.begin(); iter->first < 100 && iter != mapPeakX.end(); iter++)
            ;
    }

    if (iter == mapPeakX.end())
    {
        cout << "Estimation failed." << endl;
        fGainGuess = -1;
        return fGainGuess;
    }

    auto peak0 = iter->first;

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

    iter++;
    double peak1 = iter->first;
    // iter ++;
    // double peak2 = iter -> first;
    // iter ++;
    // double peak3 = iter -> first;
    // fGainGuess = (peak2 + peak3) / 4- (peak0 + peak1) / 4;
    fGainGuess = (peak1 - peak0);

#ifdef VERBOSE
    cout << "----------------" << endl;
    cout << "Gain guess, peak from TSpectrum." << endl;
    cout << "Peak0: " << peak0 << endl;
    cout << "Peak1: " << peak1 << endl;
#endif

    fFirstPeakFitGuess = peak0;
    fFirstPeakStartFitPoint = fFirstPeakMeanStartLimit = peak0 - 0.5 * fGainGuess;
    fFirstPeakMeanEndLimit = peak0 + 0.5 * fGainGuess;
    fFirstPeakSigma = 0.2 * fGainGuess;

#ifdef VERBOSE
    cout << "First Peak guess: " << fFirstPeakFitGuess << endl;
    cout << "First Peak start fit point: " << fFirstPeakStartFitPoint << endl;
    cout << "First Peak mean end: " << fFirstPeakMeanEndLimit << endl;
    cout << "First Peak sigma: " << fFirstPeakSigma << endl;
    cout << "Guess gain in estimation: " << fGainGuess << endl;
    cout << "---------------" << endl;
#endif
    return fGainGuess;
}

FitResult FitSpectrum::GetGain() const
{
    if (!fFitFlag)
    {
#ifdef VERBOSE
        cout << "Error! Has not fit yet, cannot get gain." << endl;
#endif
        return {-1, -1};
    }

    if (!fFitSuccess)
    {
#ifdef VERBOSE
        cout << "Error! Fit failed, cannot get gain." << endl;
#endif
        return {-1, -1};
    }

    vector<double> vecPeak;
    for (int i = 0; i < fNPeak; i++)
    {
        double peak = fFunction->GetParameter(3 * i + 1);
        vecPeak.push_back(peak);
    }

    vector<double> vecVari;
    for (int i = 0; i < fNPeak - 1; i++)
    {
        vecVari.push_back(vecPeak[i + 1] - vecPeak[i]);
    }
    double sum = 0;
    for (int i = 0; i < fNPeak - 1; i++)
    {
        sum += vecVari[i];

#ifdef VERBOSE
        cout << i << "\t"
             << "Gain: " << vecVari[i] << endl;
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

void FitSpectrum::Save(string filename)
{
    if (filename == "")
    {
        filename = "FitSiPMAutoSave.root";
    }

    if (fHistFlag)
    {
        cout << filename << endl;
        TFile f(filename.c_str(), "recreate");
        f.cd();
        fHOrigin->Write();
        if (fFitSuccess)
        {
            fHBackGround->Write();
            fFunction->Write();
        }
        f.Close();
    }
}

FitResult ReadSpectra(string sFile, string sHist, string fileNamePre, bool saveFlag)
{
    auto file = new TFile(sFile.c_str());
    auto hist = (TH1F *)file->Get(sHist.c_str());
    FitSpectrum sFitSpectrum(820);
    if (file->IsOpen() == false)
    {
        cout << "Not open" << endl;
        return {-1, -1};
    }
    bool fitResult = sFitSpectrum.Fit(hist, 3);
    if (saveFlag)
    {
        sFitSpectrum.Save(sHist + ".root");
    }

    if (!fitResult)
    {
        return {-1, -1};
    }

    auto result = sFitSpectrum.GetGain();
    // cout << "Gain guess: " << sFitSpectrum.GetGainGuess() << endl;

    auto c = new TCanvas("c", "c", 1);
    sFitSpectrum.GetGauss()->Draw();

    auto c2 = new TCanvas("c2", "c2", 1);
    sFitSpectrum.GetOrigin()->Draw();
    sFitSpectrum.GetSum()->Draw("same");

    if (fileNamePre == "")
    {
        c->SaveAs(Form("%s.pdf", sHist.c_str()));
        c2->SaveAs(Form("Origin%s.pdf", sFile.c_str()));
    }
    else
    {
        c->SaveAs(Form("%s.pdf", fileNamePre.c_str()));
        c2->SaveAs(Form("Origin%s.pdf", fileNamePre.c_str()));
    }

    delete c;
    delete c2;

    file->Close();
    file->Delete();
    return result;
}

FitResult ReadSpectra(string sFile, string sHist, int PeakNum) // PeakNum means how many peaks should be fit
{
    auto file = new TFile(sFile.c_str());
    auto hist = (TH1F *)file->Get(sHist.c_str());
    FitSpectrum sFitSpectrum(820);
    if (file->IsOpen() == false)
    {
        cout << "Not open" << endl;
        return {-1, -1};
    }
    bool fitResult = sFitSpectrum.Fit(hist, PeakNum);

    if (!fitResult)
    {
        return {-1, -1};
    }

    auto result = sFitSpectrum.GetGain();
    // cout << "Gain guess: " << sFitSpectrum.GetGainGuess() << endl;

    auto c = new TCanvas("c", "c", 1);
    sFitSpectrum.GetGauss()->Draw();

    auto c2 = new TCanvas("c2", "c2", 1);
    sFitSpectrum.GetOrigin()->Draw();
    sFitSpectrum.GetSum()->Draw("same");

    {
        c->SaveAs(Form("%s.pdf", sHist.c_str()));
        c2->SaveAs(Form("Origin%s.pdf", sFile.c_str()));
    }

    delete c;
    delete c2;

    file->Close();
    file->Delete();
    return result;
}

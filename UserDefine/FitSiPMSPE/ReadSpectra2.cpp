#include "FitSiPMSPE.h"

#include <iostream>
#include "TFile.h"

int main(int argc, char **argv)
{
    using namespace std;
    if (argc < 3)
    {
        cout << "Wrong argument number." << endl;
        cout << "Please input \" ./test2 ROOTFileName HistogramName\" " << endl;
        return 0;
    }


    string sRootFileName(argv[1]);
    string sHistName(argv[2]);

    FitResult result;

    if(argc == 3)
        result = ReadSpectra(sRootFileName.c_str(), sHistName.c_str());
    if(argc == 4)
    {
        try
        {
            string sPeaknum(argv[3]);
            int peaknum = stoi(sPeaknum);
            result = ReadSpectra(sRootFileName.c_str(), sHistName.c_str(), peaknum);
        }
        catch(const std::exception& e)
        {
            cout << "Wrong 4th argument" << endl;
            std::cerr << e.what() << '\n';
            return -1;
        }
        
    }
    cout << "Result: "
         << "FileName:" << sRootFileName << "HistoName:" << sHistName << endl
         << "Gain: " << result.gain << endl
         << "Peak: " << result.peak0 << endl;
    return 0;
}
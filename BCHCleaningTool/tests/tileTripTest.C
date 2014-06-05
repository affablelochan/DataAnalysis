//g++ -Wall -g -Wall -o tileTripTest.exe tileTripTest.C -I./home/cate/Work/BCHTool/TileTripReader/TileTripReader `root-config --cflags --glibs` -lMathCore -L./home/cate/Work/BCHTool/TileTripReader/StandAlone/libTileTripReader.so

#include <stdio.h>
#include <stdlib.h>
#include "TMathBase.h"
#include <iostream>
#include "TMath.h"
#include "TFile.h"
#include <vector>
#define ROOTCORE
// #include "/home/cate/Work/BCHTool/TileTripReader/TileTripReader/TTileTripReader.h"
#include "TileTripReader/TTileTripReader.h"

using namespace std;

int main(int argc, char* argv[])
{
  
    //your code  
    Root::TTileTripReader reader;
    reader.setTripFile("TileTripReader/data/CompleteTripList_2011-2012.root");
    reader.setVerbosity(1);
    
    cout<<"Check area next to LBA05 = "<<reader.checkEtaPhi(211787,100,0.05,0.35)<<endl;
    
    reader.m_LBOffsets.eta1=0.1;
    reader.m_LBOffsets.eta2=0.1;
    reader.m_LBOffsets.phi1=TMath::Pi()/32.;
    reader.m_LBOffsets.phi2=TMath::Pi()/32.;
    
    cout<<"Check area 2 next to LBA05 = "<<reader.checkEtaPhi(211787,100,0.05,0.35)<<endl;
    
//     //prepare a TileTripReader instance
//     Root::TTileTripReader *ttr=new Root::TTileTripReader("thetripreader");
//     ttr->setTripFile("TileTripReader/data/CompleteTripList_2011-2012.root");
//     ttr->setVerbosity(1);
//     
//     bool isInDeadRegion = ttr->checkEtaPhi(211787,100,0.05,0.35);
//     cout << "In dead region? " << isInDeadRegion << endl;
//         
//     ttr->m_LBOffsets.eta1=0.1;
//     ttr->m_LBOffsets.eta2=0.1;
//     ttr->m_LBOffsets.phi1=TMath::Pi()/32.;
//     ttr->m_LBOffsets.phi2=TMath::Pi()/32.;
//         
//     isInDeadRegion = ttr->checkEtaPhi(211787,100,0.05,0.35);
//     cout << "In dead region? " << isInDeadRegion << endl;
// 
//     return 0;
}

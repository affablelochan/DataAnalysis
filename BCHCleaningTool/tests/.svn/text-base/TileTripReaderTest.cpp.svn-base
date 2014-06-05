#include <fstream>
#include <iostream>

#include <TChain.h>
#include <TMath.h>

#define ROOTCORE
#include "/home/cate/Work/BCHTool/TileTripReader/TileTripReader/TTileTripReader.h"

using namespace std;

void TileTripReaderTest(const char* filename){
    TChain chain("caloD3PD");
    chain.Add(filename);
    chain.SetBranchStatus("*",0);
    chain.SetBranchStatus("tileFlags",1);
    chain.SetBranchStatus("tileError",1);
    chain.SetBranchStatus("RunNumber",1);
    chain.SetBranchStatus("EventNumber",1);
    chain.SetBranchStatus("lbn",1);
    UInt_t tileFlags=0,tileError=0,RunNumber=0, EventNumber=0, lbn=0;
    chain.SetBranchAddress("tileFlags",&tileFlags);
    chain.SetBranchAddress("tileError",&tileError);
    chain.SetBranchAddress("RunNumber",&RunNumber);
    chain.SetBranchAddress("EventNumber",&EventNumber);
    chain.SetBranchAddress("lbn",&lbn);
    
    ofstream readerOut("readerOut.txt");
    
    Root::TTileTripReader reader;
    reader.setTripFile("TileTripReader/data/CompleteTripList_2011-2012.root");
    reader.setVerbosity(1,readerOut);
    
    
    UInt_t event=0;
    while(chain.GetEntry(event)){
        reader.readTileFlags(RunNumber,lbn,tileError,tileFlags);
        reader.checkEtaPhi(RunNumber,lbn,0,0);
        ++event;
    }
    
}

void TripReaderTest(){
    
    Root::TTileTripReader reader;
    reader.setTripFile("TileTripReader/data/CompleteTripList_2011-2012.root");
    reader.setVerbosity(1);
    
    cout<<"Check area next to LBA05 = "<<reader.checkEtaPhi(211787,100,0.05,0.35)<<endl;
    
    reader.m_LBOffsets.eta1=0.1;
    reader.m_LBOffsets.eta2=0.1;
    reader.m_LBOffsets.phi1=TMath::Pi()/32.;
    reader.m_LBOffsets.phi2=TMath::Pi()/32.;
    
    cout<<"Check area 2 next to LBA05 = "<<reader.checkEtaPhi(211787,100,0.05,0.35)<<endl;
    
    
}
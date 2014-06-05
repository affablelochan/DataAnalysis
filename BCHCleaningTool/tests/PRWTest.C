//g++ -Wall -g -Wall -o PRWTest.exe PRWTest.C -DNO_ATHENA_BUILD -DJETEDM_STANDALONE -DROOTCORE -I./RootCore/include/ `root-config --cflags --glibs` -lMathCore -L./RootCore/lib/ -lPATCore -lTileTripReader -lJetAnalysisEDM -lPileupReweighting -lBCHCleaningTool -lPileupReweighting -lJetAnalysisEDM -lTileTripReader -lPATCore -lGenVector
#include <stdio.h>
#include <stdlib.h>
#include "BCHCleaningTool/BCHCleaningToolRoot.h"
#include "TMathBase.h"
#include <iostream>
#include "TMath.h"
#include "TFile.h"
#include <vector>

using namespace std;

int main(int argc, char* argv[])
{
  
    printf("Before tool\n");
    BCHTool::BCHCleaningToolRoot thebchtool("thebchtool");
    thebchtool.SetResponseVsBCHFile("BCHCleaningTool/share/BCH_Response.root");

    ///TileTripReader initialisation and tests outside BCH tool
    
    //prepare a TileTripReader instance
    Root::TTileTripReader *ttr=new Root::TTileTripReader("thetripreader");
    ttr->setTripFile("TileTripReader/data/CompleteTripList_2011-2012.root");
    ttr->setVerbosity(0);
    
    //here we ask the TTR directly to give us the answer of a jet that is on the edge
    //the edges haven't yet been changed, so the answer should be no.
    bool isInDeadRegionBeforeEdgeChange = ttr->checkEtaPhi(211787,25000,0.1,0.35);
    cout << "In dead region, before edge change? " << isInDeadRegionBeforeEdgeChange << endl;

    //Set the edges
    ttr->m_LBOffsets.eta1=0.1;
    ttr->m_LBOffsets.eta2=0.1;
    ttr->m_LBOffsets.phi1=TMath::Pi()/32.;
    ttr->m_LBOffsets.phi2=TMath::Pi()/32.;
    
    //here we ask the TTR directly to give us the answer of a jet that is on the edge
    //the edges have now been changed, so the answer should be yes.
    bool isInDeadRegionAfterEdgeChange = ttr->checkEtaPhi(211787,25000,0.1,0.35);
    //NOTE: THIS IS WHERE IT DOES NOT GIVE ME THE RIGHT ANSWER!!! it should be TRUE below
    cout << "In dead region, after edge change? " << isInDeadRegionAfterEdgeChange << endl;
    
    ///PRW initialisation and tests
    
    //prepare a PRW instance
    Root::TPileupReweighting *prw=new Root::TPileupReweighting("thepileupreweighting");
    //initialise it with a MC file, a data file and tell it what to do with unmatched data
    prw->AddConfigFile("RootFilesForTests/ExcitedQ_2000.prw.root");
    prw->AddLumiCalcFile("RootFilesForTests/ilumicalc_histograms_None_200841-204158.root"); 
    prw->SetUnrepresentedDataAction(2);
    prw->Initialize();
    
    //initialize the BCH tool (for MC at the moment)
    //this will change the edges for the offsets of the TTR
    thebchtool.InitializeTool(false, ttr, prw);
    
    //here we ask the TTR directly to give us the answer of a jet that is on the edge
    //the edges haven't yet been changed, so the answer should be no.
    isInDeadRegionAfterEdgeChange = ttr->checkEtaPhi(211787,25000,0.1,0.35);
    cout << "In dead region, after edge change from BCH tool? " << isInDeadRegionAfterEdgeChange << endl;

    ///Stress test 
    TFile* stressfulFile = TFile::Open("RootFilesForTests/stress_test.root");
    TTree* stressfulTree = (TTree*) stressfulFile->Get("physics");
   
    ///Some random q* MC 
    //TODO add a different sample here
    
    std::vector<float> * v_jets_pt=0;
    std::vector<float> * v_jets_eta=0;
    std::vector<float> * v_jets_phi=0;
    stressfulTree->SetBranchAddress("jet_AntiKt4LCTopo_pt", &v_jets_pt);
    stressfulTree->SetBranchAddress("jet_AntiKt4LCTopo_eta", &v_jets_eta);
    stressfulTree->SetBranchAddress("jet_AntiKt4LCTopo_phi", &v_jets_phi);

    for (Int_t i=0;i<stressfulTree->GetEntries();i++) {
      stressfulTree->GetEntry(i);
      for (UInt_t ijet=0; ijet<v_jets_pt->size(); ijet++) {
        
        cout <<  "Jet pt:" << v_jets_pt->at(ijet) << endl;
        cout <<  "Jet eta:" << v_jets_eta->at(ijet) << endl;
        cout <<  "Jet phi:" << v_jets_phi->at(ijet) << endl;

        ///testing the basic functionality
        
        //note: need to pass the random number from MC to the BCHCleaningTool!! One can decide at this point whether to use mu or not
        //Here I've passed the RunNumber from a MC sample
        double mu = 20;
        int randomRunNumber = prw->GetRandomRunNumber(195847, mu);
        std::cout << "Random run number: " << randomRunNumber << std::endl;
        bool isInMaskedRegionBCH = thebchtool.IsInMaskedRegion(195847, v_jets_pt->at(ijet),v_jets_eta->at(ijet),v_jets_phi->at(ijet),mu);
        cout << "In dead region from BCHCleaningTool? " << isInMaskedRegionBCH  << endl;
        //testing straight from the TTR
        ttr->setVerbosity(0);
        bool isInMaskedRegionTTR = ttr->checkEtaPhi(randomRunNumber,v_jets_pt->at(ijet),v_jets_eta->at(ijet),v_jets_phi->at(ijet));
        cout << "In dead region from TTR? " << isInMaskedRegionTTR << endl;
        
        ///testing the response estimate
        cout << "Jet pT after response correction:" << thebchtool.EstimateJetResponseNoBCH(v_jets_pt->at(ijet), 0.3) << endl;        
        
      }
    }
    
    printf("After tool\n");
    return 0;
}

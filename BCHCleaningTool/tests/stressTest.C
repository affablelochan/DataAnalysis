//g++ -Wall -g -Wall -o stressTest.exe stressTest.C -DNO_ATHENA_BUILD -DJETEDM_STANDALONE -I./RootCore/include/ `root-config --cflags --glibs` -lMathCore -L./RootCore/lib/ -lPATCore -lTileTripReader -lJetAnalysisEDM -lPileupReweighting -lBCHCleaningTool -lPileupReweighting -lJetAnalysisEDM -lTileTripReader -lPATCore -lGenVector

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

    thebchtool.SetSeed(14001450);
    
    //prepare a TileTripReader instance
    Root::TTileTripReader *ttr=new Root::TTileTripReader("thetripreader");
    ttr->setTripFile("TileTripReader/data/CompleteTripList_2011-2012.root");
    ttr->setVerbosity(1);

    //initialize the BCH tool (for MC at the moment)
    thebchtool.InitializeTool(false, ttr, "BCHCleaningTool/share/FractionsRejectedJetsMC.root");

    TFile* stressfulFile = TFile::Open("Tests/stress_test.root");
    TTree* stressfulTree = (TTree*) stressfulFile->Get("physics");
    
    std::vector<float> * v_jets_pt=0;
    std::vector<float> * v_jets_eta=0;
    std::vector<float> * v_jets_phi=0;
    stressfulTree->SetBranchAddress("jet_AntiKt4LCTopo_pt", &v_jets_pt);
    stressfulTree->SetBranchAddress("jet_AntiKt4LCTopo_eta", &v_jets_eta);
    stressfulTree->SetBranchAddress("jet_AntiKt4LCTopo_phi", &v_jets_phi);
    
    for (Int_t i=0;i<stressfulTree->GetEntries();i++) {
      stressfulTree->GetEntry(i);
      for (UInt_t ijet=0; ijet<v_jets_pt->size(); ijet++) {
        
        cout <<  "pt:" << v_jets_pt->at(ijet) << endl;
        cout <<  "eta:" << v_jets_eta->at(ijet) << endl;
        cout <<  "phi:" << v_jets_phi->at(ijet) << endl;
        
        //place the jet in a bad region (forget stress test for now...)        
        bool isInDeadRegion = thebchtool.IsInMaskedRegion(211787,100,0.05,0.35);
        cout << "In masked region? " << isInDeadRegion << endl;

	//mock BCH_CORR_CELL and fEM so that they're bad
        // Cut 1: (BCH,emfrac) = (0.2,0) --> (0.1,1)
        // Cut 2: (BCH,emfrac) = (0.5,0.2) --> (0,0.4)
	double BCH_CORR_CELL = 0.2;
	double fEM = 0.45;
	
	//no stress test
        bool isCut = thebchtool.IsBadMediumBCH(211787,100,0.05,0.35,
					       BCH_CORR_CELL, fEM, v_jets_pt->at(ijet),-1);
	//in case of stress test
//         bool isCut = thebchtool.IsBadMediumBCH(211787,100,v_jets_eta->at(ijet),v_jets_phi->at(ijet),
// 					       BCH_CORR_CELL, fEM, v_jets_pt->at(ijet),-1);
	
        cout << "Run #" << i <<" Jet #" << ijet << " cut? " << isCut << endl;
	
      }
    }
    
    printf("After tool\n");
    return 0;
}

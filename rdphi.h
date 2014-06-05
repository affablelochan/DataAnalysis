//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Dec 16 23:11:31 2011 by ROOT version 5.28/00e
// from TTree HWWTree_e/HWW SemilepNtuple_e
// found on file: HWWSkimmedNTUP.root
//////////////////////////////////////////////////////////

#ifndef rdphi_h
#define rdphi_h

#include <iostream>
#include <vector>
#include <TRandom3.h>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TSelector.h>
#include <TString.h>
#include "TLorentzVector.h"
#include "TMath.h"
#include <ostream>//caw
#include <cmath>
#include <stdlib.h>
#include <map>
#include <fstream>
#include <set>
#include <unistd.h> 
#include <iomanip>
#include "TVector3.h"
#include "TVector2.h"
#include <sstream>
#include <fstream>
#include <exception>
#include <TMatrixDSym.h>
#include <TMatrixDSymEigen.h>
#include <TVectorD.h>
#include <TAxis.h>
#include "TSystem.h" 

#include "JES/JESUncertaintyProvider.h"
#include "JES/MultijetJESUncertaintyProvider.h"


using namespace std;

#define Ok_el_mass 0.510998 
#define Ok_mu_mass 105.658
#define PI 3.14159



class rdphi : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //!current Tree number in a TChain

   int                         NevtAll , rejected_events;

   
   long count;
   
   double lepton_mass;

   TString ident;
   TString input_path;
   TString output_path;
   bool do_50_50;
   bool left_50;
   bool do_mc;
   bool do_muon;


  TRandom3 random_50_50;
  Double_t cut[100];

  double ht_final;

  

   bool verbose;
   bool do_jes;
   TString jesvar;
   int  jescomp;
   bool do_npv;

   MultijetJESUncertaintyProvider *myJES;
   
   //JERProvider *myJER;


   // Declaration of leaf types
  vector<float>   *jet_pt;
   vector<float>   *jet_e;
   vector<float>   *jet_eta;
   vector<float>   *jet_raw_eta;
   vector<float>   *jet_phi;
   vector<float>   *jet_y;
   vector<int>     *jet_Badness;
   vector<int>     *jet_isUgly;
   vector<float>   *jet_jvtxf;
   vector<float>   *jet_origin;
   Int_t           lbn;
   Int_t           RunNumber;
   Int_t           EventNumber;
   Int_t           n_pvtx;
   Int_t           good_pvtx;
   Float_t         mu;
   vector<string>  *triggers;
   vector<string>  *triggers_emulated;
 
   //#ifdef do_mc
     Float_t         event_weight;
     //#endif
     vector<float>   *truth_jet_pt;
      vector<float>   *truth_jet_eta;
      vector<float>   *truth_jet_phi;
      vector<float>   *truth_jet_e;
     vector<float>   *truth_jet_y;
      Int_t           mc_channel_number;
      //       Float_t         event_weight;
   
  
     //#endif
   TBranch        *b_truth_jet_pt;   //!
   TBranch        *b_truth_jet_eta;   //!
   TBranch        *b_truth_jet_phi;   //!
   TBranch        *b_truth_jet_e;   //!
   TBranch        *b_truth_jet_y;   //!
   TBranch        *b_mc_channel_number;   //!
   TBranch        *b_event_weight;   //!

   //#ifdef do_mc
    //#if (do_mc)
      //   {
 

   //     }
   //#endif
 
   //#endif

   TBranch        *b_jet_pt;   //!
   TBranch        *b_jet_e;   //!
   TBranch        *b_jet_eta;   //!
   TBranch        *b_jet_raw_eta;   //!
   TBranch        *b_jet_phi;   //!
   TBranch        *b_jet_y;   //!
   TBranch        *b_jet_Badness;   //!
   TBranch        *b_jet_isUgly;   //!
   TBranch        *b_jet_jvtxf;   //!
   TBranch        *b_jet_origin;   //!
   TBranch        *b_lbn;   //!
   TBranch        *b_RunNumber;   //!
   TBranch        *b_EventNumber;   //!
   TBranch        *b_n_pvtx;   //!
   TBranch        *b_good_pvtx;   //!
   TBranch        *b_mu;   //!
   TBranch        *b_triggers;   //!
   TBranch        *b_triggers_emulated;   //!
   /*
   TBranch        *b_truth_jet_pt;   //!
   TBranch        *b_truth_jet_eta;   //!
   TBranch        *b_truth_jet_phi;   //!
   TBranch        *b_truth_jet_e;   //!
   TBranch        *b_truth_jet_y;   //!
   TBranch        *b_mc_channel_number;   //!
   TBranch        *b_event_weight;   //!

   */


   
   ///////////////////////////////////
   //  Histograms
   ///////////////////////////////////
 
 
 
    TH1D *HT;

   const static int NHistos1D = 15;
   TH1D* HistosHT[15];

   TH1F *event_count;

   rdphi(TTree * /*tree*/ =0) { }
   virtual ~rdphi() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();
   
   virtual bool    string_true_false(TString tf);
   virtual void    ParseOptions(TString p_option);
   virtual void    print_usage();

   ClassDef(rdphi,0);
};

#endif

#ifdef rdphi_cxx
void rdphi::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).



  // Set object pointer
   jet_pt = 0;
   jet_e = 0;
   jet_eta = 0;
   jet_raw_eta = 0;
   jet_phi = 0;
   jet_y = 0;
   jet_Badness = 0;
   jet_isUgly = 0;
   jet_jvtxf = 0;
   jet_origin = 0;
   triggers = 0;
   triggers_emulated = 0;



   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

 fChain->SetBranchAddress("jet_pt", &jet_pt, &b_jet_pt);
   fChain->SetBranchAddress("jet_e", &jet_e, &b_jet_e);
   fChain->SetBranchAddress("jet_eta", &jet_eta, &b_jet_eta);
   fChain->SetBranchAddress("jet_raw_eta", &jet_raw_eta, &b_jet_raw_eta);
   fChain->SetBranchAddress("jet_phi", &jet_phi, &b_jet_phi);
   fChain->SetBranchAddress("jet_y", &jet_y, &b_jet_y);
   fChain->SetBranchAddress("jet_Badness", &jet_Badness, &b_jet_Badness);
   fChain->SetBranchAddress("jet_isUgly", &jet_isUgly, &b_jet_isUgly);
   fChain->SetBranchAddress("jet_jvtxf", &jet_jvtxf, &b_jet_jvtxf);
   fChain->SetBranchAddress("jet_origin", &jet_origin, &b_jet_origin);
   fChain->SetBranchAddress("lbn", &lbn, &b_lbn);
   fChain->SetBranchAddress("RunNumber", &RunNumber, &b_RunNumber);
   fChain->SetBranchAddress("EventNumber", &EventNumber, &b_EventNumber);
   fChain->SetBranchAddress("n_pvtx", &n_pvtx, &b_n_pvtx);
   fChain->SetBranchAddress("good_pvtx", &good_pvtx, &b_good_pvtx);
   fChain->SetBranchAddress("mu", &mu, &b_mu);
   fChain->SetBranchAddress("triggers", &triggers, &b_triggers);
   fChain->SetBranchAddress("triggers_emulated", &triggers_emulated, &b_triggers_emulated);


}

Bool_t rdphi::Notify()
{
  
   return kTRUE;
}

#endif // #ifdef rdphi_cxx

//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Oct 23 11:07:32 2013 by ROOT version 5.34/10
// from TTree qcd/qcd
// found on file: /hdfs/projects/atlas/qcd_d4pds/v4_A-L/pythia/user.tamsett.017173._00070.dpd-147913.root
//////////////////////////////////////////////////////////

#ifndef mcTree_h
#define mcTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <vector>
#include <vector>

// Fixed size dimensions of array or collections stored in the TTree if any.

class mcTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

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
   vector<float>   *truth_jet_pt;
   vector<float>   *truth_jet_eta;
   vector<float>   *truth_jet_phi;
   vector<float>   *truth_jet_e;
   vector<float>   *truth_jet_y;
   Int_t           mc_channel_number;
   Float_t         event_weight;
   vector<string>  *triggers_emulated;

   // List of branches
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
   TBranch        *b_truth_jet_pt;   //!
   TBranch        *b_truth_jet_eta;   //!
   TBranch        *b_truth_jet_phi;   //!
   TBranch        *b_truth_jet_e;   //!
   TBranch        *b_truth_jet_y;   //!
   TBranch        *b_mc_channel_number;   //!
   TBranch        *b_event_weight;   //!
   TBranch        *b_triggers_emulated;   //!

   mcTree(TTree *tree=0);
   virtual ~mcTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef mcTree_cxx
mcTree::mcTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/hdfs/projects/atlas/qcd_d4pds/v4_A-L/pythia/user.tamsett.017173._00070.dpd-147913.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/hdfs/projects/atlas/qcd_d4pds/v4_A-L/pythia/user.tamsett.017173._00070.dpd-147913.root");
      }
      f->GetObject("qcd",tree);

   }
   Init(tree);
}

mcTree::~mcTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t mcTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t mcTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void mcTree::Init(TTree *tree)
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
   truth_jet_pt = 0;
   truth_jet_eta = 0;
   truth_jet_phi = 0;
   truth_jet_e = 0;
   truth_jet_y = 0;
   triggers_emulated = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
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
   fChain->SetBranchAddress("truth_jet_pt", &truth_jet_pt, &b_truth_jet_pt);
   fChain->SetBranchAddress("truth_jet_eta", &truth_jet_eta, &b_truth_jet_eta);
   fChain->SetBranchAddress("truth_jet_phi", &truth_jet_phi, &b_truth_jet_phi);
   fChain->SetBranchAddress("truth_jet_e", &truth_jet_e, &b_truth_jet_e);
   fChain->SetBranchAddress("truth_jet_y", &truth_jet_y, &b_truth_jet_y);
   fChain->SetBranchAddress("mc_channel_number", &mc_channel_number, &b_mc_channel_number);
   fChain->SetBranchAddress("event_weight", &event_weight, &b_event_weight);
   fChain->SetBranchAddress("triggers_emulated", &triggers_emulated, &b_triggers_emulated);
   Notify();
}

Bool_t mcTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void mcTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t mcTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef mcTree_cxx

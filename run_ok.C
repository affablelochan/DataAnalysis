#include <iostream>
#include <fstream>

#include "TTree.h"
#include "TChain.h"
#include "TString.h"
#include "TROOT.h"
#include "TObjArray.h"
#include "TChainElement.h"
#include "TLeaf.h"
#include "TProof.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TEntryList.h"

using namespace std;

#include "PROOF-INF/SETUP.C";
#include "add_files.C";



// Usage: Root[0] .x run_ok.C(true,545...)
// Usage: Root[0].L read_ok.C;
//        Root[1]run_ok(build,tag,proof,workers...);
void run_ok(
            TString data_path="../data/el", //specify _path with No trailing slash "/"
            TString input_path="./input/", //specify input_path With trailing slash "/"
            TString ident="identifier.not.specified", // process identifier
            TString do_jes="false",                   // Calculate JES uncertainties
	    TString jesvar="",                        // Vary JES up or down
	    TString jescomp="0",                      // JES component, 0 = all
	    TString do_npv="false",                   // Do NPV offset correction?
            TString do_mc="false", // MC or Data
            TString do_muon="false", // else electron
            TString do_50_50="false", //
            TString left_50="false", //
            bool build = true,   //rebuild solution, else (make or proof)
            bool proof = false,   //run with proof
            bool proof_scream = false, //enable package and go, bypass packaging and loading, the process must have been packaged and loaded with a previous proof run using build
            bool dry_run = false, //do all except processing data 
            TString tree_name="qcd", // specify tree name 

            // location of output files relitive to input_path With trailing slash "/"
            TString output_path="../output/"//, (no comma on last line) 
            ) // don't delete this line 
              // line above does not end in comma
              // other lines above must end in comma!
{

  if ( proof ){
      cout << endl << "proof on :"; 
    if( build ){
         cout << "build on :"; 
      gROOT->ProcessLine(".!pwd");
      gROOT->ProcessLine(".!rm -f ./run_ok_lib");
      gROOT->ProcessLine(".!rm -f ./run_ok_lib.par");
      gROOT->ProcessLine(".!rm -f -r ./.proof");
      gROOT->ProcessLine(".!rm -f -r ./proof-package-lock-*");
      gROOT->ProcessLine(".!rm -f -r ../run_ok_lib");
      gROOT->ProcessLine(".!mkdir ../run_ok_lib");
      gROOT->ProcessLine(".!cp ../run/* ../run_ok_lib/");
//      gROOT->ProcessLine(".!rm ../run_ok_lib/*.so");
//      gROOT->ProcessLine(".!rm ../run_ok_lib/*.d");
      gROOT->ProcessLine(".!cp -r PROOF-INF ../run_ok_lib/");
      gROOT->ProcessLine(".!tar -czf run_ok_lib.par ../run_ok_lib/");
      proof_scream = false; 
    }
     cout << "\007" << endl ; 


  }

  if( !proof ){
    if( build ) gROOT->ProcessLine(".!source ./PROOF-INF/BUILD.sh");
    else        gROOT->ProcessLine(".!source ./PROOF-INF/MAKE.sh");
  }

  TString out_dir;
  out_dir = input_path + output_path + ident + "/";

  //make and/or check output path
  Int_t e_code;
  TString command_line;
  command_line = ".!mkdir -p " + out_dir ;
  cout << "command_line: " << command_line << endl;
  gROOT->ProcessLine( command_line, &e_code );
  if( e_code != 0 ){
    cout << ".!ls mkdir output path: return code: " << e_code ;
    exit(e_code);
  }
  command_line = ".!ls " + out_dir ;
  cout << "command_line: " << command_line << endl;
  e_code = gROOT->ProcessLine( command_line, &e_code );
// delete output dircory here if duplicate ntuples becomes a problem. 
  cout << endl << ".!ls out_dir return code: " << e_code << endl;
  if( e_code > 1 ) exit(e_code);

  if( !proof ) SETUP();

  try{

    if( tree_name == "" ) throw "tree name not specified";
    if( ident == "" ) throw "ident name not specified";

    build_file_list( out_dir + ident + ".list", data_path);

    TChain t1(tree_name); t1.SetDirectory(0);

    add_files( t1, out_dir + ident + ".list" );
    list_branches( t1,  out_dir + ident + ".branch_list.txt" );//optional

    gROOT->SetBatch(true);

    if( proof ){
      MyTString proof_option;
      proof_option = "lite";

      cout << endl << "new: "<< proof_option; cout.flush();
      TProof* p = TProof::Open(((TString)proof_option).Data());
      cout << endl << "after proof open: " << proof_option; cout.flush();

      if( !proof_scream )
        p->ClearCache(); //make sure it runs with latest changes!

      if( build ) p->ClearPackages();
      if( !proof_scream ) p->UploadPackage("run_ok_lib.par");

      p->EnablePackage("run_ok_lib");

      p->SetParameter("PROOF_UseTreeCache", (Int_t) 0 );
      p->SetParameter("PROOF_UseParallelUnzip", (Int_t) 1 );

      t1.SetProof();                    
    }


    TString options = "";
    if( options == "" ){
      options += 
        TString(" identifier=" + ident ) + // substitute ident with a name for this process
        TString(" input_path=" + input_path) + // location to input data
        TString(" output_path=" + output_path) + // location of result file
	TString(" do_jes=" + do_jes) +  // Do JES uncertainties
	TString(" jesvar=" + jesvar) + // If do_jes, up/down variation
	TString(" jescomp=" + jescomp) + // If do_jes, JES component (0 = all)
	TString(" do_npv=" + do_npv) + // Do NPV offset correciton 
        TString(" do_mc=" + do_mc) +    // MC or Data
        TString(" do_muon=" + do_muon) +  // else electron
        TString(" do_50_50=" + do_50_50) + //
        TString(" left_50="  + left_50) ;  //
    }

    cout << endl << "process: " << options << endl; cout.flush();

    if( !dry_run )
    {
      gSystem->Load("JES/libJetUncertainties.so");
      //gSystem->Load("JER/libJERProvider.so"); 
      t1.Process("rdphi.C+",options);
    }
    else 
    {
      cout << endl << "run_ok: dry_run=true, otherwise would have processed: " << t1.GetEntries() << ": entries" << endl;
    }

    cout << endl << "run_ok: Completed: Ok: "<< endl;
    exit(0); // exit ROOT here, if no exceptions
  }
  catch( char *s){
    cout << endl << "run_ok: ERROR: "<< s << endl;
    exit(1); //exit here with exceptions
  }
  
}

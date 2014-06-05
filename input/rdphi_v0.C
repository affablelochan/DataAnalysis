#define rdphi_cxx
// The class definition in rdphi.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived

#include "rdphi.h"
#include <TH2.h>
#include <TStyle.h>
#include <iostream>
using namespace std;




void rdphi::Begin(TTree * /*tree*/)
{
  TString option = GetOption();
//  for( int i = 0 ; i < 100 ; i++ ) cut[i] = 0;
  // please make sure to initialize all histograms to 0, otherwise there will be potential memory leak
 
  for(int i = 0; i < NHistos1D; i++) HistosHT[i] = 0;
  

//for(int i=0; i<16; i++) {             TH1D *htname[i];   htname[i] = 0; }


  HT = 0; 
  event_count = 0;
}

void rdphi::SlaveBegin(TTree * /*tree*/)
{
  TString option = GetOption();
  ParseOptions(option);


  fprintf(stdout, "\nLaTech: Slave begin : p2 \n");fflush(stdout);
  
  





    
  
  
  


  event_count = new TH1F("event_count","event_count", 10000, 200000, 220000)  ;
  event_count->SetDirectory(0);
  fOutput->Add((TObject *) event_count);
  event_count->GetXaxis()->SetTitle("2012 Run Numbers");
  event_count->GetYaxis()->SetTitle("Entries");


Float_t bins[] = { 450, 600, 750, 900, 1100, 1400, 1800, 2200, 2700, 4000 };
Int_t  binnum = sizeof(bins)/sizeof(Float_t) - 1; // or just = 9

  HT = new TH1D("HT","HT scalar sum of pT of all good jets", binnum, bins)  ;
  HT->SetDirectory(0);
  fOutput->Add((TObject *) HT);
  HT->GetXaxis()->SetTitle("HT (GeV)");
  HT->GetYaxis()->SetTitle("Entries");
 // HT->Sumw2();



   vector<string> htname;
   for ( int i = 1 ; i <= 15 ; i++ )
       {
           
            ostringstream convert; 
             string temp;
             convert << i;
             temp ="HT"+ convert.str();         cout << temp <<endl;
             htname.push_back(temp);
         }



 for(int i=0; i<NHistos1D; i++) {
            
    HistosHT[i] = new TH1D(htname[i].c_str(),"HT scalar sum of pT of all good jets", binnum, bins)  ;
    HistosHT[i]->SetDirectory(0);
    fOutput->Add((TObject *) HistosHT[i]);
    HistosHT[i]->GetYaxis()->SetTitle("Entries");
  }



  fprintf(stdout, "\nLaTech: Slave begin : p5 \n");fflush(stdout);

fprintf(stdout, "\nLaTech: Slave begin : p6 \n");fflush(stdout);


    //TAxis *ht_jet = 0;
 fprintf(stdout, "\nLaTech: Slave begin : p7 \n");fflush(stdout);

}

Bool_t rdphi::Process(Long64_t entry)
{
   NevtAll++;
   if (NevtAll % 20000 == 0)
      cout <<"Number of events processed " << NevtAll << std::endl;



   GetEntry(entry);
   event_count->Fill( RunNumber, 1);  //this histogram is to check if we ran over all the events in d4pd files 

   //triggers initiated and checked in this block
   bool J110_passed = false;
   bool J145_passed = false;
   bool J170_ht600_passed = false;
   bool J170_ht700_passed = false;
   bool trigger_passed = false;

   for( unsigned int i = 0 ; i < triggers->size(); i++ ) 
      {
		
			TString trig = triggers->at(i);
		
			if(trig == "EF_j110_a4tchad") J110_passed = true; 
			if(trig == "EF_j145_a4tchad") J145_passed = true; 
			if(trig == "EF_j170_a4tchad_ht600") J170_ht600_passed = true;
			if(trig == "EF_j170_a4tchad_ht700") J170_ht700_passed = true;
			
      }
      
   //trigger block ends here
   

   //event level phase space checks 
   bool reject_event = false;	
   double ystar= 100, yboost=100, ht=0 ;

   //check if ystar and yboost are within the phase space. else reject the event 
   if ( jet_pt->size() >=2 )
       {
           if (  jet_Badness->at(1) >=3  || jet_Badness->at(0) >=3      )  reject_event = true;
           if (  jet_pt->at(1)/1000. < 100. ) reject_event = true; 
           if (!reject_event)  { 

                                    ystar  = 0.5 *    fabs( jet_y->at(0) - jet_y->at(1) ); 
                                    yboost = 0.5 *        ( jet_y->at(0) + jet_y->at(1) ); 
                                   if ( ystar > 2) reject_event = true;
                                   if ( fabs(yboost) > 0.5) reject_event = true;                           
                                  }           

       }

    //calculate HT and check if pt1 > HT/3
    if(!reject_event && jet_pt->size() >=2) { 
               
                 for ( unsigned int i = 0 ; i < jet_pt->size() ; i++ ) 
                           { 
                            	   if (jet_pt->at(i) < 100000. ) continue;
	                           if (jet_Badness->at(i) >=3 ) continue;
                                   if (  fabs (jet_y->at(i) - yboost)  > 2    )  continue; 

                                 ht = ht + jet_pt->at(i)/1000. ; 
                           }

                     if (  (jet_pt->at(0)/1000) < (ht/3) )        reject_event = true;               

                     if (   (J110_passed && ht > 450 && ht < 600)  ||   
         (J145_passed && ht > 600 && ht < 750)  ||  
         (J170_ht600_passed && ht > 750 && ht < 900)  || 
          (J170_ht700_passed && ht > 900)   ) 
                  trigger_passed = true; 
 //if(!trigger_passed)  cout << "HT: "<<ht << " " << J110_passed << " " << J145_passed << " " << J170_ht600_passed << " " << J170_ht700_passed << endl ; 
                       } //end HT 




   if (reject_event || !trigger_passed)  rejected_events++;

   if (!reject_event  && jet_pt->size() >=2 && trigger_passed)
      {
       HT->Fill(ht);
 
       double dphi = TVector2::Phi_mpi_pi( jet_phi->at(0)-jet_phi->at(1) );
       double fdphi = fabs(dphi);


       if ( ystar >=0 && ystar <=0.5)
                  {   
                        HistosHT[0]->Fill(ht);   
                        if (fdphi <= 7*PI/8 )    HistosHT[1]->Fill(ht);                      
                        if (fdphi <= 5*PI/6 )    HistosHT[2]->Fill(ht);                      
                        if (fdphi <= 3*PI/4 )    HistosHT[3]->Fill(ht);                      
                        if (fdphi <= 2*PI/3 )    HistosHT[4]->Fill(ht);                      
                     }


       if ( ystar >0.5 && ystar <=1.0)
                  {   
                        HistosHT[5]->Fill(ht);   
                        if (fdphi <= 7*PI/8 )    HistosHT[6]->Fill(ht);                      
                        if (fdphi <= 5*PI/6 )    HistosHT[7]->Fill(ht);                      
                        if (fdphi <= 3*PI/4 )    HistosHT[8]->Fill(ht);                      
                        if (fdphi <= 2*PI/3 )    HistosHT[9]->Fill(ht); 
                     }


       if ( ystar >1.0 && ystar <=2.0)
                  {   
                          HistosHT[10]->Fill(ht);   
                        if (fdphi <= 7*PI/8 )    HistosHT[11]->Fill(ht);                      
                        if (fdphi <= 5*PI/6 )    HistosHT[12]->Fill(ht);                      
                        if (fdphi <= 3*PI/4 )    HistosHT[13]->Fill(ht);                      
                        if (fdphi <= 2*PI/3 )    HistosHT[14]->Fill(ht); 
          
                     }
      // cout << "the deltaphi= " << fabs(dphi) <<endl; 

}

   // Now the phase space check is done 
   // Left with delta phi calcuation and filling the histogram. 

//  if (reject_event) { rejected_events++; cout << "ystar: " << ystar << " yboost " << yboost << "pt " << jet_pt->at(1)/1000.  <<endl ;  }
//   for ( unsigned int i = 0 ; i < jet_pt->size() ; i++ ) 
  //    { 
    //    if (NevtAll % 20000 == 0)             cout << "jet no: " << i+1 << "pt: " << jet_pt->at(i)/1000. <<endl; 

      //}

                               



    

  return kTRUE;
}

void rdphi::SlaveTerminate()
{
  // The SlaveTerminate() function is called after all entries or objects
  // have been processed. When running with PROOF SlaveTerminate() is called
  // on each slave server.

}

void rdphi::Terminate()
{
cout << "total event: " << NevtAll << "rejected: " << rejected_events << endl; 

  TFile *f = new TFile("output.root","RECREATE");

  //Write output Histograms
  TList *tl = GetOutputList();
  int l = tl->GetEntries();
  for ( int i = 0 ; i < l ; i++ )
  {
    TObject *o = tl->At(i);

    if ( o->InheritsFrom("TH1") )
    {
      cout << "TresChorros: Saving Histogram: "
          << "  Class: " << o->ClassName()  
          << "  Name: "<< o->GetName() 
          << "  Title: " << o->GetTitle()
          << " " 
          << endl << flush;
      o->Write();
    }
    }
  f->Flush();
  f->Close();  

}






bool rdphi::string_true_false(TString tf)
{
  tf.ToLower();
  bool b = false;
  if ( tf == "true" ){
    b = true;
  }
  else if ( tf != "false" ) {
    throw(" option value must be \"true\" or \"false\" ");
  }
  return b;
}

void rdphi::ParseOptions(TString p_option)
{

  do_50_50 = false;
  left_50 = false;
  do_mc = false;
  do_muon = false;
  ident = "ident_test";
  input_path = "./";
  output_path = "../output";
  lepton_mass = Ok_el_mass;


  try
  {
    TObjArray *s = p_option.Tokenize(",; \n\015\012");
    cout << endl << "Okla_ParseOptions: input=" << endl 
      << p_option << endl;

    for( int i = 0 ; i < s->GetEntriesFast() ; i++)
    {
      TString c = ((TObjString *)(*s)[i])->GetString();
      c.ToLower();
      cout << endl << "Okla_ParseOptions: directive \"" << c << "\": " << endl;

      TObjArray *t = c.Tokenize("=:");

      if ( 
        (s->GetEntriesFast() == 1 ) &&
        ( t->GetEntriesFast() == 1 ) ) throw "Error processing parameters";

      if ( t->GetEntriesFast() != 2 ) throw("invalid var=val or var:val (no space or \",\" allowed around\"=\" or \":\")");
      TString var = ((TObjString *)(*t)[0])->GetString();
      TString val = ((TObjString *)(*t)[1])->GetString();


      if( var == "identifier" ){
        ident = val;
      }
      else if( var == "input_path" ){
        input_path = val;
      }
      else if( var == "output_path" ){
        output_path = val;
      }
      else if( var == "do_50_50" ){
        do_50_50 = string_true_false(val);
      }
      else if( var == "left_50" ){
        left_50 = string_true_false(val);
      }
      else if( var == "do_mc" ){
        do_mc = string_true_false(val);
      }
      
      else if( var == "do_muon" ){
        do_muon = string_true_false(val);
        if ( do_muon ) lepton_mass = Ok_mu_mass;
      }
      
      else{
        cout << endl << "throwing exception c:" << c << "   var: "<< var << "  val: " << val << endl; 
        throw( (const char *)(TString(" unknown var ") + var) );
      }
    }
  }
  catch(char * e)
  {
    cout << endl << "OklaParseOptions: Error: " << e << endl;
    exit(0);
  }
  catch(...)
  {
    cout << endl << "OklaParseOptions: Error: invalid parameter: " << endl;
    cout << p_option << endl;
    exit(0);
  }
}

void rdphi::print_usage()
{
  cout << endl << "rdphi: Usage: " << endl;
  cout << "rdphi *ok;" << endl;
  cout << "ok = new rdphi(\""
    << "identifier=name "
    << "input_path=path/ " 
    << "do_mc=true " 
    << "do_50_50=true " 
    << "left_50=true (else right 50%)  " 
    << "output_path=path/ )\"" 
    << endl  << "have fun" << endl;

}


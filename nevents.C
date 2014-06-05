// Atlas style for ratios using 50 70 100 neighbout on 24th june 2011
//a.C is the root generated macro and use them for fine tuning of result. 


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


void nevents(){
   gStyle->SetErrorX(0.0001);


 TFile* f = new TFile("output/output.root");
        f->TFile::Open();

          //TH1F *h1   = (TH1F*)f->Get("hist");
	  //h1->Sumw2();
           vector<string> names;
   for ( int i = 1 ; i <= 15 ; i++ )
       {
           
            ostringstream convert; 
             string temp;
             convert << i;
             temp ="HT"+ convert.str();         cout << temp <<endl;
             names.push_back(temp);
         }

 TCanvas *c1 = new TCanvas("c1", "Canvas title",0,0,800,600);
   gStyle->SetOptStat(0);   gStyle->SetErrorX(0.0001);
   gStyle->SetOptTitle(0);
 //  c1->Range(0,0,1,1);
  
 

      gPad->SetLogx();       gPad->SetLogy(); 

 int ii =0 ; //first ystar region
// int ii=5; //second ystar region
//int ii =10;   //final ystar region

//----- Extract the data directory

                     TH1D *R2   = (TH1D*)f->Get(names[ii].c_str());
                     R2->Draw("][ HIST");      
                   //R2->SetFillStyle(1001); 
              //     R2->SetFillColor(3); 
                      R2->SetLineColor(3);      
                   R2->SetMaximum(90000000); //to set the y axis maximum
                   R2->SetMinimum(0.5); // to set the y axis minimum
                    // R2->SetAxisRange(40,3000);
                     //R2->SetMarkerStyle(20); 
                     //R2->GetYaxis()->SetTitle("f_{#DeltaR} [p_{T},#DeltaR,p_{Tneighbor}]"); 
                      R2->GetYaxis()->SetTitle("Nevents"); 
                     R2->GetYaxis()->CenterTitle(true);
                     R2->GetYaxis()->SetTitleSize(0.06);
                     R2->SetLabelSize(0.06,"y");
                     R2->SetLabelSize(0.06,"x");
                     //R2->GetXaxis()->SetTitle("p_{T}[GeV]");
                     R2->GetXaxis()->SetMoreLogLabels(1);             //        R2->GetYaxis()->SetMoreLogLabels(1);
                    // R2->GetXaxis()->SetNoExponent(1);                     


                    
                     TH1D *R3   = (TH1D*)f->Get(names[ii+1].c_str());
                     R3->Draw("same ][ HIST");
                   //R3->SetFillStyle(1001); 
        //           R3->SetFillColor(7);        
                      R3->SetLineColor(7);      
                     //R3->SetMarkerStyle(21); 

                    
                     TH1D *R4   = (TH1D*)f->Get(names[ii+2].c_str());
                     R4->Draw("same ][ HIST"); 
                    // R4->SetFillStyle(1001);
 //                    R4->SetFillColor(4);   
                     R4->SetLineColor(4);          
                     //R4->SetMarkerStyle(22);                     
                

                     TH1D *R5   = (TH1D*)f->Get(names[ii+3].c_str());
                     R5->Draw("same ][ HIST"); R5->SetLineColor(6);     
                     //R5->SetFillStyle(1001);
                 //    R5->SetFillColor(6);                
                   

                      
                     TH1D *R6   = (TH1D*)f->Get(names[ii+4].c_str());
                     R6->Draw("same ][ HIST"); R6->SetLineColor(46);     
                     //R6->SetFillStyle(1001); 
                   //  R6->SetFillColor(46);                
                      

  
 }

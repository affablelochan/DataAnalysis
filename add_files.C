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


using namespace std;

//create a text file containing a list of all .root[.#] files located in all of the database containers located 
//in the specified directory data_path is the full name of directory containing the database containres  list_file is the name of the output file 


void build_file_list(TString list_file, TString data_path){
  gROOT->ProcessLine(".!ls -d -R "+data_path+"/*/*.root* > "+list_file);
}

void build_container_list(TString list_file, TString data_path){
  gROOT->ProcessLine(".!ls -d "+data_path +" > "+list_file);
}


void add_files(TChain &t, TString list_file){
// add files to a TChain from a text file containing
// a list of root files to be added 
//
// list_file is a text file created by build_file_list()
// t is the TChain to which file names are added
//
  fstream f(list_file,ios::in);
  TString s;
  s.ReadLine(f);
  while( s.Length() > 0){
    cout << s << endl;
    t.Add(s);
    s.ReadLine(f);
  }
  f.close();
}

void add_containers(TChain &t, TString list_file){
  fstream f(list_file,ios::in);
  TString s;
  s.ReadLine(f);
  while( s.Length() > 0){
    cout << s << endl;
    t.Add(s+"/*.root*");//"/*/*.root*");
    s.ReadLine(f);
  }
  f.close();
}


void list_files(TChain &t){
//list the files that have been added to a TChain
  TObjArray *f = t.GetListOfFiles();
  int l = f->GetEntries();
  for ( int i = 0 ; i < l ; i++){
    TChainElement *s = (TChainElement *)(*f)[i];
    cout << s->GetTitle() << endl;
  }
}

// given a TChain with a tree specified and a file added produce a list branch names and data types t is the input TChain
// out_file is the name of the text file to recieve the list




void list_branches(TChain &t, TString out_file="branch_list.txt"){

  if( &t ){
    if( t.GetNtrees() < 1 ) throw("TChain has no TTrees.");
  }
  else throw ("TChain is null.");
  fstream o(out_file,ios::trunc|ios::out|ios::in);
  TObjArray *bl = t.GetListOfBranches();
//  cout << "number of branches: " << bl->GetEntries() << endl;
  cout << "Tree "<< t.GetName() << endl;
  o    << "Tree ... "<< t.GetName() << endl;
  int l = bl->GetEntries();
  for( int i = 0 ; i < l ; i++){
      TBranch *tb = ((TBranch *)(*bl)[i]);
//      cout << tb->GetName() << endl;
      TObjArray *ll = tb->GetListOfLeaves();
//      cout << "number of leaves: " << ll->GetEntries() << endl;
      int lc =ll->GetEntries(); if ( lc > 1 ) lc = 1;
      for( int j = 0 ; j < lc /* ll->GetEntries()*/; j++)
      {
        TLeaf *tl = ((TLeaf*)(*ll)[j]);
        cout  
             << tl->GetName() << "\t"
             << tl->GetTypeName() ;
        o  
             << tl->GetName() << " \t"
             << tl->GetTypeName() ;
      }
      if ( ll->GetEntries() > 1 )
      { 
        cout << "  >>more than one leaf: " ;
        o    << "  >>more than one leaf: " ;
      }
      cout << endl; 
      o    << endl; 
  }
}

/*
void some_examples()
{
  TString e1 = "/raid04/users/walker/demo/muon";
  TString e2 = "/raid04/users/walker/demo/egamma";

  build_file_list("e1.txt",e1);
  build_file_list("e2.txt",e2);

  TChain t1("susy");

  add_files(t1,"e1.txt");
  add_files(t1,"e2.txt");
  list_files(t1);
  try{
    list_branches(t1,"branch_list.txt");
  }
  catch (char *s)
  {
    cout << endl << s << endl; 
  }


  build_container_list("c1.txt",e1);
  build_container_list("c2.txt",e2);

  TChain t2("susy");

  build_container_list("c1.txt",e1);
  build_container_list("c2.txt",e2);
  add_containers(t2,"c1.txt");
  add_containers(t2,"c2.txt");
  list_files(t2);
  list_branches(t2,"branch_list_2.txt");
}

*/




void run_s1(){
  try{
    TString e1 = "/raid04/users/walker/demo/muon";
    build_file_list("e1.txt",e1);
    
    TChain t1("susy");
    
    add_files(t1,"e1.txt");
    list_files(t1);
    list_branches(t1,"branch_list.txt");

    gROOT->SetBatch(true);
    TProof* p = TProof::Open("lite");
    //gProof->SetLogLevel(0);
    t1.SetProof();
    t1.Process("s1.C+");
    exit(0);
  }
  catch( char *s){
    cout << endl << s << endl;
  }
}

class MyTString : public TString{

public:

  TString name;

  MyTString():TString(){};
 
  MyTString operator+ ( int i )
  {
    char c[50];
    sprintf(c,"%d",i);
    MyTString s;
    s.name = this->name;
    return s;
  }

  //MyTString operator= ( int i )
  //{
  //  MyTString s;
  //  s.name = name;
  //  s = s + i;
  //  return s;
  //}


  TString operator = (fstream f){
    TString t;
    t.ReadFile(f);
    return t;
  }

  TString operator = (TString f){
    return f;
  }

  void operator >> (TString &s){
    if( name == "" ) throw "MyTString must have a name to use the >> operator (e.g. s.name = \"MyName\";)";
    TFile t(s,"UPDATE");
    if( !(&t) )throw "unable to open file "+s+ "for output";
    t.WriteObject( (TString *) this, this->name );
    t.Write();
    t.Close();
  }

  void operator << (TString &root_file){
    if( name == "" ) throw "MyTString must have a name to use the << operator (e.g. s.name = \"MyName\";)";
    TFile t(root_file,"READ");
    if( !(&t) )throw "unable to open file "+root_file+" for input";
      TString *s = (TString *) t.Get(this->name);
    if( !(&s) )throw "Root file "+root_file+" does not contain a TString with the name of "+this->name;
    t.Close();
  }

  //void getput(TString text_file, TString root_file){
  //  MyTString *s = new MyTString();
  //  cout << "saving " << text_file << endl; cout.flush();
  //  s->name = text_file;
  //  fstream *f = new fstream( text_file , ios::in );
  //  *s = *f;
  //  *s >> root_file;
  //  delete s;
  //}

};



void Save_String(TString root_file, TString string_name)
{
  
  MyTString s;
  s.name = string_name;
  s >> root_file;
}

void Load_String(TString root_file, TString string_name)
{
  MyTString s;
  s.name = string_name;
  s << root_file;
}

//void Read_String(TString text_file){
//
//  try{
//    fstream f(text_file,ios::in);
//    MyTString s = f;
//    s.name = text_file;
//  }
//  catch( char * e){
//    cout << endl << e << endl;
//  }
//}
//

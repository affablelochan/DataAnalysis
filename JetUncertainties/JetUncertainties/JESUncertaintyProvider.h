///***********************************************************************
///
/// JESUncertaintyProvider
/// Authors: C. Doglioni, D. Gillberg, P.O. DeViveiros, S. Schramm, J. Taenzer
/// PROOF-compatibility: B. Butler
///
///***********************************************************************
#ifndef _JESUNCERTAINTYPROVIDER_
#define _JESUNCERTAINTYPROVIDER_

#include "TNamed.h"
#include "TFile.h"
#include "TString.h"
#include "TH2D.h"
#include <iostream>
#include <cmath>
#include <map>
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystem.h"


using std::map;
typedef std::vector<TString> StrV;

// different kind of supported uncertainties
enum compType { uncorr_nuis, corr_nuis, mu_nuis, npv_nuis };
// different kinds of component categories
enum compCategory { cat_UNKNOWN, cat_stat, cat_detector, cat_model, cat_mixed, cat_special };

class JESUncertaintyProvider : public TNamed
{

 public:
  
  //////////////////////////////////////////////////
  //
  //  Constructor/destructor/initialization
  //
  //////////////////////////////////////////////////
  
  // Constructor
  // Input arguments:
  // 1. path to configuration file
  // 2. jet algorithm (default AntiKt R=0.4 EM)
  // 3. MC type (default MC11b)
  // 4. Path to the share directory (default none)
  JESUncertaintyProvider(TString configFile,//="InsituJES2011_AllNuisanceParameters.config",
			 TString jetAlgo="AntiKt4TopoEM", 
			 TString MCtype="MC11b", TString path="");
			 
  JESUncertaintyProvider(); //for persistification, can also be used with subsequent initJES() call.
  
  // Destructor
  virtual ~JESUncertaintyProvider();

  // Initialize the provider - only for backward compatibility, please use non-default constructor
  void initJES(TString configFile="InsituJES2011_AllNuisanceParameters.config",
	       TString jetAlgo="AntiKt4TopoEM", 
	       TString MCtype="MC11b");

  // Safely open an input file
  TFile* openInputFile(TString fn) { 
    if ( !FileExist(m_path+fn)) 
      Fatal( "JESUncertaintyProvider::openInputFile()",
	     "Cannot find file %s in %s",fn.Data(),m_path.Data() );
    TFile* fileHandle = new TFile(m_path+fn,"READ");
    if (fileHandle == NULL || fileHandle->IsZombie())
      Fatal( "JESUncertaintyProvider::openInputFile()","Found but could not open file %s in %s",fn.Data(),m_path.Data() );
    return fileHandle;
  };

// For StandAlone compatibility
#ifdef JES_STANDALONE
  ClassDef(JESUncertaintyProvider,1);
#endif //JES_STANDALONE


  //////////////////////////////////////////////////
  //
  //  General provider functions
  //
  //////////////////////////////////////////////////
  
  // Returns MC type
  TString getMCtype();

  // Set NPV and mu reference values
  void setNPVRef(double NPVRef = 4.9) { m_NPVRef = NPVRef; }
  void setMuRef(double muRef = 5.4) { m_muRef = muRef; }
  
  // Switch to units of GeV
  // (Initialized at MeV as per ATLAS policy)
  void useGeV(bool doIt=true) { m_GeV = ( doIt ? 1.0 : 1000.0 ); }


  //////////////////////////////////////////////////
  //
  //  Uncertainty getting functions
  //
  //////////////////////////////////////////////////

  // Get total, default uncertainty EXCLUDING offset term 
  // (uncertainty components added in quadrature)
  double getRelUncert(double pT, double eta);
  
  // Get relative offset uncertainty only: each term separatly, and their sum in quad.
  double getRelNPVOffsetTerm(double pT, double eta, double NPV);           // NPV term
  double getRelMuOffsetTerm(double pT, double eta, double mu);             // mu term
  double getRelPileupPtTerm(double pT, double eta, double NPV, double mu); // pt term
  double getRelPileupRhoTopology(double pT, double eta);                   // rho topology term
  double getRelOffsetUncert(double pT, double eta, double NPV, double mu); // quad. sum

  // Get total uncertainty (all uncertainty components added in quadrature)
  //double getRelUncert(double pT, double eta, double NPV, double mu);
  //double getAbsUncert(double pT, double eta, double NPV, double mu);
  
  // Get relative JES uncertainty from single component, by name or number
  double getRelUncertComponent(TString compName, double pT, double eta);
  double getRelUncertComponent(int iComp, double pT, double eta);

  
  //////////////////////////////////////////////////
  //
  //  Component information functions
  //
  //////////////////////////////////////////////////
  
  // return the number of uncertainty components
  int getNUncertaintyComponents() { return m_uncertComp.size(); }

  // All-component access helper functions for easier looping on nuisance parameters
  // Note: only the components that are listed in the .config file will be included
  // Index-parallel vectors for easy access:
  std::vector<TString> getComponentNames();           // Name for each component
  std::vector<TString> getComponentDescriptions();    // Description for each component
  std::vector<compCategory> getComponentCategories(); // Category for each component
  
  
  // Individual component accessors
  // Provide the component index (or name) to use
  int getComponentIndex(TString compName);
  TString getComponentName(int iComp);
  TString getUncertaintyDescription(int iComp);
  TString getUncertaintyDescription(TString compName);
  compCategory getComponentCategory(int iComp);
  compCategory getComponentCategory(TString compName);
 
  // Special category manipulation functions
  // Pseudo-code to loop over all components by category:
  //    for (aCategory in getListOfCategories())
  //      for (aComponent in getComponentNamesInCategory(aCategory))
  //        do stuff
  std::vector<compCategory> getListOfCategories(); // Only lists each category once
  std::vector<TString> getComponentNamesInCategory(TString category);
  std::vector<TString> getComponentNamesInCategory(compCategory category);
  TString getCategoryStringFromEnum(compCategory category); // Convert category enum to string
  
  // Whether or not a particular component has well-known correlations
  // (recommendation is to only profile on components with well known correlations)
  bool hasWellKnownCorrelations(int iComp);
  bool hasWellKnownCorrelations(TString compName);

  

 protected:

  //////////////////////////////////////////////////
  //
  //  Variables and histograms
  //
  //////////////////////////////////////////////////
  
  // Input arguments and initialization flags
  TString m_inputConfigFile;  // config file name
  TString m_jetAlgo;          // jet algorithm (ex AntiKt4TopoEM)
  TString m_MCtype;           // MC type (e.g. MC11b, MC11c, ...)
  TString m_path;             // path to the share folder
  TString m_fileName;         // Root file name read from the config file
  bool m_isInit;              // initialization flag
  
  // reference vaues
  double m_GeV;     // for MeV and GeV support
  double m_NPVRef;  // NPV reference value (initialized to 4.9 in constructor, 2011 default, set in config file for 2012)
  double m_muRef;   // mu reference value  (initialzed to 5.4 in constructor, 2011 default, set in config file for 2012)
  bool m_is2012Moriond; // Special flag for Moriond 2013 (with 2012 data) uncertainties to keep backwards compatibility
  
  // histograms
  std::vector<TH2*> m_uncertComp;     // vector of uncertainty components
  TH2 *m_uncert, *m_corrUncert;       // quadratic sum of {uncorrelated,correlated} components
  TH2 *m_muUncert, *m_npvUncert;      // The NPV and mu uncertainty
  TH2 *m_ptNpvUncert, *m_ptMuUncert;  // The pt-term uncertainty {NPV,mu} components
  TH2 *m_rhoTopologyUncert;           // The rho topology uncertainty
  
  // component information
  StrV m_compNames, m_compDesc;               // Names and descriptions for each component
  std::vector<compCategory> m_compCategories; // Categories for each component
  map<TString,int> m_compIndex;               // Map for component name to index
  std::vector<compType> m_compType;           // Component type (correlated, uncorrelated, etc)
  
  //////////////////////////////////////////////////
  //
  //  Protected methods
  //
  //////////////////////////////////////////////////
  
  // Get a histogram with error message information if it doesn't exist
  TH2 *GetHisto(TFile *file, TString hname, TString methodName);

  // Check if a file exists using Root's macros
  bool FileExist(TString fn) { return gSystem->AccessPathName(fn)==false; };

  // helper function to help those doing pseudoexperiments (still relevant now that it's protected?)
  // this is the standard provider histogram (log binning in pt, hardcoded eta bins)
  TH2 *MakeEmptyPtEtaHisto(TString hname);

  
 private :

  //////////////////////////////////////////////////
  //
  //  Private methods
  //
  //////////////////////////////////////////////////
  
  StrV Vectorize(TString str, TString sep=" ");
  
  double readPtEtaHisto(TH2 *h, double pT, double eta);
  
  // Deal with SubComponents and CorrSubComponents
  TH2 *MergeSubcomponents(StrV compVec, TString name, TString suffix, TFile *file, bool isCorrelated);

  // The uncorrelated components added in quadrature
  double getRelUncertUncorr(double pT, double eta);
  // the correlated components added linearly
  double getRelUncertBias(double pT, double eta);

  // Accessing information on a component or category
  compCategory getCategoryEnumFromString(TString category);
  bool categoryExists(compCategory category);

  // Methods relating to combining histograms
  bool checkBinning(const TH2 *in, const TH2 *sum);
  void Add(TH2 *in, TH2 *sum);
  void AddInQuad(TH2 *in, TH2 *sum);

  // helpers to test which component we're dealing with (for binning)
  bool isIntercalibrationComponent (const TString & component);
  bool isInSituComponent (const TString & component);
  bool isNonClosureComponent (const TString & component);
  bool isPileupComponent (const TString & component);
  bool isEffectiveComponent(const TString & component);
  
};

#endif

///***********************************************************************
///
/// UJUncertaintyProvider
/// Shamelessly copied from JESUncertaintyProvider: C. Doglioni, D. Gillberg, P.O. DeViveiros
/// Responsible: A. Davison, D. Miller
///
///***********************************************************************
#ifndef _UJUNCERTAINTYPROVIDER_
#define _UJUNCERTAINTYPROVIDER_

#include "TNamed.h"
#include "TFile.h"
#include "TString.h"
#include "TH3.h"
#include <iostream>
#include <cmath>
#include <map>
#include "TObjArray.h"
#include "TObjString.h"

using std::map;
typedef std::vector<TString> StrV;


class UJUncertaintyProvider : public TNamed
{

 public:
 
  // different kind of supported uncertainties
  enum compType { uncorr_nuis, corr_nuis, mu_nuis, npv_nuis };
  // Different properties for which uncertainties are available
  // Always check this matches the strings in initUJ if you change it
  enum e_prop { JPTS = 0, JMS, TAU21, TAU32, D12, D23, _PROP_SIZE };
 
  // Constructor
  // Input arguments:
  // 1. path to configuration file
  // 2. jet algorithm
  // 3. MC type MC11b
  UJUncertaintyProvider(
      TString inputFile,
  		TString jetAlgo, 
			TString MCtype="MC11b");
			 
  UJUncertaintyProvider(); //for persistification, can also be used with subsequent initUJ() call.
  
  // Destructor
  ~UJUncertaintyProvider();

  // Initialize the provider - do we really need this??
  void initUJ(
      TString inputFile,
  		TString jetAlgo="AntiKt4TopoEM", 
			TString MCtype="MC11b");

  // Get total, default uncertainty EXCLUDING offset term 
  // (uncertainty components added in quadrature)
  double getRelUncert(double pT, double mass, double eta, e_prop p);
  
  // Get relative offset uncertainty only: each term separatly, and their sum in quad.
  double getRelNPVOffsetTerm(double pT, double mass, double eta, double NPV, e_prop p);
  double getRelMuOffsetTerm(double pT, double mass, double eta, double mu, e_prop p);
  double getRelOffsetUncert(double pT, double mass, double eta, double NPV, double mu, e_prop p);

  // Get total uncertainty (all uncertainty components added in quadrature)
  double getRelUncert(double pT, double mass, double eta, double NPV, double mu, e_prop p);
  double getAbsUncert(double pT, double mass, double eta, double NPV, double mu, double val, e_prop p);
  
  // return the number of uncertainty components
  int getNUncertaintyComponents() { return m_uncertComp.size(); }

  // 
  TString getUncertaintyDescription(int iComp) { return m_compDesc.at(iComp); };
  TString getUncertaintyDescription(TString compName) { return getUncertaintyDescription(getComponentIndex(compName)); };

  // Get relative UJ uncertainty from single component, by name or number
  double getRelUncertComponent(TString compName, double pT, double mass, double eta, e_prop p);
  double getRelUncertComponent(int iComp, double pT, double mass, double eta, e_prop p);

  // Old methods, kept for backward compatibility
  double getRelUncertComponent(TString compName, double pT, double mass, double eta, 
			       double NPV, double mu, e_prop p);
  double getRelUncertComponent(int iComp, double pT, double mass, double eta, double /*NPV*/, double /*mu*/, e_prop p);

  // Returns MC type
  TString getMCtype();

#ifdef JES_STANDALONE
  ClassDef(UJUncertaintyProvider,1);
#endif //JES_STANDALONE
  
  // Set NPV and mu reference values
  void setNPVRef(double NPVRef = 4.9) { m_NPVRef = NPVRef; }
  
  void setMuRef(double muRef = 5.4) { m_muRef = muRef; }
  
  void useGeV(bool doIt=true) { m_GeV = ( doIt ? 1.0 : 1000.0 ); }
  
  // Helper function for easier looping on nuisance parameters:
  // these are the names that should be used as components when calling getRelUncert
  // Note: only the component names that are listed in the .config file will be listed
  std::vector<TString> getComponentNames();
  TString getComponentName(int iComp);
  
  // these are the titles of the nuisance parameters (index-parallel with the vector of nuisance parameter names)
  std::vector<TString> getComponentDescriptions();
  
 protected:

  // Total uncertainty for each variable i.e. E, m
  std::vector<TH3*> m_unc;

  // File for histograms
  TFile* m_file;

  // Input arguments, config file, jet collection and MC type (e.g. MC11b, MC11c...)
  TString m_inputConfigFile, m_jetAlgo, m_MCtype, m_fileName;
  
  // vector of uncertainty components
  std::vector<TH3*> m_uncertComp;
  
  // quadratic sum of uncorrelated uncertainty components, correlated components
  //TH2 *m_uncert, *m_corrUncert;
  
  // The NPV and mu uncertainty
  //TH2 *m_muUncert, *m_npvUncert;
  
  StrV m_compNames, m_compDesc;
  map<TString,int> m_compIndex;
  std::vector<compType> m_compType;
  //  map<TString,int> m_compIndex;
  
  // For standalone compatibility
  double m_GeV;
  
  // Initialization flags
  bool m_isInit;
  
  //NPV and mu (initialised in constructor to 4.9 and 5.4 respectively, as in MC11a_valid averages)
  double m_NPVRef, m_muRef;
  
  //calibration and jet type
  TString m_calib;


 private :

  // The uncorrelated components added in quadrature
  double getRelUncertUncorr(double pT, double mass, double eta, e_prop p);
  // the correlated components
  double getRelUncertBias(double pT, double mass, double eta, e_prop p);

  int getComponentIndex(TString compName);

  double readPtEtaHisto(TH3 *h, double pT, double mass, double eta);

  void Add(TH3 *in, TH3 *sum);
  bool checkBinning(const TH3 *in, const TH3 *sum);
  StrV Vectorize(TString str, TString sep=" ");

  //helpers to test which component we're dealing with (for binning)
  bool isIntercalibrationComponent (const TString & component);
  bool isInSituComponent (const TString & component);
  bool isNonClosureComponent (const TString & component);
  bool isPileupComponent (const TString & component);
};

#endif

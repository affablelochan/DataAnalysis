#include "JetUncertainties/UJUncertaintyProvider.h"
#include "TEnv.h"
#include <stdlib.h>

/////////////////////
// Constructor/destructor
/////////////////////

// Constructor
//compatibility with MultijetUJ: LC needs to be both here and in the config file
//the function parameter is ignored later on in the UJUncertaintyProvider
UJUncertaintyProvider::UJUncertaintyProvider(TString inputFile, TString jetAlgo, TString MCtype):
  m_file(0), m_GeV(1000.0), m_isInit(false), m_NPVRef(4.9), m_muRef(5.4)
{
  initUJ(inputFile, jetAlgo, MCtype);
}

//for persistification, doesn't call initUJ() automatically
UJUncertaintyProvider::UJUncertaintyProvider():
  m_file(0), m_GeV(1000.0), m_isInit(false), m_NPVRef(4.9), m_muRef(5.4) { }

// Destructor
UJUncertaintyProvider::~UJUncertaintyProvider()
{
  if (m_file) {
    m_file->Close();
    delete m_file;
  }
}

int UJUncertaintyProvider::getComponentIndex(TString compName) {
  if (m_compIndex.find(compName)==m_compIndex.end())
    Fatal("UJUncertaintyProvider::getComponentIndex()",
	  "ERROR: No such uncertainty component: %s",compName.Data());
  return m_compIndex[compName];
}

double UJUncertaintyProvider::readPtEtaHisto(TH3 *h3d, double pt, double mass, double eta) {
  static int nWarn = 0;
  if ( pt/m_GeV<100.0 || pt/m_GeV>=1000.0 ) {
    if (++nWarn<10) 
      Warning("UJUncertaintyProvider::readPtEtaHisto()","jet pT outside 100-1000 range: pT=%.1f GeV \n (only first 10 messages will be printed).",pt/m_GeV);
    if (pt/m_GeV<100.0) pt=100.0*m_GeV;
    if (pt/m_GeV>=1000.0) pt=1000.0*m_GeV;
  }
  if (fabs(eta)>=1.2) {
   if (++nWarn<10) 
     Warning("UJUncertaintyProvider::readPtEtaHisto()","jet eta=%.2f outside |eta|<1.2 \n (only first 10 messages will be printed).",eta);
     eta=1.2;
  }
  //if (mass/m_GeV<0.0 || mass/m_GeV>=400.0) {
  // if (++nWarn<10) 
  //   Warning("UJUncertaintyProvider::readPtEtaHisto()","jet mass=%.2f outside 0-400 range\n (only first 10 messages will be printed).",mass/m_GeV);
  //   if (mass/m_GeV<0.0) { mass = 0.0*m_GeV; }
  //   if (mass/m_GeV>400.0) { mass = 400.0*m_GeV; }
  //}
  double mopt = mass / pt;
  // NOTE WE HAVE fabs HERE, NOT THE CASE IN JESUncertaintyProvider!
  return h3d->Interpolate(pt/m_GeV, mopt,fabs(eta));
  
}

/////////////////////
// Initialisation
/////////////////////
void UJUncertaintyProvider::initUJ(TString inputFile, TString jetAlgo, TString MCtype) 
{

  //prevent multiple initializations
  if (m_isInit == false) {

    m_compIndex["Total Uncertainty"] = 0;

    m_file = new TFile(inputFile, "READ");
    if (!m_file->IsOpen()) {
      Fatal("UJUncertaintyProvider::initUJ()", "Cannot open input file %s!", (const char*)inputFile);
    }

    m_jetAlgo = jetAlgo;
    m_MCtype = MCtype;

    // Always check this matches e_prop in the headers if you change it
    static const char* propstr[_PROP_SIZE] = {"pT", "m", "tau21", "tau32", "d12", "d23"};

    for (int i = 0; i < _PROP_SIZE; i++) {
      TString hkey = TString::Format("%s_%s", (const char*)jetAlgo, propstr[i]);
      TH3F* unch = (TH3F*)m_file->Get(hkey);
      if (!unch) {
        Fatal("UJUncertaintyProvider::initUJ()", "Algorithm %s and variable %s is not present in input file!", (const char*)jetAlgo, propstr[i]);
      }
      m_unc.push_back(unch);
    }

    Info("UJUncertaintyProvider::init()","================================================");
    Info("UJUncertaintyProvider::init()","  Initialized the UJUncertaintyProvider tool");
    Info("UJUncertaintyProvider::init()","  Using track-jet uncertainties" );
    Info("UJUncertaintyProvider::init()","  For %s",(const char*)jetAlgo);
    
  }
  else {
    Warning( "UJUncertaintyProvider::init()", "WARNING: UJUncertaintyProvider already initialized, skipping re-initialization");
  }
}

//bool UJUncertaintyProvider::setInputCollection(TString jetAlgo) {} 

TString UJUncertaintyProvider::getMCtype() {
  if (m_MCtype.Contains("MC11b", TString::kIgnoreCase)) return "MC11b";
  if (m_MCtype.Contains("MC11c", TString::kIgnoreCase)) return "MC11c";
  if (m_MCtype.Contains("FrozenShowers", TString::kIgnoreCase)) return "FrozenShowers";
  if (m_MCtype.Contains("AFII", TString::kIgnoreCase)) return "AFII";
  return "Error";
}

double UJUncertaintyProvider::getRelUncertComponent(TString compName, double pT, double mass, double eta, e_prop p) {
  return getRelUncertComponent(getComponentIndex(compName),pT,mass,eta,p);
}

double UJUncertaintyProvider::getRelUncertComponent(int iComp, double pT, double mass, double eta, e_prop p) {
  if (iComp>getNUncertaintyComponents())
    Fatal("UJUncertaintyProvider::getRelUncertComponent","UJ uncertainty component index out-of-range.");
  TH3 *h3d = m_unc.at(p);
  return readPtEtaHisto(h3d,pT,mass,eta);
}

// This method should be removed at some point
double UJUncertaintyProvider::getRelUncertComponent(TString compName, double pT, double mass, double eta, 
						     double NPV, double mu, e_prop p) {
  return getRelUncertComponent(getComponentIndex(compName),pT,mass,eta,NPV,mu, p);
}

// This method should be removed at some point
double UJUncertaintyProvider::getRelUncertComponent(int iComp, double pT, double mass, double eta, 
						     double /*NPV*/, double /*mu*/, e_prop p) {

  return getRelUncertComponent(iComp,pT,mass,eta,p);
}


/////////////////////
// Uncertainty 
/////////////////////

// Absolute Uncertainty
double UJUncertaintyProvider::getAbsUncert(double pT, double mass, double eta, double NPV, double mu, double val, e_prop p) {
  return getRelUncert(pT, mass, eta, NPV, mu, p)*val;
}

double UJUncertaintyProvider::getRelUncert(double pT, double mass, double eta, double NPV, double mu, e_prop p) {
  return sqrt( pow(getRelUncert(pT,mass,eta,p),2) + pow(getRelOffsetUncert(pT,mass,eta,NPV,mu,p),2) );
}

double UJUncertaintyProvider::getRelUncert(double pT, double mass, double eta, e_prop p) {
  return fabs(getRelUncertUncorr(pT,mass,eta, p)) + fabs(getRelUncertBias(pT,mass,eta, p));
}

double UJUncertaintyProvider::getRelUncertUncorr(double pT, double mass, double eta, e_prop p) {
  return getRelUncertComponent(0, pT, mass, eta, p); 
}

double UJUncertaintyProvider::getRelUncertBias(double pT, double mass, double eta, e_prop p) {
  // Cast parameters to void to tell the compiler that we know the variables aren't used right now
  (void)pT;
  (void)eta;
  (void)p;
  (void)mass;
  return 0.0;
}

double UJUncertaintyProvider::getRelNPVOffsetTerm(double pT, double mass, double eta, double NPV, e_prop p) {
  // Cast parameters to void to tell the compiler that we know the variables aren't used right now
  (void)pT;
  (void)mass;
  (void)eta;
  (void)p;
  (void)NPV;
  return 0.0;
}

double UJUncertaintyProvider::getRelMuOffsetTerm(double pT, double mass, double eta, double mu, e_prop p) {
  // Cast parameters to void to tell the compiler that we know the variables aren't used right now
  (void)pT;
  (void)mass;
  (void)eta;
  (void)mu;
  (void)p;
  return 0.0;
}

double UJUncertaintyProvider::getRelOffsetUncert(double pT, double mass, double eta, double NPV, double mu, e_prop p) {
  double muUncert = getRelMuOffsetTerm(pT,mass,eta,mu,p);
  double npvUncert = getRelNPVOffsetTerm(pT,mass,eta,NPV,p);
  return sqrt( muUncert*muUncert + npvUncert*npvUncert );
}

//function for easier looping on nuisance parameters
//these are the names that should be used as components when calling getRelUncert
std::vector<TString> UJUncertaintyProvider::getComponentNames() {  
  return m_compNames;
}

TString UJUncertaintyProvider::getComponentName(int iComp) {
  if (iComp>=int(m_compNames.size()))
    Fatal("UJUncertaintyProvider::getComponentName()",
	  "You are asking for comp. %d. Only %d components available.",iComp+1,int(m_compNames.size()));
  return m_compNames.at(iComp);
}



//these are the titles of the nuisance parameters (index-parallel with the vector of names)
std::vector<TString> UJUncertaintyProvider::getComponentDescriptions() {
  return m_compDesc;
}

bool UJUncertaintyProvider::isIntercalibrationComponent (const TString & component) {
  return component.Contains("etaIntercalibration", TString::kIgnoreCase);
}

bool UJUncertaintyProvider::isInSituComponent (const TString & component) {
  return ( component.Contains("InSitu", TString::kIgnoreCase) || 
	   component.Contains("Zjet", TString::kIgnoreCase) || 
	   component.Contains("MPF", TString::kIgnoreCase) ||
	   component.Contains("MJB", TString::kIgnoreCase) );
}

bool UJUncertaintyProvider::isNonClosureComponent (const TString & component) {
  return ( component.Contains("NonClosure", TString::kIgnoreCase) );
}

bool UJUncertaintyProvider::isPileupComponent (const TString & component) {
  return ( component.Contains("Pileup", TString::kIgnoreCase) );
}

StrV UJUncertaintyProvider::Vectorize(TString str, TString sep) {
  StrV result; TObjArray *strings = str.Tokenize(sep.Data());
  if (strings->GetEntries()==0) return result;
  TIter istr(strings);
  while (TObjString* os=(TObjString*)istr())
    if (os->GetString()[0]!='#') result.push_back(os->GetString());
    else break;
  delete strings;
  return result;
}


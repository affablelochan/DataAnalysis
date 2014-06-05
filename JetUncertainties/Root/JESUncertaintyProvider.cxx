#include "JetUncertainties/JESUncertaintyProvider.h"
#include "JetUncertainties/JESUtils.h"
#include "TEnv.h"
#include "TROOT.h"
#include <stdlib.h>


//////////////////////////////////////////////////
//
//  Constructor/destructor/initialization
//
//////////////////////////////////////////////////

// Constructor
//compatibility with MultijetJES: LC needs to be both here and in the config file
//the function parameter is ignored later on in the JESUncertaintyProvider
JESUncertaintyProvider::JESUncertaintyProvider(TString inputConfigFile, TString jetAlgo, TString MCtype, TString path):
  m_path(path), m_isInit(false), m_GeV(1000.0), m_NPVRef(4.9), m_muRef(5.4), m_is2012Moriond(false),
  m_uncert(0), m_corrUncert(0), m_muUncert(0), m_npvUncert(0), m_ptNpvUncert(0), m_ptMuUncert(0), m_rhoTopologyUncert(0)
{
  initJES(inputConfigFile, jetAlgo, MCtype);
}

//for persistification, doesn't call initJES() automatically
JESUncertaintyProvider::JESUncertaintyProvider():
  m_path(""), m_isInit(false), m_GeV(1000.0), m_NPVRef(4.9), m_muRef(5.4), m_is2012Moriond(false),
  m_uncert(0), m_corrUncert(0), m_muUncert(0), m_npvUncert(0), m_ptNpvUncert(0), m_ptMuUncert(0), m_rhoTopologyUncert(0)
{ }

// Destructor
JESUncertaintyProvider::~JESUncertaintyProvider()
{
  for (unsigned i=0;i<m_uncertComp.size();++i)
    if (m_uncertComp[i]) delete m_uncertComp[i];
  if (m_uncert) delete m_uncert;
  if (m_corrUncert) delete m_corrUncert;
  if (m_muUncert) delete m_muUncert;
  if (m_npvUncert) delete m_npvUncert;
  if (m_ptNpvUncert) delete m_ptNpvUncert;
  if (m_ptMuUncert) delete m_ptMuUncert;
  if (m_rhoTopologyUncert) delete m_rhoTopologyUncert;
}


/////////////////////
// Initialisation
/////////////////////
void JESUncertaintyProvider::initJES(TString inputConfigFile, TString jetAlgo, TString MCtype) 
{
  
  //cache current directory 
  TDirectory* curdir = gDirectory;  
  gROOT->cd(); 
  
  //prevent multiple initializations
  if (m_isInit == false) {
    
    // Identify the path to the uncertainty configuration files based on the location of
    // the main configuration file according to the below priority:
    //   1. User specified path using the setPath("myPath") method
    //   2. The current directory: ./
    //   3. $ROOTCOREDIR/JetUncertainties/data 
    //      (which is a soft link to JetUncertainites/share)
    //   4. If athena is set up ($TestArea exists), the standard athena location:
    //      $TestArea/Reconstruction/Jet/JetUncertainties/share
    if ( m_path!="" ) {
      // 1. User specified path
      if (m_path[m_path.Length()-1]!='/') m_path+='/';
      if (!FileExist(m_path+inputConfigFile)) 
	Fatal("JESUncertaintyProvider::init()","Cannot find file %s\nUser specified path: %s",
	      (m_path+inputConfigFile).Data(),m_path.Data());
    } else if (FileExist(inputConfigFile)) {
      m_path="./"; 
    } else {
    
      // First check ROOTCORE
      TString systemPath = gSystem->Getenv("ROOTCOREBIN");
      if (systemPath == "")
        systemPath = gSystem->Getenv("ROOTCOREDIR");
      if (systemPath != "" && FileExist(systemPath+"/data/JetUncertainties"))
        m_path = systemPath+"/data/JetUncertainties/";
      else
      {
        systemPath = gSystem->Getenv("TestArea");
        if (systemPath != "" && FileExist(systemPath+"/Reconstruction/Jet/JetUncertainties/share/"))
          m_path = systemPath+"/Reconstruction/Jet/JetUncertainties/share/";
        else
          Fatal("JESUncertaintyProvider::init()","\nCannot find file %s\nSearched in:\n  1. ./\n  2. $ROOTCOREBIN/data/JetUncertainties/\n  3. $ROOTCOREDIR/data/JetUncertainties/\n  4. $TestArea/Reconstruction/Jet/JetUncertainties/share/",inputConfigFile.Data());
      }
    }
    m_inputConfigFile = m_path+inputConfigFile;

    Info("JESUncertaintyProvider::init()", "Path for input files: %s",m_path.Data());

    m_jetAlgo = jetAlgo;
    m_MCtype = MCtype;
 
    // 1. Check jet algo
    int jetR = 4;
    if (m_jetAlgo.Contains("Kt6")) jetR=6;
    else if (!m_jetAlgo.Contains("Kt4")) 
      Fatal("JESUncertaintyProvider::init()", "ERROR: Cannot interpret jet algo argument: %s",m_jetAlgo.Data());
    
    TString calib="EMJES";
    if (m_jetAlgo.Contains("LC")) calib="LCJES";
    else if (!m_jetAlgo.Contains("EM")) 
      Fatal("JESUncertaintyProvider::init()", "ERROR: Cannot interpret jet algo argument: %s",m_jetAlgo.Data());
    
    // 2. Check the MC type
    TString mcType=getMCtype();
    if (mcType=="Error") 
      Fatal("JESUncertaintyProvider::init()", "ERROR: Cannot interpret MC type argument: %s",m_MCtype.Data());
    
    // -- Input arguments seems OK, lets print a message:
    // --------------------------------------------------
    
    Info("JESUncertaintyProvider::init()","================================================");
    Info("JESUncertaintyProvider::init()","  Initializing the JESUncertaintyProvider tool");
    Info("JESUncertaintyProvider::init()","  Using combination of in-situ techniques" );
    Info("JESUncertaintyProvider::init()","  For Anti kt R=0.%d, %s",jetR,TString(calib).ReplaceAll("JES","+JES").Data());
    Info("JESUncertaintyProvider::init()","  Configuration read in from:" );
    Info("JESUncertaintyProvider::init()","    %s",m_inputConfigFile.Data());
    
    // -- Now read the config file, and identify and open the input file

    TEnv settings;
    //this returns -1 if unsuccessful
    int status = settings.ReadFile(m_inputConfigFile.Data(),EEnvLevel(0));
    if (status!=0) 
      Fatal("JESUncertaintyProvider::init()", 
	    "Cannot read file  %s",m_inputConfigFile.Data());

    //avoid ROOT's "Replacing existing TH1" by having specialised file name
    TString emptyHistoSuffix = m_inputConfigFile;
    m_uncert = MakeEmptyPtEtaHisto("TotalUncertainty_"+emptyHistoSuffix+"_"+m_jetAlgo+"_"+m_MCtype);
    m_corrUncert = MakeEmptyPtEtaHisto("TotalBias_"+emptyHistoSuffix+"_"+m_jetAlgo+"_"+m_MCtype);

    m_fileName = settings.GetValue("JESUncertaintyRootFile","");
    if (m_fileName.CompareTo("")==0) Fatal("JESUncertaintyProvider::init()",
                                      "No JES root file specified in config file");
    
    //need to do this via separate function for ATHENA's PathResolver
    TFile* inputFile = NULL;
    inputFile = openInputFile(m_fileName);

    if (inputFile==NULL)
      Fatal("JESUncertaintyProvider::init()","Cannot open %s",m_fileName.Data());

    TString suffix=Form("_AntiKt%dTopo_",jetR)+calib;
    
    
    // Read in whether this is the Moriond2012 calibration (2012 data, released for Moriond 2013)
    TString is2012Moriond = settings.GetValue("Is2012Moriond","NO");
    if (is2012Moriond.Contains("YES",TString::kIgnoreCase) || is2012Moriond.Contains("TRUE",TString::kIgnoreCase))
    {
      m_is2012Moriond = true;
      // Currently no AntiKt6LCTopo calibration for this release, so not allowed
      if (  m_jetAlgo.Contains("LC",TString::kIgnoreCase) && m_jetAlgo.Contains("6") && 
            m_jetAlgo.Contains("AntiKt",TString::kIgnoreCase) &&
            mcType.Contains("AFII",TString::kIgnoreCase)
         )
        Fatal("JESUncertaintyProvider::init()","Unfortunately AntiKt6LCTopo AFII calibrations did not finish running in time for the Moriond recommendation.");
    }
    
    
    // Let's read in all those uncertainty components!
    Info("JESUncertaintyProvider::init()","%6s %-30s : %s","","JES uncert. comp.","Description");
    Info("JESUncertaintyProvider::init()","%6s %-30s --%s","","-----------------","-----------");
    for (int ci=0;ci<250;++ci) {
      
      //easier to printf-style this 
      TString prefix=Form("JESComponent.%d.",ci);

      // Read in the name, description, and type of the uncertainty component
      TString compName = settings.GetValue(prefix+"Name","");
      TString desc     = settings.GetValue(prefix+"Desc","");
      TString category = settings.GetValue(prefix+"Type","");

      // ignore if component is not defined
      if (compName=="") continue;

      bool addAsCorrelated = settings.GetValue(prefix+"AddAsCorrelated",false);
      
      compName.ReplaceAll("MCTYPE",mcType);

      int index=m_uncertComp.size();

      /*
       *  Read in the uncertainty component
       */
      TH2 *uncComp = 0;
      StrV subComponents = Vectorize(settings.GetValue(prefix+"SubComponents",""));
      StrV corrSubComponents = Vectorize(settings.GetValue(prefix+"CorrSubComponents",""));
      if (subComponents.size())
        uncComp = MergeSubcomponents(subComponents,compName,suffix,inputFile,false);// "false" as not correlated subcomponents
      else if (corrSubComponents.size())
          uncComp = MergeSubcomponents(corrSubComponents,compName,suffix,inputFile,true);// "true" as correlated subcomponents
      else 
	uncComp = GetHisto(inputFile,compName+suffix,"JESUncertaintyProvider::init()");
     
      uncComp->SetName(TString("the").Append(compName)); // for PROOF compatibility
      uncComp->SetDirectory(0);
      
      /*
       *  If this is the pile-up uncertainty, store it away and jump to next component
       */
      if (compName.Contains("offsetmu",TString::kIgnoreCase)) {
	//m_compType.push_back(mu_nuis);
	if ( m_muUncert == NULL ) m_muUncert = uncComp;
	else Fatal("JESUncertaintyProvider::init()","Mu offset component specified twice? %s",compName.Data());
	continue;
      } else if (compName.Contains("offsetnpv",TString::kIgnoreCase)) {
	//m_compType.push_back(npv_nuis);
	if ( m_npvUncert == NULL ) m_npvUncert = uncComp;
	else Fatal("JESUncertaintyProvider::init()","NPV offset component specified twice? %s",compName.Data());
	continue;
      } else if (compName.Contains("PtTerm_NPV",TString::kIgnoreCase)) {
        if (m_ptNpvUncert == NULL) m_ptNpvUncert = uncComp;
        else Fatal("JESUncertaintyProvider::init()","Pileup Pt-term NPV component specified twice? %s",compName.Data());
        continue;
      } else if (compName.Contains("PtTerm_Mu",TString::kIgnoreCase)) {
        if (m_ptMuUncert == NULL) m_ptMuUncert = uncComp;
        else Fatal("JESUncertaintyProvider::init()","Pileup Pt-term mu component specified twice? %s",compName.Data());
        continue;
      } else if (compName.Contains("RhoTopology",TString::kIgnoreCase)) {
        if (m_rhoTopologyUncert == NULL) m_rhoTopologyUncert = uncComp;
        else Fatal("JESUncertaintyProvider::init()","Pileup rho topology component specified twice? %s",compName.Data());
        continue;
      }
      
      /*
       *  If not pile-up, print it, cache it, and add it to the list ...
       */
      Info("JESUncertaintyProvider::init()","%5d. %-30s : %s",index+1,compName.Data(),desc.Data());
      if (addAsCorrelated) 
	Info("JESUncertaintyProvider::init()","%6s  => added as fully correlated","");

      // a. Cache, cache baby
      if (addAsCorrelated) {
	m_compType.push_back(corr_nuis);
	Add(uncComp,m_corrUncert);
      } else {
	m_compType.push_back(uncorr_nuis);
	AddInQuad(uncComp,m_uncert);
      }

      // b. Store histo and the index in the histogram vector
      m_compIndex[compName]=index;
      m_uncertComp.push_back(uncComp);
      
      // c. Store the name, the description and correlation information, and the category
      m_compNames.push_back(compName);
      m_compDesc.push_back(desc);
      compCategory cat;
      if (category.CompareTo("")==0)
        cat = cat_UNKNOWN; // No category provided, leave it as unknown
      else
        cat = getCategoryEnumFromString(category); // Get the corresponding enum
      m_compCategories.push_back(cat);
    }

    // Read in the pile-up uncertainty reference point
    double muRef = settings.GetValue("Pileup.MuRef",-1.0);
    double npvRef = settings.GetValue("Pileup.NPVRef",-1.0);
    if (muRef>=0) setMuRef(muRef); // sets m_muRef
    if (npvRef>=0) setNPVRef(npvRef); // sets m_npvRef


    // write lines
    //printf(  "%6s %-30s --%s\n","---","-----------------","-----------");
    m_isInit = true;
    if (  m_muUncert ) 
      Info("JESUncertaintyProvider::init()","    Pileup: Offset, mu term");
    if (  m_npvUncert ) 
      Info("JESUncertaintyProvider::init()","    Pileup: Offset, NPV term");
    if (  m_ptMuUncert && m_ptNpvUncert)
      Info("JESUncertaintyProvider::init()","    Pileup: Offset, pT term, mu and NPV components");
    if (  m_rhoTopologyUncert)
      Info("JESUncertaintyProvider::init()","    Pileup: Offset, rho topology");

    Info("JESUncertaintyProvider::init()","    Pileup reference: (NPV,mu) = (%.1f,%.1f)",m_NPVRef,m_muRef);
    
    // Print category information
    Info("JESUncertaintyProvider::init()","   Read in %d parameters in %d categories",unsigned(m_compNames.size()),unsigned(getListOfCategories().size()));
    Info("JESUncertaintyProvider::init()","================================================");

    inputFile->Close(); 
    delete inputFile;
  }//end of if not initialised
  else {
    Warning( "JESUncertaintyProvider::init()", "WARNING: JESUncertaintyProvider already initialized, skipping re-initialization");
  }
  
  //go back to earlier directory 
  gDirectory = curdir;  

}




//////////////////////////////////////////////////
//
//  General provider functions
//
//////////////////////////////////////////////////

TString JESUncertaintyProvider::getMCtype() {
  if (m_MCtype.Contains("MC11b", TString::kIgnoreCase)) return "MC11b";
  if (m_MCtype.Contains("MC11c", TString::kIgnoreCase)) return "MC11c";
  if (m_MCtype.Contains("FrozenShowers", TString::kIgnoreCase)) return "FrozenShowers";
  if (m_MCtype.Contains("AFII", TString::kIgnoreCase)) return "AFII";
  if (m_MCtype.Contains("Pythia8", TString::kIgnoreCase)) return "Pythia8";
  if (m_MCtype.Contains("MC12a", TString::kIgnoreCase)) return "Pythia8";
  return "Error";
}



//////////////////////////////////////////////////
//
//  Uncertainty getting functions
//
//////////////////////////////////////////////////

double JESUncertaintyProvider::getRelUncert(double pT, double eta) {
  return fabs(getRelUncertUncorr(pT,eta)) + fabs(getRelUncertBias(pT,eta));
}

double JESUncertaintyProvider::getRelNPVOffsetTerm(double pT, double eta, double NPV) {
  if ( m_npvUncert == NULL ) 
    Fatal("JESUncertaintyProvider::getRelNPVOffsetTerm",
	  "Offset NPV-dependent uncertainty not initialized");
  return readPtEtaHisto(m_npvUncert,pT,eta) * ( NPV - m_NPVRef);
}

double JESUncertaintyProvider::getRelMuOffsetTerm(double pT, double eta, double mu) {
  if ( m_muUncert == NULL ) 
    Fatal("JESUncertaintyProvider::getRelMuOffsetTerm",
  	  "Offset mu-dependent uncertainty not initialized");
  return readPtEtaHisto(m_muUncert,pT,eta) * ( mu - m_muRef);
}

double JESUncertaintyProvider::getRelPileupPtTerm(double pT, double eta, double NPV, double mu)
{
  if ( m_ptNpvUncert == NULL )
    Fatal("JESUncertaintyProvider::getRelPileupPtTerm",
          "Offset pt-NPV-dependent uncertainty not initialized");
  if ( m_ptMuUncert == NULL )
    Fatal("JESUncertaintyProvider::getRelPileupPtTerm",
          "Offset pt-mu-dependent uncertainty not initialized");
  return readPtEtaHisto(m_ptNpvUncert,pT,eta)*(NPV - m_NPVRef) + readPtEtaHisto(m_ptMuUncert,pT,eta)*(mu - m_muRef);
}

double JESUncertaintyProvider::getRelPileupRhoTopology(double pT, double eta)
{
  if ( m_rhoTopologyUncert == NULL)
    Fatal("JESUncertaintyProvider::getRelPileupRhoTopology",
          "Offset rho topology uncertainty not initialized");
  return readPtEtaHisto(m_rhoTopologyUncert,pT,eta);
}

double JESUncertaintyProvider::getRelOffsetUncert(double pT, double eta, double NPV, double mu) {
  double muUncert = getRelMuOffsetTerm(pT,eta,mu);
  double npvUncert = getRelNPVOffsetTerm(pT,eta,NPV);
  if (!m_is2012Moriond)
    // Backward compatibility hack, pileup pt term and rho topology are new to Moriond2013 calibration
    return sqrt( muUncert*muUncert + npvUncert*npvUncert );

  double ptTermUncert = getRelPileupPtTerm(pT,eta,NPV,mu);
  double rhoTopologyUncert = getRelPileupRhoTopology(pT,eta);
  return sqrt(muUncert*muUncert + npvUncert*npvUncert + ptTermUncert*ptTermUncert + rhoTopologyUncert*rhoTopologyUncert);
}

// Absolute Uncertainty
/*
double JESUncertaintyProvider::getAbsUncert(double pT, double eta, double NPV, double mu) {
  return getRelUncert(pT, eta, NPV, mu)*pT;
}

double JESUncertaintyProvider::getRelUncert(double pT, double eta, double NPV, double mu) {
  return sqrt( pow(getRelUncert(pT,eta),2) + pow(getRelOffsetUncert(pT,eta,NPV,mu),2) );
}
*/

double JESUncertaintyProvider::getRelUncertComponent(TString compName, double pT, double eta) {
  return getRelUncertComponent(getComponentIndex(compName),pT,eta);
}

double JESUncertaintyProvider::getRelUncertComponent(int iComp, double pT, double eta) {
  if (iComp>getNUncertaintyComponents())
    Fatal("JESUncertaintyProvider::getRelUncertComponent","JES uncertainty component index out-of-range.");
  TH2 *h2d = m_uncertComp.at(iComp);
  return readPtEtaHisto(h2d,pT,eta);
}

  
//////////////////////////////////////////////////
//
//  Component information functions
//
//////////////////////////////////////////////////

//function for easier looping on nuisance parameters
//these are the names that should be used as components when calling getRelUncert
std::vector<TString> JESUncertaintyProvider::getComponentNames() {  
  return m_compNames;
}

//these are the titles of the nuisance parameters (index-parallel with the vector of names)
std::vector<TString> JESUncertaintyProvider::getComponentDescriptions() {
  return m_compDesc;
}

// These are the categories for the nuisance parameters (index-parallel with the vector of names)
std::vector<compCategory> JESUncertaintyProvider::getComponentCategories() {
  return m_compCategories;
}

TString JESUncertaintyProvider::getComponentName(int iComp) {
  if (iComp>=int(m_compNames.size()))
    Fatal("JESUncertaintyProvider::getComponentName()",
	  "You are asking for comp. %d. Only %d components available.",iComp+1,int(m_compNames.size()));
  return m_compNames.at(iComp);
}

TString JESUncertaintyProvider::getUncertaintyDescription(int iComp)
{
  if (iComp >= int(m_compDesc.size()))
    Fatal("JESUncertaintyProvider::getUncertaintyDescription()",
          "You are asking for comp. %d. Only %d components available.",iComp+1,int(m_compDesc.size()));
  return m_compDesc.at(iComp);
}

TString JESUncertaintyProvider::getUncertaintyDescription(TString compName)
{ return getUncertaintyDescription(getComponentIndex(compName)); }

compCategory JESUncertaintyProvider::getComponentCategory(int iComp)
{
  if (iComp >= int(m_compCategories.size()))
    Fatal("JESUncertaintyProvider::getComponentCategory()",
          "You are asking for comp. %d. Only %d components available.",iComp+1,int(m_compCategories.size()));
  return m_compCategories.at(iComp);
}

compCategory JESUncertaintyProvider::getComponentCategory(TString compName)
{ return getComponentCategory(getComponentIndex(compName)); }



std::vector<compCategory> JESUncertaintyProvider::getListOfCategories()
{
  std::vector<compCategory> categoriesReadIn;
  
  if (categoryExists(cat_UNKNOWN))
    categoriesReadIn.push_back(cat_UNKNOWN);
  if (categoryExists(cat_stat))
    categoriesReadIn.push_back(cat_stat);
  if (categoryExists(cat_detector))
    categoriesReadIn.push_back(cat_detector);
  if (categoryExists(cat_model))
    categoriesReadIn.push_back(cat_model);
  if (categoryExists(cat_mixed))
    categoriesReadIn.push_back(cat_mixed);
  if (categoryExists(cat_special))
    categoriesReadIn.push_back(cat_special);

  return categoriesReadIn;
}

std::vector<TString> JESUncertaintyProvider::getComponentNamesInCategory(TString category)
{
  return getComponentNamesInCategory(getCategoryEnumFromString(category));
}

std::vector<TString> JESUncertaintyProvider::getComponentNamesInCategory(compCategory category)
{
  unsigned int numComps = m_compNames.size();
  StrV namesInCategory;
  for (unsigned int i = 0; i < numComps; i++)
    if (m_compCategories[i] == category)
      namesInCategory.push_back(m_compNames[i]);

  return namesInCategory;
}

TString JESUncertaintyProvider::getCategoryStringFromEnum(compCategory category)
{
  if (category == cat_stat)           return "Statistical";
  else if (category == cat_detector)  return "Detector";
  else if (category == cat_model)     return "Modelling";
  else if (category == cat_mixed)     return "Mixed";
  else if (category == cat_special)   return "Special";
  else                                return "UNKNOWN";
}

bool JESUncertaintyProvider::hasWellKnownCorrelations(int iComp)
{
  TString component = getComponentName(iComp);
  // The residual terms from effective components do not
  if (isEffectiveComponent(component))
  {
    // Easy case (global reduction)
    if (component.Contains("restTerm",TString::kIgnoreCase))
      return false;
    // Harder case (category reduction)
    else
    {
      compCategory cat = getComponentCategory(iComp);
      if (iComp != cat_UNKNOWN && iComp != cat_special)
      {
        // Check if it's the last effective term in the category (the residual term)
        // (There may be non-effective terms in the category too, be careful)
        StrV compsInCat = getComponentNamesInCategory(cat);
        unsigned int numNonEffective = 0;
        for (unsigned int i = 0; i < compsInCat.size(); i++)
          if (!isEffectiveComponent(compsInCat[i]))
            numNonEffective++;
        // Not well known if rest term (last effective component in category)
        if (component.Contains(Form("%d",int(compsInCat.size()-numNonEffective))))
          return false;
      }
    }
  }
  // Also some eta intercalibration components
  else if (isIntercalibrationComponent(component))
  {
    // two-point component (difference between Pythia and Herwig)
    if (component.Contains("Modelling",TString::kIgnoreCase))
      return false;
    // Quadrature sum of a huge number of components
    else if (component.Contains("TotalStat",TString::kIgnoreCase))
      return false;
    // Sum of components
    else if (component.Contains("StatAndMethod",TString::kIgnoreCase))
      return false;
  }
  // Also non-closure terms
  else if (isNonClosureComponent(component))
    return false;
  // Also single particle high pt term
  else if (component.Contains("SingleParticle_HighPt",TString::kIgnoreCase))
    return false;
  // Full baseline as one component (in preliminary files before insitu completed)
  else if (component.Contains("Baseline2010",TString::kIgnoreCase) ||
           component.Contains("BaselineJES",TString::kIgnoreCase)  ||
           component.Contains("ForwardJES",TString::kIgnoreCase)    )
    return false;
  return true;
}

bool JESUncertaintyProvider::hasWellKnownCorrelations(TString compName)
{ return hasWellKnownCorrelations(getComponentIndex(compName)); }


  
  
//////////////////////////////////////////////////
//
//  Protected methods
//
//////////////////////////////////////////////////

TH2 *JESUncertaintyProvider::GetHisto(TFile *file, TString hname, TString method) {
  TH2 *h = (TH2*)file->Get(hname);
  if (h==NULL) 
    Fatal(method.Data(),"Cannot access histo %s in file %s",hname.Data(),file->GetName());
  return h;
}

TH2 *JESUncertaintyProvider::MakeEmptyPtEtaHisto(TString hname) {
  return JESUtils::MakeEmptyPtEtaHisto(hname);
}



//////////////////////////////////////////////////
//
//  Private methods
//
//////////////////////////////////////////////////

StrV JESUncertaintyProvider::Vectorize(TString str, TString sep) {
  StrV result; TObjArray *strings = str.Tokenize(sep.Data());
  if (strings->GetEntries())
  {
    TIter istr(strings);
    while (TObjString* os=(TObjString*)istr())
      if (os->GetString()[0]!='#') result.push_back(os->GetString());
      else break;
  }
  delete strings;
  return result;
}

double JESUncertaintyProvider::readPtEtaHisto(TH2 *h2d, double pt, double eta) {
  static int nWarn = 0;
  if ( pt/m_GeV<15 || pt/m_GeV>=2500 ) {
    if (++nWarn<10) 
      Warning("JESUncertaintyProvider::readPtEtaHisto()","jet pT outside 15-2500 range: pT=%.1f GeV. Using closest valid value. \n (only first 10 messages will be printed). ",pt/m_GeV);
    if (pt/m_GeV<15) pt=15.0*m_GeV;
    if (pt/m_GeV>=2500) pt=2499.9*m_GeV;
  }
  if (fabs(eta)>=4.5) {
   if (++nWarn<10) 
     Warning("JESUncertaintyProvider::readPtEtaHisto()","jet eta=%.2f outside |eta|<4.5. Using closest valid value. \n (only first 10 messages will be printed). ",eta);
     eta=eta>=0?4.499:-4.499;
  }
  return h2d->Interpolate(pt/m_GeV,eta);
}

TH2 *JESUncertaintyProvider::MergeSubcomponents(StrV subComponents, TString name, TString suffix, TFile *file, bool isCorrelated) {
  StrV compVec;
  for (unsigned i=0;i<subComponents.size();++i) {
    TString compName=subComponents[i];
    compName.ReplaceAll(" ",""); // cleanup
    if (compName.Contains("FROM")) {
      int start=compName.Index("FROM"), mid=compName.Index("TO"), end=compName.Sizeof();
      int starti=atol(compName(start+4,mid-start-4).Data());
      int endi=atol(compName(mid+2,end-mid-3).Data());
      if (endi<=starti)
	Fatal("JESUncertaintyProvider::MergeSubcomponents","Can not interpret %s",compName.Data());
      compName.ReplaceAll(Form("FROM%dTO%d",starti,endi),"");
      for (int i=starti;i<=endi;++i) {
	compVec.push_back(compName+Form("%d",i));
      }
    } else {
      compVec.push_back(compName);
    }
  }
  if (compVec.size()==0)
    Fatal("JESUncertaintyProvider::MergeSubcomponents","No components?");
  
  TH2 *totUnc = MakeEmptyPtEtaHisto(name);
  for (unsigned i=0;i<compVec.size();++i) {
    TH2 *uncComp = GetHisto(file,compVec[i]+suffix,
			    "JESUncertaintyProvider::MergeSubcomponents()");
    if (!isCorrelated) AddInQuad(uncComp,totUnc);
    else Add(uncComp,totUnc);
  }
  return totUnc;
}

double JESUncertaintyProvider::getRelUncertUncorr(double pT, double eta) {
  return readPtEtaHisto(m_uncert,pT,eta);
}

double JESUncertaintyProvider::getRelUncertBias(double pT, double eta) {
  return readPtEtaHisto(m_corrUncert,pT,eta);
}

int JESUncertaintyProvider::getComponentIndex(TString compName) {
  if (m_compIndex.find(compName)==m_compIndex.end())
    Fatal("JESUncertaintyProvider::getComponentIndex()",
	  "ERROR: No such uncertainty component: %s",compName.Data());
  return m_compIndex[compName];
}

compCategory JESUncertaintyProvider::getCategoryEnumFromString(TString category)
{
  // Start with UNKNOWN as default
  compCategory cat = cat_UNKNOWN;
  if (category.CompareTo("statistical",TString::kIgnoreCase)==0)    cat = cat_stat;
  else if (category.CompareTo("detector",TString::kIgnoreCase)==0)  cat = cat_detector;
  else if (category.CompareTo("modelling",TString::kIgnoreCase)==0) cat = cat_model;
  else if (category.CompareTo("mixed",TString::kIgnoreCase)==0)     cat = cat_mixed;
  else if (category.CompareTo("special",TString::kIgnoreCase)==0)   cat = cat_special;
  
  if ( (cat == cat_UNKNOWN) && (! category.CompareTo("unknown",TString::kIgnoreCase)==0) )
    Fatal("JESUncertaintyProvider::getComponentNamesInCategory()",
	  "ERROR: No such category: %s",category.Data());

  return cat;
}

bool JESUncertaintyProvider::categoryExists(compCategory category)
{
  for (unsigned int i = 0; i < m_compCategories.size(); i++)
    if (m_compCategories[i] == category)
      return true;
  return false;
}



bool JESUncertaintyProvider::checkBinning(const TH2 *in, const TH2 *tot) {
  
  /*
  for (int i=1;i<=in->GetNbinsX()+1;++i) {
    double x=in->GetXaxis()->GetBinLowEdge(i);
    int bin=tot->GetXaxis()->FindBin(x+1e-5);
    if (fabs(x-tot->GetXaxis()->GetBinLowEdge(bin))>1e-3) 
      Fatal("JESUncertaintyProvider::checkBinning","Bad binning at pT=%.2f for histo %s",x,in->GetName());
  }
  for (int i=1;i<=in->GetNbinsY()+1;++i) {
    double x=in->GetYaxis()->GetBinLowEdge(i);
    int bin=tot->GetYaxis()->FindBin(x+1e-5);
    if (fabs(x-tot->GetYaxis()->GetBinLowEdge(bin))>1e-3) 
      Fatal("JESUncertaintyProvider::checkBinning","Bad binning at eta=%.2f for histo %s",x,in->GetName());
  }
  */

  // New method which doesn't require identically binned histograms
  // (saves space to have components that are flat in eta as a single eta bin
  // Low edge of low bin and high edge of high bin (== low edge of overflow bin) should be the same within tolerance
  float tolerance = 1e-6;
  
  // Check pt binning
  float lowEdge1  =  in->GetXaxis()->GetBinLowEdge(1);
  float lowEdge2  = tot->GetXaxis()->GetBinLowEdge(1);
  float highEdge1 =  in->GetXaxis()->GetBinLowEdge(in->GetNbinsX()+1);
  float highEdge2 = tot->GetXaxis()->GetBinLowEdge(tot->GetNbinsX()+1);
  if ( fabs(lowEdge1 - lowEdge2) > tolerance || fabs(highEdge1 - highEdge2) > tolerance )
    Fatal("JESUncertaintyProvider::checkBinning","Bad pt binning of [%.2f,%.2f] for histo %s, expected [%.2f,%.2f]",lowEdge1,highEdge1,in->GetName(),lowEdge2,highEdge2);

  // Now check eta binning
  lowEdge1  =  in->GetYaxis()->GetBinLowEdge(1);
  lowEdge2  = tot->GetYaxis()->GetBinLowEdge(1);
  highEdge1 =  in->GetYaxis()->GetBinLowEdge(in->GetNbinsY()+1);
  highEdge2 = tot->GetYaxis()->GetBinLowEdge(tot->GetNbinsY()+1);
  if ( fabs(lowEdge1 - lowEdge2) > tolerance || fabs(highEdge1 - highEdge2) > tolerance )
    Fatal("JESUncertaintyProvider::checkBinning","Bad eta binning of [%.2f,%.2f] for histo %s, expected [%.2f,%.2f]",lowEdge1,highEdge1,in->GetName(),lowEdge2,highEdge2);


  return true;
}

void JESUncertaintyProvider::Add(TH2 *in, TH2 *tot) {
  checkBinning(in,tot);
  for (int ipt=1;ipt<=tot->GetNbinsX();++ipt)
    for (int ieta=1;ieta<=tot->GetNbinsY();++ieta) {
      double pt=tot->GetXaxis()->GetBinCenter(ipt);
      double eta=tot->GetYaxis()->GetBinCenter(ieta);
      tot->SetBinContent(ipt,ieta,in->Interpolate(pt,eta)+tot->GetBinContent(ipt,ieta));
    }
}

void JESUncertaintyProvider::AddInQuad(TH2 *in, TH2 *tot) {
  checkBinning(in,tot);
  for (int ipt=1;ipt<=tot->GetNbinsX();++ipt)
    for (int ieta=1;ieta<=tot->GetNbinsY();++ieta) {
      double pt=tot->GetXaxis()->GetBinCenter(ipt);
      double eta=tot->GetYaxis()->GetBinCenter(ieta);
      double uin=in->Interpolate(pt,eta), uold=tot->GetBinContent(ipt,ieta);
      tot->SetBinContent(ipt,ieta,sqrt(uin*uin+uold*uold));
    }
}



bool JESUncertaintyProvider::isIntercalibrationComponent (const TString & component) {
  return component.Contains("etaIntercalibration", TString::kIgnoreCase);
}

bool JESUncertaintyProvider::isInSituComponent (const TString & component) {
  return ( component.Contains("InSitu", TString::kIgnoreCase) || 
	   component.Contains("Zjet", TString::kIgnoreCase) || 
	   component.Contains("MPF", TString::kIgnoreCase) ||
           component.Contains("Gjet", TString::kIgnoreCase) ||
	   component.Contains("MJB", TString::kIgnoreCase) );
}

bool JESUncertaintyProvider::isNonClosureComponent (const TString & component) {
  return ( component.Contains("NonClosure", TString::kIgnoreCase) );
}

bool JESUncertaintyProvider::isPileupComponent (const TString & component) {
  return ( component.Contains("Pileup", TString::kIgnoreCase) );
}

bool JESUncertaintyProvider::isEffectiveComponent(const TString & component)
{
  return component.Contains("EffectiveNP",TString::kIgnoreCase);
}







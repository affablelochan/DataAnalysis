#include "JetUncertainties/MultijetJESUncertaintyProvider.h"
#include "TROOT.h"
#include "TEnv.h"
#include "TIterator.h"
#include "TKey.h"
#include <cstdlib>
#include <map>
#include <sstream>

// Constructor
MultijetJESUncertaintyProvider::MultijetJESUncertaintyProvider(TString multijetConfig,
							       TString configFile,
							       TString jetAlgo,
							       TString MCtype, TString path):
  JESUncertaintyProvider(configFile,jetAlgo,MCtype,path),
  m_multijetConfig(),
  m_flavorGlu(NULL),
  m_flavorLight(NULL),
  m_closeBy(NULL),
  m_gluonResponseDifference(NULL),
  m_bJESResponse(NULL), 
  m_gluonFraction(),
  m_gluonFractionError(),
  m_useNjetBins(false),
  m_multiIsInit(false),
  m_relGluonResponseUncert(-1),
  m_maxNumJets(0)
{
  initMultijet(multijetConfig);
}

//PROOF
MultijetJESUncertaintyProvider::MultijetJESUncertaintyProvider():
  m_multijetConfig(),
  m_flavorGlu(NULL),
  m_flavorLight(NULL),
  m_closeBy(NULL),
  m_gluonResponseDifference(NULL),
  m_bJESResponse(NULL), 
  m_gluonFraction(),
  m_gluonFractionError(),
  m_useNjetBins(false),
  m_multiIsInit(false),
  m_relGluonResponseUncert(-1),
  m_maxNumJets(0)
{ }

// Destructor
MultijetJESUncertaintyProvider::~MultijetJESUncertaintyProvider()
{
  //delete histograms, we own them
  if(m_flavorGlu) delete m_flavorGlu;
  if(m_flavorLight) delete m_flavorLight;  
  if(m_closeBy) delete m_closeBy;
  if(m_gluonResponseDifference) delete m_gluonResponseDifference;
  if(m_bJESResponse) delete m_bJESResponse;
  for ( unsigned int iNJet = 0; iNJet<m_gluonFraction.size(); iNJet++) {
     if(m_gluonFraction[iNJet]) delete m_gluonFraction[iNJet];
     if(m_gluonFractionError[iNJet]) delete m_gluonFractionError[iNJet];
  }
}

void MultijetJESUncertaintyProvider::initMultijet(TString multijetConfig)
{
  //cache current directory 
  TDirectory* curdir = gDirectory;  
  gROOT->cd(); 
  
  // Ensure that the base class has already been initialized
  // If not, then fatal (*need* base class now that mctype is required for closeby uncertainty)
  if (! m_isInit )
    Fatal("MultijetJESUncertaintyProvider::init()","Base class is not initialized - please call initJES first!");
  TString mcType=getMCtype();

  // Prevent multiple initializations 
  if (m_multiIsInit == false) {

    m_multijetConfig = m_path+multijetConfig;

    // this has already been checked
    int jetR = m_jetAlgo.Contains("Kt6") ? 6 : 4;
    TString calib=m_jetAlgo.Contains("LC") ? "LCJES" : "EMJES";

    Info( "MultijetJESUncertaintyProvider::init()", "======================================================" );
    Info( "MultijetJESUncertaintyProvider::init()", " Initializing the MultijetJESUncertaintyProvider tool");
    Info( "MultijetJESUncertaintyProvider::init()","  Configuration read in from:" );
    Info( "MultijetJESUncertaintyProvider::init()","    %s",m_multijetConfig.Data());
    Info( "MultijetJESUncertaintyProvider::init()", "======================================================" );
    
    TEnv settings;
    //this returns -1 if unsuccessful
    int status = settings.ReadFile(m_multijetConfig.Data(),EEnvLevel(0));
    
    if (status!=0) Fatal("MultijetJESUncertaintyProvider::init()","Cannot read file %s",m_multijetConfig.Data());

    // MJES input file name
    TString multijetFN = settings.GetValue("MultijetJESUncertaintyRootFile","MJESUncertainty.root");

    // Analysis input file name
    TString analysisFN = settings.GetValue("AnalysisRootFile","analysisFiles/UnknownFlavourComp.root");

    // Open the input files
    TFile *multijet_file = openInputFile(multijetFN);
    if (multijet_file == NULL) 
      Fatal( "MultijetJESUncertaintyProvider::init()", "ERROR: Cannot open file %s",multijetFN.Data());
    TFile* analysis_file = multijet_file;
    if ( analysisFN!="" ) {
      analysis_file = openInputFile(analysisFN);
      if (analysis_file==NULL)
	Fatal( "MultijetJESUncertaintyProvider::init()", "ERROR: Cannot open file %s",analysisFN.Data());
    }

    TString suffix=Form("_AntiKt%dTopo_",jetR)+calib;
    
    int index=0;
    // Now check which MJES components we are interested in
    for (int ci=0;ci<100;++ci) {
      
      //easier to printf-style this (thanks Dag)
      TString prefix = Form("MultijetJESComponent.%d.",ci);
      
      // Read in the name and the description of the uncertainty component
      TString compName = settings.GetValue(prefix+"Name","");
      TString desc = settings.GetValue(prefix+"Desc","");
      
      compName.ReplaceAll(" ","");
      if (compName=="") continue;
      index++;

      //set the flags for the two multijet components
      if ( compName.Contains("Flavor_Comp",TString::kIgnoreCase) || 
	   compName.Contains("Flavour_Comp",TString::kIgnoreCase)) {

	//m_FlavorCompName = inputName;
	Info( "MultijetJESUncertaintyProvider::init()", " %2d. %-15s : %s",index,compName.Data(),desc.Data()); 

	// Gluon flavor composition graph
	m_flavorGlu   = (TH2D*)GetHisto(multijet_file,"flavorCompGlu"+suffix,"MultijetJESUncertaintyProvider::init()");
	m_flavorLight = (TH2D*)GetHisto(multijet_file,"flavorCompLight"+suffix,"MultijetJESUncertaintyProvider::init()");

	// Some magic needed for PROOF
	m_flavorGlu->SetName(TString("the")+"flavorCompGlu"+suffix); m_flavorGlu->SetDirectory(0);
	m_flavorLight->SetName(TString("the")+"flavorCompLight"+suffix); m_flavorLight->SetDirectory(0);
	
	// Analysis sample graphs

	// The user may provide nJet specific histograms, or they may not
        // (gluon fraction and gluon fraction error histograms)
        // If they do, we want to use only the binned histograms
        // If not, we want to use the njet specific histograms
        // Only way to check is to check the key names in the file
        // Load general histograms if this fails to find any nJet histos

        m_useNjetBins = false;

        // Input should contain nJets from 0 to N, but user may forget to include all histos
        // Keep a multimap of njet histograms read (adapted from suggestion by Will Bell, March 26 2013)
        std::multimap<size_t, std::string> gluonFractionNames;
        std::multimap<size_t, std::string> gluonFractionErrorNames;
        TIter nextkey(analysis_file->GetListOfKeys());
        TKey *key;

        while ( (key = (TKey*)nextkey()) )
        {
          std::string histoName = key->GetName();
          size_t nJetDigitPosition = histoName.find("njet");

          // Found an njet histogram
          if (nJetDigitPosition != std::string::npos)
          { 
            std::string nJetDigit = histoName.substr(nJetDigitPosition+4,histoName.length()); // +4 for "njet".length()
            size_t numJet;
            std::stringstream(nJetDigit) >> numJet; // get a size_t from a string

            // gluon fraction error or gluon fraction histogram?
            if (histoName.find("gluonFractionError") != std::string::npos)
            {
              // Check if the histogram is already in the multimap
              if (gluonFractionErrorNames.find(numJet) != gluonFractionErrorNames.end())
                Fatal("MultijetJESUncertaintyProvider::init()","Already have a gluon fraction error histogram for njets = %zu",numJet);
              gluonFractionErrorNames.insert(std::pair<size_t,std::string>(numJet,histoName));
            }
            else if (histoName.find("gluonFraction") != std::string::npos)
            {
              // Check if the histogram is already in the multimap
              if (gluonFractionNames.find(numJet) != gluonFractionNames.end())
                Fatal("MultijetJESUncertaintyProvider::init()","Already have a gluon fraction histogram for njets = %zu",numJet);
              gluonFractionNames.insert(std::pair<size_t,std::string>(numJet,histoName));
            }
          }
        }
        
        // If we found any njet histograms, then we are not using njet specific histograms
        if (gluonFractionNames.size() > 0 || gluonFractionErrorNames.size() > 0)
          m_useNjetBins = true;

        // Ensure that we always have pairs of histograms (gluonFraction,gluonFractionError)
        // We don't want to be in cases where we only have one of the two
        // Also ensure that they run from 0 to size()-1
        if (m_useNjetBins)
        {
          if (gluonFractionNames.size() != gluonFractionErrorNames.size())
            Fatal("MultijetJESUncertaintyProvider::init()","Require an equal number of histograms for gluon fraction and gluon fraction error, but got %zu and %zu respectively",gluonFractionNames.size(),gluonFractionErrorNames.size());

          // Same size, now compare key-by-key
          // As they are the same size, if they differ then the first map will have an element not in the second map
          // No need to check both maps
          std::multimap<size_t,std::string>::const_iterator iter;
          for (iter = gluonFractionNames.begin(); iter != gluonFractionNames.end(); ++iter)
            if (gluonFractionErrorNames.find((*iter).first) == gluonFractionErrorNames.end())
              Fatal("MultijetJESUncertaintyProvider::init()","Found key njet %zu for gluon fraction, but not for gluon fraction errors map",(*iter).first);

          // Same keys, so now ensure that the keys range from 0 to size()-1
          // Get the last element, which has the highest number of jets
          size_t largest = gluonFractionNames.rbegin()->first;
          if (gluonFractionNames.size() != largest+1)
            Fatal("MultijetJESUncertaintyProvider::init()","Not enough histograms for number of jets requested (%zu found, %zu requested)",gluonFractionNames.size(),largest+1);

          // All good!  Set the maximum number of jets
          // We're done with the maps now - they've served their purpose
          // From here, do everything with respect to the max number of jets
          m_maxNumJets = largest;
        }


        // Now load the histograms
        // If not using njet histograms, then load the general histograms
        // Otherwise, load only the njet histograms
        if (!m_useNjetBins)
        {
          // Inclusive gluon fraction/gluon fraction uncertainty 2D histo
	  m_gluonFraction.push_back( (TH2D*)GetHisto(analysis_file,"gluonFraction"+suffix,"MultijetJESUncertaintyProvider::init()") );
	  m_gluonFraction[0]->SetName(TString("the")+"gluonFraction"+suffix); m_gluonFraction[0]->SetDirectory(0);
          m_gluonFractionError.push_back( (TH2D*)GetHisto(analysis_file,"gluonFractionError"+suffix,"MultijetJESUncertaintyProvider::init()") );
          m_gluonFractionError[0]->SetName(TString("the")+"gluonFractionError"+suffix); m_gluonFractionError[0]->SetDirectory(0);
        }
        else
        {
          for (size_t iJet = 0; iJet <= m_maxNumJets; iJet++)
          {
            TString njetstring = Form("_njet%zu",iJet);
            
            m_gluonFraction.push_back( (TH2D*)GetHisto(analysis_file,"gluonFraction"+suffix+njetstring,"MultijetJESUncertaintyProvider::init()") );
            m_gluonFraction[iJet]->SetName(TString("the")+"gluonFraction"+suffix+njetstring); 
            m_gluonFraction[iJet]->SetDirectory(0);
       
            m_gluonFractionError.push_back( (TH2D*)GetHisto(analysis_file,"gluonFractionError"+suffix+njetstring,"MultijetJESUncertaintyProvider::init()") );
            m_gluonFractionError[iJet]->SetName(TString("the")+"gluonFractionError"+suffix+njetstring); 
            m_gluonFractionError[iJet]->SetDirectory(0);

	    Info( "MultijetJESUncertaintyProvider::init()", "       Retrieved iJet = %zu histograms",iJet);
          }
        }


        if (!m_useNjetBins) {
          
          Warning( "MultijetJESUncertaintyProvider::init()", " No Njet-dependent flavour uncertainty available. You will not be able to use the nJets argument of the getRelUncert functions. ");         
        
        }
        
//     
//         // Try to read in the 1-jet
//         TString njetstring = Form("_njet%d",1);
//         TH2D* histoGluonFractionNjet = (TH2D*)GetHisto(analysis_file,"gluonFraction"+suffix+njetstring,"MultijetJESUncertaintyProvider::init()");
//         
//         if ( histoGluonFractionNjet == NULL ) {
//           Warning( "MultijetJESUncertaintyProvider::init()", " No Njet-dependent flavour uncertainty available. You will not be able to use the nJets argument of the getRelUncert functions. ");         
//         }
//         else {
//           
//           //push back the 1-jet        
//           m_gluonFraction.push_back( histoGluonFractionNjet  );
//           
//           //loop on the others up to 7 jets
//           for(int i=2; i<8; i++){
//             njetstring = Form("_njet%d",i);
//             TH2D* h = (TH2D*)GetHisto(analysis_file,"gluonFraction"+suffix+njetstring,"MultijetJESUncertaintyProvider::init()");
//             
//             Fatal("MultijetJESUncertaintyProvider::init()","Don't know how to interpret %s",compName.Data());
// 
//             if(!h) std::cout << " MultijetJESUncertaintyProvide::initMultijet: ERROR input histogramm can't be found!" << std::endl;
//             //      std::cout << "MJES: read histogram " << h->GetName() << std::endl; 
//             h->SetName(TString("the")+"gluonFraction"+suffix+njetstring); 
//             h->SetDirectory(0);
//             m_gluonFraction.push_back(h);
//           }
//           
//         }
//         
        
        
        
	// Pull the correct gluon fraction error 2D histo
//	m_gluonFractionError.push_back( (TH2D*)GetHisto(analysis_file,"gluonFractionError"+suffix,"MultijetJESUncertaintyProvider::init()") );
//	m_gluonFractionError[0]->SetName(TString("the")+"gluonFractionError"+suffix); m_gluonFractionError[0]->SetDirectory(0);

        // Pull the correct gluon fraction 2D histo     
//         for(int i=0; i<8; i++){
//           TString njetstring = Form("_njet%d",i);
//           TH2D* h = (TH2D*)GetHisto(analysis_file,"gluonFraction"+suffix+njetstring,"MultijetJESUncertaintyProvider::init()");
//           if(!h) std::cout << " MultijetJESUncertaintyProvide::initMultijet: ERROR input histogramm can't be found!" << std::endl;
//           //      std::cout << "MJES: read histogram " << h->GetName() << std::endl; 
//           h->SetName(TString("the")+"gluonFraction"+suffix+njetstring); 
//           h->SetDirectory(0);
//           m_gluonFraction.push_back(h);
// 
//           // Pull the correct gluon fraction error 2D histo
//           TH2D* he = (TH2D*)GetHisto(analysis_file,"gluonFractionError"+suffix+njetstring,"MultijetJESUncertaintyProvider::init()");
//           he->SetName(TString("the")+"gluonFractionError"+suffix+njetstring); 
//           he->SetDirectory(0);
//           m_gluonFractionError.push_back(he); 
//         }

	// Pull the correct response graph
// 	m_responseSample = (TH2D*)GetHisto(analysis_file,"rSample"+suffix,"MultijetJESUncertaintyProvider::init()");
// 	m_responseSample->SetName(TString("the")+"rSample"+suffix); m_responseSample->SetDirectory(0);

      }
      else if ( compName.Contains("Close_By",TString::kIgnoreCase)) {

	Info( "MultijetJESUncertaintyProvider::init()", " %2d. %-15s : %s",index,compName.Data(),desc.Data()); 

	// Pull the correct deltaR graph
        // Note there may be MCTYPE dependence - have to check
        TString closebySuffix = compName.ReplaceAll("MCTYPE",mcType).ReplaceAll("Close_By","") + suffix;
	m_closeBy = (TH2D*)GetHisto(multijet_file,"CloseBy"+closebySuffix,"MultijetJESUncertaintyProvider::init()");
    	m_closeBy->SetName(TString("the")+"CloseBy"+suffix); m_closeBy->SetDirectory(0);
        
        //issue a warning that things have changed wrt 2011
        if (m_is2012Moriond) {
          Info( "MultijetJESUncertaintyProvider::init()", "The close-by uncertainty for 2012 needs to be called with a new argument wrt 2011 (frac_closeby)"); 
          Info( "MultijetJESUncertaintyProvider::init()", "Further instructions on: https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/JetUncertainties2012#Flavor_and_topology_uncertaintie"); 
        }
	
      }
      else if (compName.Contains("Flavor_Response",TString::kIgnoreCase) || 
	       compName.Contains("Flavour_Response",TString::kIgnoreCase)) {
	
	m_relGluonResponseUncert = settings.GetValue("RelativeGluonResponseUncertainty",0.02);
	Info( "MultijetJESUncertaintyProvider::init()", " %2d. %-15s : %s",index,compName.Data(),desc.Data()); 

	//Pull the correct flavor response graph (Herwig-Pythia gluon response)
	m_gluonResponseDifference = (TH2D*)GetHisto(multijet_file,"FlavorResponse"+suffix,"MultijetJESUncertaintyProvider::init()");
	m_gluonResponseDifference->SetName(TString("the")+"FlavorResponse"+suffix); m_gluonResponseDifference->SetDirectory(0);
	
      }
      else if ( compName.Contains("bJES_Response",TString::kIgnoreCase)) {
              
        Info( "MultijetJESUncertaintyProvider::init()", " %2d. %-15s : %s",index,compName.Data(),desc.Data()); 
              
        // Pull the correct bJES graph
        m_bJESResponse = (TH2D*)GetHisto(multijet_file,"bJES"+suffix,"MultijetJESUncertaintyProvider::init()");
        m_bJESResponse->SetName(TString("the")+"bJES"+suffix); m_bJESResponse->SetDirectory(0);
                                      
      }
                  
      else 
	Fatal("MultijetJESUncertaintyProvider::init()","Don't know how to interpret %s",compName.Data());
    }
    
    if (analysis_file) {
      analysis_file->Close();
      delete analysis_file;
    }
    
    multijet_file->Close();
    delete multijet_file;
    
    m_multiIsInit = true;

    Info( "MultijetJESUncertaintyProvider::init()", "======================================================");  
  }
  else {
    Warning( "MultijetJESUncertaintyProvider::init()", 
	     "WARNING: MultijetJESUncertaintyProvider already initialized, skipping re-initialization");
  }
  
  //go back to earlier directory 
  gDirectory = curdir;  
  
}

/*
 *  Method to access total uncertainty - excluding pile-up uncertainty
 */
double MultijetJESUncertaintyProvider::getRelUncert(double pT, double Eta, double dRminOrFracCloseby, bool isUp, bool isBJet, unsigned int nJets) {

  // Add the uncertainties in quadrature
    
  return sqrt( pow(JESUncertaintyProvider::getRelUncert(pT,Eta),2) +
               pow(getRelFlavorUncert(pT,Eta,isUp,nJets),2)*int(!isBJet) +
               pow(getRelClosebyUncert(pT,dRminOrFracCloseby),2) +
               pow(getRelBJESUncert(pT,Eta),2)*int(isBJet) );
  
  //this is equivalent to:
  /*if (!isBJet)
   *    return sqrt( pow(JESUncertaintyProvider::getRelUncert(pT,Eta),2) +
   *                 pow(getRelFlavorUncert(pT,Eta,isUp),2) +
   *                 pow(getRelClosebyUncert(pT,dRminOrFracCloseby),2) );
   *    
   *  if (isBJet)
   *    return sqrt( pow(JESUncertaintyProvider::getRelUncert(pT,Eta),2) +
   *                 pow(getRelClosebyUncert(pT,dRminOrFracCloseby),2) +
   *                 pow(getRelBJESUncert(pT,Eta),2)
   *               );
   */
  
}

/*
 *  Method to access total uncertainty - including pile-up uncertainty
 */
double MultijetJESUncertaintyProvider::getRelUncert(double pT, double Eta, double dRminOrFracCloseby, bool isUp, double NPV, double mu, bool isBJet, unsigned int nJets) {
  return sqrt( pow(getRelUncert(pT,Eta,dRminOrFracCloseby,isUp,isBJet,nJets),2) + pow(JESUncertaintyProvider::getRelOffsetUncert(pT,Eta,NPV,mu),2) );
}

/*
 *  The close-by uncertainty
 */
double MultijetJESUncertaintyProvider::getRelClosebyUncert(double pT, double dRminOrFracCloseby) {
  static TString method = "MultijetJESUncertaintyProvider::getRelClosebyUncert()";
  static int nbadclose = 0;
  
  if (m_closeBy==NULL) 
    Fatal("MultijetJESUncertaintyProvider::getRelClosebyUncert()","Close-by histogram not initialized");
  
  double minDRmin = m_closeBy->GetYaxis()->GetBinLowEdge(1);
  double maxDRmin = m_closeBy->GetYaxis()->GetBinLowEdge(m_closeBy->GetNbinsY()+1);
  
  // fCloseby: maximal at large values, minimal at small
  // dRmin: maximal at small values, minimal at large
  if (m_is2012Moriond)
  {
    if (dRminOrFracCloseby >= maxDRmin)
    {
      TString msg=Form("fCloseby > %.1f for jet of pT=%.1f GeV: fCloseby=%.2f. Setting fCloseby to %.1f.", 
          	     maxDRmin, pT/m_GeV, dRminOrFracCloseby, maxDRmin);
      if (++nbadclose==10) msg+=" Last warning!";
      if (nbadclose<=10) Warning(method,"%s",msg.Data());
      
      dRminOrFracCloseby = maxDRmin - 1e-6;
    }
    else if (dRminOrFracCloseby <= minDRmin)
      dRminOrFracCloseby = minDRmin + 1e-6;
  }
  else
  {
    if (dRminOrFracCloseby >= maxDRmin)
      dRminOrFracCloseby = maxDRmin - 1e-6;
    else if (dRminOrFracCloseby <= minDRmin)
    {
      TString msg=Form("dRMin < %.1f for jet of pT=%.1f GeV: dRMin=%.2f. Setting dRMin to %.1f.", 
          	     minDRmin, pT/m_GeV, dRminOrFracCloseby, minDRmin);
      if (++nbadclose==10) msg+=" Last warning!";
      if (nbadclose<=10) Warning(method,"%s",msg.Data());
      
      dRminOrFracCloseby = minDRmin + 1e-6;
    }
  }

  double pTMax = m_closeBy->GetXaxis()->GetBinUpEdge(m_closeBy->GetNbinsX())*m_GeV;
  double pTMin = m_closeBy->GetXaxis()->GetBinLowEdge(1)*m_GeV;//low edge of first bin
  
  if (pT>=pTMax) pT = pTMax-1e-6;
  if (pT<pTMin) pT = pTMin+1e-6;
  
  return m_closeBy->Interpolate(pT/m_GeV,dRminOrFracCloseby);
}



/*
 *  Below: Flavor composition and flavor response uncertainty method and
 *         helper functions
 */
double MultijetJESUncertaintyProvider::getRelFlavorUncert(double pT, double eta, bool isUp, unsigned int nJets) {
  
  //as this parameter is here for later use, let's silence the compiler warning about it being unused
  (void)isUp;
  return sqrt( pow(getRelFlavorCompUncert(pT,eta,true,nJets),2) + pow(getRelFlavorResponseUncert(pT,eta,nJets),2) );
}


/*
 *   Returns the flavor composition uncertainty using the formula:
 *     
 *   Uncertainty = df * | Rq - Rg | / ( fg * Rg + (1 - fg) * Rq )
 *   with 
 *     Rs = fg * Rg + (1 - fg) * Rq as heavy q uncertainties accounted separately
 *     df = error on fg
 *     fg = fraction of gluons
 *     Rq = light quark response
 *     Rg = gluon response
 */

double MultijetJESUncertaintyProvider::getRelFlavorCompUncert(double pT, double eta, bool isUp, unsigned int nJets) {
  
  //as this parameter is here for later use, let's silence the compiler warning about it being unused
  (void)isUp;

  static TString method = "MultijetJESUncertaintyProvider::getRelFlavorCompUncert()";

  // Must have at least one gluonFraction element
  if (!m_gluonFraction.size())
    Fatal(method.Data(),"The gluonFraction vector is empty.  Check your setup!");

  // Take overflow from highest bin
  if (nJets >= m_gluonFraction.size())
    nJets = m_gluonFraction.size()-1;

  // Watch for missing histograms
  if (m_flavorGlu == NULL || m_flavorLight == NULL)
    Fatal(method.Data(),"Flavour fractions not initialised.  Check your setup!");
  if (nJets >= m_gluonFraction.size() || nJets >= m_gluonFractionError.size())
    Fatal(method.Data(),"Gluon fractions vectors do no contain njet=%u.  Check your setup!",nJets);
  if (m_gluonFraction[nJets]==NULL || m_gluonFractionError[nJets]==NULL)
    Fatal(method.Data(),"Gluon fractions histograms for njet=%u have not been loaded.  Check your setup!",nJets);
  
  
  //calculating the sample response:
  //fg*Rg + (1-fg)*Rq
  double gluonFrac = gluonFraction(pT,eta,nJets);
  double Rg = gluonResponse(pT,eta);
  double Rq = lightQuarkResponse(pT,eta);
    
  double Rsample = gluonFrac * Rg + (1-gluonFrac) * Rq;
  
  //this should never happen (it means the Rg == Rq == 0), but checking anyway
  if (Rsample==0) Fatal(method,"R(sample) = 0 for pT=%.1f GeV, eta=%.2f",pT/m_GeV,eta);

  //calculating the uncertainty
  double gluonFracError =  gluonFractionError(pT,eta,nJets);
  double flavorCompUnc = gluonFracError*fabs(Rq-Rg)/Rsample;

  return flavorCompUnc;
  
}


/*
 *  Assumption
 *    dR(q) = JES uncertainty (measured in gamma/Z+jet)
 *    dR(g) = JES uncertainty (+) additional gluon response component
 *  component to be added to JES uncertainty: 
 *      fg*dR(gluon response modeling uncertainty)
 *      where gluon response modeling uncertainty is taken as difference between gluon response in Pythia and Herwig++
 */
double MultijetJESUncertaintyProvider::getRelFlavorResponseUncert(double pT, double eta, unsigned int nJets) {
  static TString method = "MultijetJESUncertaintyProvider::getRelFlavorResponseUncert()";
   
  // Must have at least one gluonFraction element
  if (m_gluonFraction.size() == 0)
    Fatal(method.Data(),"The gluonFraction vector is empty.  Check your setup!");

  // Take overflow from the highest bin
  if (nJets >= m_gluonFraction.size())
    nJets = m_gluonFraction.size()-1;

  // Watch for missing histograms
  if (m_gluonResponseDifference==NULL)
    Fatal(method.Data(),"Gluon response difference is null.  Check your setup!");
  if (nJets >= m_gluonFraction.size() || nJets >= m_gluonFractionError.size())
    Fatal(method.Data(),"Gluon fractions vectors do no contain njet=%u.  Check your setup!",nJets);
  if (m_gluonFraction[nJets]==NULL)
    Fatal(method.Data(),"The gluonFraction histogram for njet=%u has not been loaded.  Check your setup!",nJets);

  return gluonResponseDifference(pT,eta)*gluonFraction(pT,eta,nJets);
}


double MultijetJESUncertaintyProvider::getRelBJESUncert(double pT, double eta) {
  return bJESResponse(pT,eta);
}


double MultijetJESUncertaintyProvider::bJESResponse(double pT, double eta) {
  return getValue(m_bJESResponse,pT,eta);
}

double MultijetJESUncertaintyProvider::gluonResponseDifference(double pT, double eta) {
  return getValue(m_gluonResponseDifference,pT,eta);
}

double MultijetJESUncertaintyProvider::lightQuarkResponse(double pT, double eta) {
  return getValue(m_flavorLight,pT,eta);
}

double MultijetJESUncertaintyProvider::gluonResponse(double pT, double eta) {
  return getValue(m_flavorGlu,pT,eta);
}

double MultijetJESUncertaintyProvider::gluonFraction(double pT, double eta, unsigned int nJets) {
  
  if (m_useNjetBins && nJets > m_maxNumJets) nJets = m_maxNumJets;
  else if (nJets > m_maxNumJets) nJets = 0;

  TH2D* hGluonFraction = m_gluonFraction.at(nJets);
  if(!hGluonFraction) Fatal("MultijetJESUncertaintyProvider::gluonFraction", "Gluon fraction histogram not found for nJets=%d", nJets);
  return getValue(hGluonFraction,pT,eta);
  
}

double MultijetJESUncertaintyProvider::gluonFractionError(double pT, double eta, unsigned int nJets) {

  if (m_useNjetBins && nJets > m_maxNumJets) nJets = m_maxNumJets;
  else if (nJets > m_maxNumJets) nJets = 0;
  
  TH2D* hGluonFractionError = m_gluonFractionError.at(nJets);
  if(!hGluonFractionError) Fatal("MultijetJESUncertaintyProvider::gluonFractionError", "Gluon fraction error histogram not found for nJets=%d", nJets);
  return getValue(hGluonFractionError,pT,eta);
  
}

// Extrapolates across bins in a 2D histo
double MultijetJESUncertaintyProvider::getValue(const TH2 *h, double pt, double eta) {
  static const char* method = "MultijetJESUncertaintyProvider::getValue";
  static const char* msg = "Using closest valid value. (Only 10 first messages will be printed)";
  static int Nmsg = 0;
  double pT = pt/m_GeV;
  double minPt  = h->GetXaxis()->GetBinLowEdge(1);
  double maxPt  = h->GetXaxis()->GetBinLowEdge(h->GetNbinsX()+1);
  double maxEta = h->GetYaxis()->GetBinLowEdge(h->GetNbinsY()+1);
  if ( (pT<=minPt || pT>=maxPt) && ++Nmsg<=10 )
    Warning(method,"Jet pT=%.1f outside %.0f-%.0f range for %s.\n  %s",
	    pT,minPt,maxPt,h->GetName(),msg);
  if (pT<=minPt) pT=minPt+0.001*m_GeV;
  if (pT>=maxPt) pT=maxPt-0.001*m_GeV;
  if ( fabs(eta)>=maxEta && ++Nmsg<=10 )
    Warning(method,"Jet eta=%.1f outside |eta|<%.0f for %s.\n  %s",
	    eta,maxEta,h->GetName(),msg);
  if ( fabs(eta) >= maxEta ) 
    return ((TH2*)h)->Interpolate(pT,maxEta-1e-6);
  return ((TH2*)h)->Interpolate(pT,fabs(eta));
  
/*
  Using bins
  else
  {
    int ptbin = h->GetXaxis()->FindBin(pT);
    int etabin = h->GetYaxis()->FindBin(fabs(eta));
  
    if ( ptbin == 0 ) { ptbin=1;
      if (++nWarn<20) Warning(method,"Jet pT=%.1f GeV outside histogram range for %s. Using closest valid value.",
  			    pt/m_GeV,h->GetName());
    }
    if ( ptbin  > h->GetNbinsX()) { ptbin = h->GetNbinsX();
      if (++nWarn<20) Warning(method,"Jet pT=%.1f GeV outside histogram range for %s. Using closest valid value.",
  			    pt/m_GeV,h->GetName());
    }
    if ( etabin > h->GetNbinsY()) { etabin = h->GetNbinsY();
      if (++nWarn<20) Warning(method,"Jet eta=%.2f outside histogram range for %s. Using closest valid value.",
  			    eta,h->GetName());
    }
    return h->GetBinContent(ptbin,etabin);
  }
    */

}

// TH2 *MultijetJESUncertaintyProvider::MakeEmptyPtDRMinHisto(TString hname) {
//   //it is the responsibility of the user to delete this...
//   //Ŵorking on this next...
//   TH2* emptyDeltaRHisto = m_closeBy->Clone()
//   double ptbins[101], dx=(log(2500)-log(15))/100;
//   for (int i=0;i<=100;++i) ptbins[i]=exp(log(15)+i*dx);
//   double dRMinBins[] = {};//need to add sensible binning (using Interpolate earlier on, but no big deal up to 0.8/1.0)
//   int dRMinBins=sizeof(dRMinBins)/sizeof(double)-1;
//   return new TH2D(hname,"",100,ptbins,dRMinBins,dRMinBins);
// }

#include "BCHCleaningTool/BCHCleaningToolBase.h"
#include "TMath.h"
#include "TFile.h"

namespace BCHTool
{


//////////////////////////////////////////////////
// Constructors/destructors                     //
//////////////////////////////////////////////////

BCHCleaningToolBase::BCHCleaningToolBase()
    : m_isInit(false)
    , m_isData(false)
    , m_tileTripReader(NULL)
{
    // Support for default constructor
}


BCHCleaningToolBase::~BCHCleaningToolBase()
{
    if (m_isInit)
        m_tileTripReader = NULL;
}

//////////////////////////////////////////////////
// Tool initialization                          //
//////////////////////////////////////////////////

void BCHCleaningToolBase::InitializeTool(const bool isData, Root::TTileTripReader* tripReader, const std::string MCInefficiencyFileName)
{
    // Prevent double-initialization
    if (m_isInit)
        Terminate("BCHCleaningToolBase::InitializeTool","Tool was already initialized");
        
    // Check if TTR is NULL
    if (!tripReader)
        Terminate("BCHCleaningToolBase::InitializeTool","TTileTripReader tool passed is NULL");

    // Store data/MC switch
    m_isData = isData;
    
    // Store TTR and add boundaries around dead modules
    m_tileTripReader = tripReader;
    AddTileTripReaderBoundaries();

    // If MC, check that there is a file 
    if (!m_isData && MCInefficiencyFileName=="") 
        Terminate("BCHCleaningToolBase::InitializeTool","MC inefficiency file path not valid");
    
    SetMCInefficiencyHistograms(MCInefficiencyFileName);
    
    // Done, set initialization state to true
    m_isInit = true;
}


//////////////////////////////////////////////////
// Tool initialization helpers                  //
//////////////////////////////////////////////////

void BCHCleaningToolBase::AddTileTripReaderBoundaries()
{
    //set larger boundaries for trips: only for LB
    m_tileTripReader->m_LBOffsets.eta1=0.1;
    m_tileTripReader->m_LBOffsets.eta2=0.1;
    m_tileTripReader->m_LBOffsets.phi1=TMath::Pi()/32.;
    m_tileTripReader->m_LBOffsets.phi2=TMath::Pi()/32.;
}

void BCHCleaningToolBase::ResetTileTripReaderBoundaries()
{
    // Set back to default for discriminating between edge vs core
    m_tileTripReader->m_LBOffsets.eta1=0;
    m_tileTripReader->m_LBOffsets.eta2=0;
    m_tileTripReader->m_LBOffsets.phi1=0;
    m_tileTripReader->m_LBOffsets.phi2=0;
}

void BCHCleaningToolBase::SetMCInefficiencyHistograms(const std::string& MCInefficiencyFileName){
    
    // Get the path from the filename (possibly equivalent)
    const std::string MCInefficiencyPath = GetPathFromFileName(MCInefficiencyFileName);
    RequireFileToExist(MCInefficiencyPath, "BCHCleaningToolBase::SetMCInefficiencyHistograms");
    Info("BCHCleaningToolBase::SetMCInefficiencyHistograms",Form("Found inefficiency histograms at: %s",MCInefficiencyPath.c_str()));


    TFile * MCInefficiencyFile = TFile::Open(MCInefficiencyPath.c_str());
    //find the histogram we expect - for now hardwired, but chance for customization could be added
    m_h_inefficiency_lightq = GetHisto(MCInefficiencyFile, "FractionRejectedByCut_lightQ", "BCHCleaningToolBase::SetMCInefficiencyHistograms");  
    m_h_inefficiency_lightq->SetName(TString("theFractionRejectedByCut_lightQ")); // for PROOF compatibility
    m_h_inefficiency_lightq->SetDirectory(0);
    m_h_inefficiency_glu = GetHisto(MCInefficiencyFile, "FractionRejectedByCut_glu", "BCHCleaningToolBase::SetMCInefficiencyHistograms");  
    m_h_inefficiency_glu->SetName(TString("theFractionRejectedByCut_glu")); // for PROOF compatibility
    m_h_inefficiency_glu->SetDirectory(0);
    m_h_inefficiency_all = GetHisto(MCInefficiencyFile, "FractionRejectedByCut_all", "BCHCleaningToolBase::SetMCInefficiencyHistograms");  
    m_h_inefficiency_all->SetName(TString("theFractionRejectedByCut_all")); // for PROOF compatibility
    m_h_inefficiency_all->SetDirectory(0);
    
    MCInefficiencyFile->Close();
  
}

//////////////////////////////////////////////////////////////////
// Public methods to check dead regions and jet response        //
//////////////////////////////////////////////////////////////////

//bool BCHCleaningToolBase::IsInMaskedRegion(const int run, const int lbn, const Jet& jet)
//{
//    return IsInMaskedRegion(run,lbn,jet.eta(),jet.phi());
//}
//
//bool BCHCleaningToolBase::IsInMaskedRegion(const int run, const int lbn, const Jet* jet)
//{
//    return IsInMaskedRegion(run,lbn,*jet);
//}

bool BCHCleaningToolBase::IsInMaskedRegion(const int run, const int lbn, const double eta, const double phi)
{    
    if (!m_isInit)
        Terminate("BCHCleaningToolBase::IsInMaskedRegion","Tool is not initialized");

    // Ensure the passed run is sensible for MC
    // (zero if problem occurred in PRW's GetRandomRunNumber)
    if (!m_isData && !run)
        Terminate("BCHCleaningToolBase::IsInMaskedRegion","RunNumber is 0, most likely because the PileupReweighting tool failed to find a random data RunNumber with similar conditions for your value of mu.  The pileup weight should also be zero.  The user must watch for this case and handle it themselves, in order to ensure that groups not using weights do not miss this problem.  (A RunNumber of 0 will not provide a sensible treatment of masked tile modules.)");

    // Call the TTR
    return m_tileTripReader->checkEtaPhi(run,lbn,eta,phi);
}


bool BCHCleaningToolBase::IsBadLooseBCH(const int run, const int lbn, const double eta, const double phi, 
					const double BCH_CORR_CELL, const double emfrac, const double pt)
{
    
    // variable emfrac-BCH cut in core, fixed emfrac-BCH cut in edge
    if (IsInCore(run,lbn,eta,phi))
        return BadCoreCut(BCH_CORR_CELL,emfrac,pt);
    else if (IsInEdge(run,lbn,eta,phi))
        return BadEdgeCut(BCH_CORR_CELL,emfrac,pt);
    
    return false;
}

bool BCHCleaningToolBase::IsBadMediumBCH(const int run, const int lbn, const double eta, const double phi, 
					 const double BCH_CORR_CELL, const double emfrac, const double pt, const int isUp)
{
    (void)pt;
    // Geometric cut in core, fixed emfrac-BCH cut in edge
    if (IsInCore(run,lbn,eta,phi))
        return true;
    else if (IsInEdge(run,lbn,eta,phi))
        return BadEdgeCut(BCH_CORR_CELL,emfrac,pt,isUp);

    return false;
}

bool BCHCleaningToolBase::IsBadTightBCH(const int run, const int lbn, const double eta, const double phi, 
				        const double BCH_CORR_CELL, const double emfrac, const double pt)
{
    (void)BCH_CORR_CELL;
    (void)emfrac;
    (void)pt;
    // Geometric cut
    return IsInMaskedRegion(run,lbn,eta,phi);
}

bool BCHCleaningToolBase::IsBadLooseDeltaPhiJetMET(const int run, const int lbn, const double eta, const double phi, 
						   const double deltaPhiJetMET, const double emfrac, const double pt)
{
    // only apply those cuts above 50 GeV
    if (pt < 50000) return false;
    
    if (IsInCore(run,lbn,eta,phi))
        return BadCoreCut_DeltaPhi(deltaPhiJetMET,emfrac);
    else if (IsInEdge(run,lbn,eta,phi))
        return BadEdgeCut_DeltaPhi(deltaPhiJetMET,emfrac);
    
    return false;
}

bool BCHCleaningToolBase::IsBadMediumDeltaPhiJetMET(const int run, const int lbn, const double eta, const double phi, 
						   const double deltaPhiJetMET, const double emfrac, const double pt)
{
    // only apply those cuts above 50 GeV
    if (pt < 50000) return false;
    
    if (IsInCore(run,lbn,eta,phi))
        return true;
    else if (IsInEdge(run,lbn,eta,phi))
        return BadEdgeCut_DeltaPhi(deltaPhiJetMET,emfrac);
    
    return false;
}

bool BCHCleaningToolBase::IsBadTightDeltaPhiJetMET(const int run, const int lbn, const double eta, const double phi, 
						   const double deltaPhiJetMET, const double emfrac, const double pt)
{
    (void)deltaPhiJetMET;
    (void)emfrac;
    (void)pt;
    // Geometric cut
    return IsInMaskedRegion(run,lbn,eta,phi);
}


//////////////////////////////////////////////////
// Core and edge helpers and cuts               //
//////////////////////////////////////////////////

bool BCHCleaningToolBase::IsInCore(const int run, const int lbn, const double eta, const double phi)
{
    if (!m_isInit)
        Terminate("BCHCleaningToolBase::IsInMaskedRegion","Tool is not initialized");

    // Ensure the passed run is sensible for MC
    // (zero if problem occurred in PRW's GetRandomRunNumber)
    if (!m_isData && !run)
        Terminate("BCHCleaningToolBase::IsInMaskedRegion","RunNumber is 0, most likely because the PileupReweighting tool failed to find a random data RunNumber with similar conditions for your value of mu.  The pileup weight should also be zero.  The user must watch for this case and handle it themselves, in order to ensure that groups not using weights do not miss this problem.  (A RunNumber of 0 will not provide a sensible treatment of masked tile modules.)");
    
    // Remove edge boundaries, check TTR, and re-add edge boundaries
    // Not ideal, but for now don't want two copies of the TTR
    ResetTileTripReaderBoundaries();
    const bool isInCore = m_tileTripReader->checkEtaPhi(run,lbn,eta,phi);
    AddTileTripReaderBoundaries();

    return isInCore;
}

bool BCHCleaningToolBase::IsInEdge(const int run, const int lbn, const double eta, const double phi)
{
    return IsInMaskedRegion(run,lbn,eta,phi) && !IsInCore(run,lbn,eta,phi);
}

bool BCHCleaningToolBase::BadCoreCut(const double BCH_CORR_CELL, const double emfrac, const double pt)
{
    // Check for what should never happen
    if (pt <= 0)
        Terminate("BCHCleaningToolBase::InitializeTool",Form("Jet pt is <= 0 (got %f).  This should never happen.",pt));

    // Cut depends on pT
    const double scale = 1.e6; // 1 TeV
    return BCH_CORR_CELL < 0.02*emfrac+log10(scale/pt)*(1-emfrac)/8.;
}

bool BCHCleaningToolBase::BadEdgeCut(const double BCH_CORR_CELL, const double emfrac, const double pt, const int isUp)
{
  
  bool isBadEdge = false;
  
  if (m_isData) {
    // Cut 1: (BCH,emfrac) = (0.2,0) --> (0.1,1)
    const bool cut1 = BCH_CORR_CELL > (2 - emfrac)/10.;

    // Cut 2: (BCH,emfrac) = (0.5,0.2) --> (0,0.4)
    const bool cut2 = BCH_CORR_CELL < 1 - 2.5*emfrac;
    
    isBadEdge = (cut1 || cut2) ? true : false;
    
  }
  
  else {
   
    //draw a random number 
    double randomNumber= m_rand.Uniform();
    
    //get the probability of the event to be rejected, given the current jet pT and flavor
    double probabilityToBeRejected = 0;
    //central value: probability from QCD, all flavors
    if (isUp == 0) probabilityToBeRejected = m_h_inefficiency_all->Interpolate(pt/1e3);
    //downwards (more probability to be rejected) from quark jets
    if (isUp == -1) probabilityToBeRejected = m_h_inefficiency_lightq->Interpolate(pt/1e3);
    //upwards (less probability to be rejected) from quark jets
    if (isUp == 1) probabilityToBeRejected = m_h_inefficiency_glu->Interpolate(pt/1e3);
    
    //check whether the random number is below the threshold, in that case reject the event
    if (randomNumber < probabilityToBeRejected) isBadEdge = true;
    
  }
  
  return isBadEdge;
  
}

bool BCHCleaningToolBase::BadEdgeCut_DeltaPhi(const double deltaPhiJetMET, const double emfrac)
{
    //Cut: (DeltaPhiJetMET, emfrac) = (PI/2, 0) --> (PI, 0.45)
    //fEM = (0.45)/(PI/2)*deltaPhiJetMET - 0.45 = 0.45(deltaPhiJetMET/(PI/2)-1)
    const bool cut = (emfrac < 0.45*(deltaPhiJetMET/(TMath::Pi()/2)-1));     
    return cut;
}

bool BCHCleaningToolBase::BadCoreCut_DeltaPhi(const double deltaPhiJetMET, const double emfrac)
{

    //Cut: (DeltaPhiJetMET, emfrac) = (0, 0.4) --> (1.4, 1)
    //fEM = (0.3/0.7)*deltaPhiJetMET + 0.4
    const bool cut = (emfrac > (0.3/0.7)*deltaPhiJetMET + 0.4);
    return cut;
}


//////////////////////////////////////////////////
// Utility helper methods                       //
//////////////////////////////////////////////////

bool BCHCleaningToolBase::FileExists(const std::string& filePath) const
{
    // Weird convention...
    return gSystem->AccessPathName(filePath.c_str()) == false;
}

void BCHCleaningToolBase::RequireFileToExist(const std::string& filePath, const char* callingFunctionName)
{
    if (!FileExists(filePath))
        Terminate(callingFunctionName,Form("Specified file path does not exist \"%s\"",filePath.c_str()));
}

TH1* BCHCleaningToolBase::GetHisto(TFile *file, TString hname, TString method) {
  TH1 *h = (TH1*)file->Get(hname);
  if (h==NULL)
  Fatal(method.Data(),"Cannot access histo %s in file %s",hname.Data(),file->GetName());
  return h;
}

const std::string BCHCleaningToolBase::GetPathFromFileName(const std::string& fileName)
{
    // Identify the path to the specified file based on the location of:
    //      1. Current directory (taking the input string as is)
    //      2. Prepend "share"
    //      3. $ROOTCOREDIR/data/BCHCleaningTool (soft link to BCHCleaningTool/share)
    //      4. If Athena is set up ($TestArea exists), the standard athena location:
    //          $TestArea/Reconstruction/Jet/JetAnalysisTools/BCHCleaningTool/share

    // Sanity check
    if (fileName == "")
        Terminate("BCHCleaningToolBase::GetPathFromFileName","Input file name is an empty string");
    
    // Check current dir
    if (FileExists(fileName))
        return fileName;
    
    // Get just the file name string if the proposed directory structure (if any) didn't work
    // Then check both options
    const std::string fileNameNoDir = fileName.substr(fileName.find_last_of("\\/")+1);

    // Prepend share
    if (FileExists("share/"+fileName))
        return "share/"+fileName;
    else if (FileExists("share/"+fileNameNoDir))
        return "share/"+fileNameNoDir;

    // Check for RootCore
    std::string systemPath = gSystem->Getenv("ROOTCOREDIR");
    if (systemPath != "")
    {
        std::string expectedPath = systemPath+"/data/BCHCleaningTool/";
        if (FileExists(expectedPath+fileName))
            return expectedPath+fileName;
        else if (FileExists(expectedPath+fileNameNoDir))
            return expectedPath+fileNameNoDir;
        else
            Terminate("BCHCleaningToolBase::GetPathFromFileName",Form("RootCore is setup, but file is not in expected location: %s",(expectedPath+fileName).c_str()));
            
    }
    
    // Check for Athena
    systemPath = gSystem->Getenv("TestArea");
    if (systemPath != "")
    {
        std::string expectedPath = systemPath+"/Reconstruction/Jet/JetAnalsysisTools/BCHCleaningTool/share/";
        if (FileExists(expectedPath+fileName))
            return expectedPath+fileName;
        else if (FileExists(expectedPath+fileNameNoDir))
            return expectedPath+fileNameNoDir;
        else
            Terminate("BCHCleaningToolBase::GetPathFromFileName",Form("Athena is setup, but file is not in expected location: %s",(expectedPath+fileName).c_str()));
    }

    // Give up
    Terminate("BCHCleaningToolBase::GetPathFromFileName",Form("Could not find the specified file in any of the expected paths: %s",fileName.c_str()));

    return "";
}

} // End namespace BCHTool

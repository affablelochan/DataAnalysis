#ifndef BCHCLEANINGTOOLBASE_H
#define BCHCLEANINGTOOLBASE_H

// JetAnalysisEDM dual use athena/root include
//#include "JetAnalysisEDM/JetDualUseEDMInclude.h"

// Tool dependencies
#include "TileTripReader/TTileTripReader.h"

// Other dependencies
#include "TSystem.h"
#include "TRandom3.h"
#include "TH1.h"
#include "TFile.h"
#include "TString.h"
#include <string>
#include <vector>

namespace BCHTool
{

class BCHCleaningToolBase
{
    public:
        // Constructors/destructors
        virtual ~BCHCleaningToolBase();

        // Tool initialization
        virtual void InitializeTool(const bool isData, Root::TTileTripReader* tripReader, const std::string MCInefficiencyFileName="");

        // Check dead regions
        virtual bool IsInMaskedRegion(const int run, const int lbn, const double eta, const double phi);

        // Preliminary BCH cut methods
       
	virtual bool IsBadLooseBCH(const int run, const int lbn, const double eta, const double phi, 
				   const double BCH_CORR_CELL, const double emfrac, const double pt);
        virtual bool IsBadMediumBCH(const int run, const int lbn, const double eta, const double phi, 
				    const double BCH_CORR_CELL, const double emfrac, const double pt, const int isUp=0);
        virtual bool IsBadTightBCH(const int run, const int lbn, const double eta, const double phi,
				   const double BCH_CORR_CELL, const double emfrac, const double pt);
	
        virtual bool IsBadLooseDeltaPhiJetMET(const int run, const int lbn, const double eta, const double phi, 
					      const double deltaPhiJetMET, const double emfrac, const double pt);
        virtual bool IsBadMediumDeltaPhiJetMET(const int run, const int lbn, const double eta, const double phi, 
					       const double deltaPhiJetMET, const double emfrac, const double pt);
        virtual bool IsBadTightDeltaPhiJetMET(const int run, const int lbn, const double eta, const double phi,
					      const double deltaPhiJetMET, const double emfrac, const double pt);
        
	//random number handling
        void SetSeed(int seed=1234){m_rand.SetSeed(seed);};
        
    protected:
      
        BCHCleaningToolBase();
        
        // Control variables
        bool m_isInit;
        
        // Arguments and tool dependencies
        bool m_isData;
        Root::TTileTripReader* m_tileTripReader;
        
        // Initialization helpers
        void AddTileTripReaderBoundaries();
        void ResetTileTripReaderBoundaries();
	void SetMCInefficiencyHistograms(const std::string& MCInefficiencyFileName);

        // Core and edge helpers and cuts
        bool IsInCore(const int run, const int lbn, const double eta, const double phi);
        bool IsInEdge(const int run, const int lbn, const double eta, const double phi);
        bool BadCoreCut(const double BCH_CORR_CELL, const double emfrac, const double pt);
        bool BadEdgeCut(const double BCH_CORR_CELL, const double emfrac, const double pt, const int isUp=0);
        bool BadCoreCut_DeltaPhi(const double deltaPhiJetMET, const double emfrac);
        bool BadEdgeCut_DeltaPhi(const double deltaPhiJetMET, const double emfrac);

        // File access helpers
        bool FileExists(const std::string& filePath) const;
        void RequireFileToExist(const std::string& filePath, const char* callingFunctionName);
        TH1* GetHisto(TFile *file, TString hname, TString method);
        const std::string GetPathFromFileName(const std::string& fileName);

        // Throw an error or print a message - differs for athena or root
        virtual void Terminate(const char* caller, const char* message) = 0;
        virtual void Warning(const char* caller, const char* message) const = 0;
        virtual void Info(const char* caller, const char* message) const = 0;
	
        // Histograms for simulating inefficiency in MC
        TH1* m_h_inefficiency_lightq;
        TH1* m_h_inefficiency_glu;
        TH1* m_h_inefficiency_all;
        
        // Random number generator
        TRandom3 m_rand;       
};

} // End namespace BCHTool 

#endif

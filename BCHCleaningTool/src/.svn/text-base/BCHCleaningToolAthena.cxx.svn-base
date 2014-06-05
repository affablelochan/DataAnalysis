#include "BCHCleaningTool/BCHCleaningToolAthena.h"
#include "EventInfo/EventInfo.h"
#include "PathResolver/PathResolver.h"

namespace BCHTool
{

BCHCleaningToolAthena::BCHCleaningToolAthena(const std::string& type,
                                             const std::string& name,
                                             const IInterface* pParent)
    : BCHCleaningToolBase()
    , AthAlgTool(type,name,pParent)
    /*
    , m_inputIsMC(false)
    */
    , m_ttrHandle("AthTileTripReader",this)
{
    // Interface for retrieval via ToolSvc
    declareInterface<BCHTool::BCHCleaningToolAthena>(this);

    /*
    // Properties from jobOptions
    declareProperty("InputIsMC",m_inputIsMC=true,"Whether or not the input will be MC");
    */
}

BCHCleaningToolAthena::~BCHCleaningToolAthena()
{
    // nothing for now
}

StatusCode BCHCleaningToolAthena::initialize()
{
    
    // Retrieve the TTR tool
    if (m_ttrHandle.retrieve().isFailure())
    {
      msg(MSG::FATAL) << "Unable to retrieve TileTripReader! Giving up!" << endreq;
      return StatusCode::FAILURE;
    }
    msg(MSG::INFO) << "Successfully retrieved TileTripReader, name: " << m_ttrHandle->name() << endreq;
    
    
    /*
    // Initialize the underlying BCH tool
    // TODO possibly add this?
    msg(MSG::INFO) << "BCHCleaningToolAthena initialized successfully for " << (m_inputIsMC ? "MC" : "data") << ", name:" << name() << endreq;
    */
    msg(MSG::INFO) << "BCHCleaningToolAthena initialized successfully, name:" << name() << endreq;
    return StatusCode::SUCCESS;
}

StatusCode BCHCleaningToolAthena::finalize()
{
    return StatusCode::SUCCESS;
}


void BCHCleaningToolAthena::InitializeTool(const bool isData)
{
    const std::string emulationFile = PathResolver::find_file("FractionsRejectedJetsMC.root","DATAPATH");
    BCHCleaningToolBase::InitializeTool(isData,m_ttrHandle->getRootTool(),emulationFile);
}


// No longer works as PRW is not internal - need to get random run/lbn from user for MC
//bool BCHCleaningToolAthena::IsInMaskedRegion(const INavigable4Momentum* part)
//{
//    // Get the EventInfo object
//    const EventInfo* evtInfo = 0;
//    if ( (evtStore()->retrieve(evtInfo)).isFailure() || !evtInfo)
//        Terminate("BCHCleaningToolAthena::IsInMaskedRegion","Failed to retrieve EventInfo object from evtStore");
//    
//    // Get the EventID from EventInfo
//    const EventID* eid = evtInfo->event_ID();
//    if (!eid)
//        Terminate("BCHCleaningToolAthena::IsInMaskedRegion","Failed to retrieve EventID object from EventInfo object");
//
//    return IsInMaskedRegion(eid->run_number(),eid->lumi_block(),part->eta(),part->phi(),evtInfo->averageInteractionsPerCrossing());
//}

bool BCHCleaningToolAthena::IsInMaskedRegion(const int run, const int lbn, const INavigable4Momentum* part)
{
    return IsInMaskedRegion(run,lbn,part->eta(),part->phi());
}


void BCHCleaningToolAthena::Terminate(const char* caller, const char* message)
{
    msg(MSG::FATAL) << caller << ": " << message << endreq;
    sysStop();
}

void BCHCleaningToolAthena::Warning(const char* caller, const char* message) const
{
    msg(MSG::WARNING) << caller << ": " << message << endreq;
}

void BCHCleaningToolAthena::Info(const char* caller, const char* message) const
{
    msg(MSG::INFO) << caller << ": " << message << endreq;
}


} // End namespace BCHTool


///////////////////////// -*- C++ -*- /////////////////////////////
// TestBCH.cxx 
// Implementation file for class TestBCH
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 

// TestBCH includes
#include "TestBCH.h"
#include "PileupReweighting/TPileupReweighting.h"

// STL includes

// FrameWork includes
#include "GaudiKernel/Property.h"



/////////////////////////////////////////////////////////////////// 
// Public methods: 
/////////////////////////////////////////////////////////////////// 

// Constructors
////////////////
TestBCH::TestBCH( const std::string& name, ISvcLocator* pSvcLocator )
  : AthAlgorithm( name, pSvcLocator )
  , m_BCHTool("BCHTool::BCHCleaningToolAthena",this)
  //, m_ttr("AthTileTripReader",this)
  ////, m_ttr(NULL)
  //, m_prw(NULL)
{
  //
  // Property declaration
  // 
  //declareProperty( "Property", m_nProperty );
  //Root::TTileTripReader* ttr = (new AthTileTripReader("testType","testTTR",pSvcLocator))->getRootTool();

}

// Destructor
///////////////
TestBCH::~TestBCH()
{}

// Athena Algorithm's Hooks
////////////////////////////
StatusCode TestBCH::initialize()
{
  ATH_MSG_INFO ("Initializing " << name() << "...");

  // Retrieve the BCH tool
  if (m_BCHTool.retrieve().isFailure())
  {
    msg(MSG::FATAL) << "Unable to retrieve BCHCleaningTool!  Giving up!" << endreq;
    return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << "Successfully retrieved BCHCleaningTool, name: " << m_BCHTool->name() << endreq;
  
  //// Retrieve the TTR tool
  //if (m_ttr.retrieve().isFailure())
  //{
  //  msg(MSG::FATAL) << "Unable to retrieve TileTripReader! Giving up!" << endreq;
  //  return StatusCode::FAILURE;
  //}
  //msg(MSG::INFO) << "Successfully retrieved TileTripReader, name: " << m_ttr->name() << endreq;

  ////// Create the TTR tool
  ////m_ttr = new Root::TTileTripReader("ttrTool");
  ////m_ttr->setTripFile("/afs/cern.ch/user/s/sschramm/testarea/17.2.8.3-2/PhysicsAnalysis/TileID/TileTripReader/data/CompleteTripList_2011-2012.root");
  ////msg(MSG::INFO) << "Created TileTripReader tool" << endreq;

  // Create the PRW tool
  m_prw = new Root::TPileupReweighting("prwTool");
  m_prw->AddConfigFile("/afs/cern.ch/user/s/sschramm/testarea/17.2.8.3-2/PhysicsAnalysis/AnalysisCommon/PileupReweighting/share/mc12a_defaults.prw.root");
  m_prw->AddLumiCalcFile("/afs/cern.ch/user/s/sschramm/testarea/BCHtool/RootFilesForTests/ilumicalc_histograms_None_200841-204158.root");
  m_prw->Initialize();
  msg(MSG::INFO) << "Created PileupReweighting tool" << endreq;

  // Now initialize the BCH tool
  //m_BCHTool->InitializeTool(false,(dynamic_cast<AthTileTripReader*>(&(*m_ttr)))->getRootTool(),m_prw);
  //m_BCHTool->InitializeTool(false,m_ttr->getRootTool(),m_prw);
  //m_BCHTool->InitializeTool(false,m_ttr,m_prw);
  m_BCHTool->InitializeTool(false);
  msg(MSG::INFO) << "Initialized the BCHCleaningTool for MC" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode TestBCH::finalize()
{
  ATH_MSG_INFO ("Finalizing " << name() << "...");

  return StatusCode::SUCCESS;
}

StatusCode TestBCH::execute()
{  
  ATH_MSG_DEBUG ("Executing " << name() << "...");
  
  // Should be in a dead region
  msg(MSG::INFO) << "Should be in dead region (LBA05), got " << (m_BCHTool->IsInMaskedRegion(211787,25000,0.1,0.35) ? "true" : "false") << endreq;
  msg(MSG::INFO) << "Should not yet be in dead region (LBC16), got " << (m_BCHTool->IsInMaskedRegion(204264,100,-0.1,1.52) ? "true" : "false") << endreq;
  msg(MSG::INFO) << "Should now be in dead region (LBC16), got " << (m_BCHTool->IsInMaskedRegion(211521,100,-0.1,1.52) ? "true" : "false") << endreq;

  // TODO


  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Protected methods: 
/////////////////////////////////////////////////////////////////// 

/////////////////////////////////////////////////////////////////// 
// Const methods: 
///////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////// 
// Non-const methods: 
/////////////////////////////////////////////////////////////////// 



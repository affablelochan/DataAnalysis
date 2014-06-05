///////////////////////// -*- C++ -*- /////////////////////////////
// TestBCH.h 
// Header file for class TestBCH
// Author: S.Binet<binet@cern.ch>
/////////////////////////////////////////////////////////////////// 
#ifndef TESTBCH_TESTBCH_H
#define TESTBCH_TESTBCH_H 1

// STL includes
#include <string>

// FrameWork includes
#include "AthenaBaseComps/AthAlgorithm.h"

// For testing
#include "GaudiKernel/ToolHandle.h"
#include "BCHCleaningTool/BCHCleaningToolAthena.h"
#include "TileTripReader/TTileTripReader.h"
#include "TileTripReader/AthTileTripReader.h"
//#include "PileupReweighting/TPileupReweighting.h"


// Forward declare PRW
namespace Root
{
    class TPileupReweighting;
}

class TestBCH
  : public ::AthAlgorithm
{ 

  /////////////////////////////////////////////////////////////////// 
  // Public methods: 
  /////////////////////////////////////////////////////////////////// 
 public: 

  // Copy constructor: 

  /// Constructor with parameters: 
  TestBCH( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor: 
  virtual ~TestBCH(); 

  // Assignment operator: 
  //TestBCH &operator=(const TestBCH &alg); 

  // Athena algorithm's Hooks
  virtual StatusCode  initialize();
  virtual StatusCode  execute();
  virtual StatusCode  finalize();

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Private data: 
  /////////////////////////////////////////////////////////////////// 
 private: 

  /// Default constructor: 
  TestBCH();

  /// Containers
  

  // BCH tool
  //ToolHandle<IAlgTool> m_BCHTool;
  ToolHandle<BCHTool::BCHCleaningToolAthena> m_BCHTool;
  ////ToolHandle<IAthSelectorTool> m_ttr;
  //ToolHandle<AthTileTripReader> m_ttr;
  ////Root::TTileTripReader* m_ttr;
  Root::TPileupReweighting* m_prw;

}; 

// I/O operators
//////////////////////

/////////////////////////////////////////////////////////////////// 
// Inline methods: 
/////////////////////////////////////////////////////////////////// 


#endif //> !TESTBCH_TESTBCH_H

#ifndef BCHCLEANINGTOOLATHENA_H
#define BCHCLEANINGTOOLATHENA_H

// Base tool
#include "BCHCleaningTool/BCHCleaningToolBase.h"

// Athena dependencies
#include "AthenaBaseComps/AthAlgTool.h"
#include "EventInfo/EventID.h"
#include "JetEvent/Jet.h"

// Tool dependencies
#include "GaudiKernel/ToolHandle.h"
#include "TileTripReader/AthTileTripReader.h"






namespace BCHTool
{

static const InterfaceID IID_BCHTool_BCHCleaningToolAthena("BCHTool::BCHCleaningToolAthena",1,0); 

class BCHCleaningToolAthena : public BCHCleaningToolBase, 
                              public AthAlgTool
{
    public:
        static const inline InterfaceID& interfaceID() { return IID_BCHTool_BCHCleaningToolAthena; }

        BCHCleaningToolAthena( const std::string& type, const std::string& name,
                               const IInterface* pParent);
        virtual ~BCHCleaningToolAthena();
        
        virtual StatusCode initialize();
        virtual StatusCode finalize();
        
        using BCHCleaningToolBase::InitializeTool;
        virtual void InitializeTool(const bool isData);
        
        using BCHCleaningToolBase::IsInMaskedRegion;
        virtual bool IsInMaskedRegion(const int run, const int lbn, const INavigable4Momentum* part);

    protected:
        virtual void Terminate(const char* caller, const char* message);
        virtual void Warning(const char* caller, const char* message) const;
        virtual void Info(const char* caller, const char* message) const;

    private:
        ToolHandle<AthTileTripReader> m_ttrHandle;

};


} // End namespace BCHTool

#endif

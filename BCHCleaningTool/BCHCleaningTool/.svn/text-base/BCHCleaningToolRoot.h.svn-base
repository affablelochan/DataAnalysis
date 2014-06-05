#ifndef BCHCLEANINGTOOLROOT_H
#define BCHCLEANINGTOOLROOT_H

// Base tool
#include "BCHCleaningTool/BCHCleaningToolBase.h"

//// JetAnalysisEDM dependency
//#include "JetAnalysisEDM/EventObject.h"

namespace BCHTool
{

class BCHCleaningToolRoot : public BCHCleaningToolBase
{
    public:
        BCHCleaningToolRoot(const char * toolName="BCHCleaningToolRoot");
        virtual ~BCHCleaningToolRoot();
        
        const char* GetName() const;
        using BCHCleaningToolBase::IsInMaskedRegion;

    protected:
        virtual void Terminate(const char* caller, const char* message);
        virtual void Warning(const char* caller, const char* message) const;
        virtual void Info(const char* caller, const char* message) const;
        
        const TString m_toolName;

};

} // End namespace BCHTool

#endif

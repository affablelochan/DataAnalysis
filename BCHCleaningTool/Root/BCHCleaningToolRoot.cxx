#include "BCHCleaningTool/BCHCleaningToolRoot.h"

namespace BCHTool
{

BCHCleaningToolRoot::BCHCleaningToolRoot(const char* toolName)
    : m_toolName(toolName)
{
    // nothing for now
}

BCHCleaningToolRoot::~BCHCleaningToolRoot()
{
    // nothing for now
}

const char* BCHCleaningToolRoot::GetName() const
{
    return m_toolName.Data();
}

void BCHCleaningToolRoot::Terminate(const char* caller, const char* message)
{
    gSystem->Fatal(caller,message);
}

void BCHCleaningToolRoot::Warning(const char* caller, const char* message) const
{
    gSystem->Warning(caller,message);
}

void BCHCleaningToolRoot::Info(const char* caller, const char* message) const
{
    gSystem->Info(caller,message);
}

} // End namespace BCHTool


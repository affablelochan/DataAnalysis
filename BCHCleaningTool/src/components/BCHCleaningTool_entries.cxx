#include "BCHCleaningTool/BCHCleaningToolAthena.h"

#include "../TestBCH.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_NAMESPACE_TOOL_FACTORY( BCHTool,BCHCleaningToolAthena )
DECLARE_ALGORITHM_FACTORY( TestBCH )

DECLARE_FACTORY_ENTRIES( BCHCleaningTool )
{
    DECLARE_NAMESPACE_TOOL( BCHTool,BCHCleaningToolAthena );
    DECLARE_ALGORITHM( TestBCH );
}


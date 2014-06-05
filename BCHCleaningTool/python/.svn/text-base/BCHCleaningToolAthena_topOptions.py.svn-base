
from BCHCleaningTool.BCHCleaningToolConf import BCHTool__BCHCleaningToolAthena

def getBCHTool(isMC,prwConfig,lumicalcFile):
    from os import environ,path
    from AthenaCommon.AppMgr import ToolSvc
    
    # Check if the tool exists
    if hasattr(ToolSvc,'BCHCleaningToolAthena'):
        return ToolSvc.BCHCleaningToolAthena
    
    # If this is MC, check paths
    if isMC:
        pathsAreValid = False
        prwConfigFull = None
        if path.isfile(prwConfig) and path.isfile(lumicalcFile):
            pathsAreValid = True
            prwConfigFull = prwConfig
        elif path.isfile(lumicalcFile):
            testArea = environ.get('TestArea')
            prwDir = "%s/PhysicsAnalysis/AnalysisCommon/PileupReweighting"%(testArea)
            if path.isfile("%s/share/%s"%(prwDir,prwConfig)):
                pathsAreValid = True
                prwConfigFull = "%s/share/%s"%(prwDir,prwConfig)
            elif path.isfile("%s/%s"%(prwDir,prwConfig)):
                pathsAreValid = True
                prwConfigFull = "%s/%s"%(prwDir,prwConfig)
            elif path.isfile("%s/PhysicsAnalysis/AnalysisCommon/%s"%(testArea,prwConfig)):
                pathsAreValid = True
                prwConfigFull = "%s/PhysicsAnalysis/AnalysisCommon/%s"%(testArea,prwConfig)
            
        # Should have found the path now if it exists - make the tool if possible
        if pathsAreValid:
            tool = BCHTool__BCHCleaningToolAthena(InputIsMC = isMC, PileupReweightingConfig = prwConfigFull, LumicalcFile = lumicalcFile)
            return tool
        
        print "ERROR getBCHTool: input file(s) do not exist and this is MC - returning no tool"
        print "\tprwConfig    = %s"%(prwConfig)
        print "\tlumicalcFile = %s"%(lumicalcFile)
        return None

    # This is data - quite easy
    tool = BCHTool__BCHCleaningToolAthena(InputIsMC = isMC, PileupReweightingConfig = None, LumicalcFile = None)
    return tool
        

def addDefaultBCHTool():

    from AthenaCommon.GlobalFlags import globalflags
    from AthenaCommon.AppMgr import ToolSvc
    
    isData = globalflags.DataSource() == 'data'
    
    bchTool = None
    if isData:
        bchTool = getDefaultBCHTool(not isData,None,None)
    else:
        # Intended to fail if the user doesn't provide their own input lumicalc file
        bchTool = getDefaultBCHTool(not isData,'share/mc12a_defaults.prw.root','ilumicalc_histograms.root')
    
    if bchTool:
        ToolSvc += bchTool
        print      bchTool
    else:
        print "ERROR: BCHCleaningToolAthena was not created"








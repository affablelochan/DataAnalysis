{
//here I'm running from the StandAlone directory of the BCHCleaningTool package

//load all libraries
gSystem->Load("../../JetAnalysisEDM/StandAlone/libJetAnalysisEDM.so");
gSystem->Load("../../PATCore/StandAlone/libPATCore.so");
gSystem->Load("../../PileupReweighting/StandAlone/libPileupReweighting.so");
gSystem->Load("../../TileTripReader/StandAlone/libTileTripReader.so");
gSystem->Load("libBCHCleaningTool.so");
//prepare a TileTripReader instance
Root::TTileTripReader *ttr=new Root::TTileTripReader("thetripreader");
ttr->setTripFile("../../TileTripReader/data/CompleteTripList_2011-2012.root");
//prepare a BCHCleaningTool instance (NOTE: here I'm not sure why ROOT only sees either base or derived class functions, not both)
BCHTool::BCHCleaningToolBase *bchtool = new BCHTool::BCHCleaningToolRoot("thebchtool");
//initialize it (for data)
bchtool->InitializeTool(true, ttr);
//call some function
bool isInDeadRegion = bchtool->IsInDeadRegion(211787,100,0.05,0.1);


}
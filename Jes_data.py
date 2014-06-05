#!/usr/bin/env python
import os
import ROOT
import sys, string, commands
import user
import PyCintex; PyCintex.Cintex.Enable()

CWD = os.getcwd()
from ROOT import gSystem
os.chdir(CWD)

from ROOT import *
from array import array
from ROOT import TVector2


from optparse import OptionParser 
parser = OptionParser()
parser.add_option("-e", "--input_esd", help="input esd", action="store", type="string", dest="input_esd", default=False)
parser.add_option("-a", "--input_aod", help="input aod", action="store", type="string", dest="input_aod", default=False)
parser.add_option("-d", "--input_dpd", help="input dpd", action="store", type="string", dest="input_dpd", default=False)
parser.add_option("-v", "--verbose_mode", help="turn on verbose mode", action="store_true", dest="verbose", default=False)
parser.add_option("-n", "--number_of_events", action="store", type="int", help="only run over a certain number of events", dest="n_events", default=False)

(options, args) = parser.parse_args()

##tile trip reader
ROOT.gSystem.Load( "libTTileTripReader.so" )
m_treader= ROOT.Root.TTileTripReader("myTripReader");
m_treader.setTripFile("CompleteTripList_2011-2012.root") 


#initialize JEUncertainty package
ROOT.gSystem.Load( "JetUncertainties/StandAlone/libJetUncertainties.so" )
Jetuncertaintytool = ROOT.MultijetJESUncertaintyProvider("JES_2012/Moriond2013/MultijetJES_2012.config","JES_2012/Moriond2013/InsituJES2012_AllNuisanceParameters.config","AntiKt6TopoLC","MC12a", "JetUncertainties/share/");




def Calculate_DeltaPhi(phi_1,phi_2):
   dphi = TVector2.Phi_mpi_pi(phi_1-phi_2)
   return fabs(dphi)


if not options.input_dpd:
   print "No list of files to slim given. Use -I to give a file or -i a list containing the list of D4PDS, this analysis will not work."
   d3pds = []
else :
   print "using list of D3PDs given on the command line"
   d3pds=options.input_dpd.split(",")




eventTree = ROOT.TChain("qcd")

for inputFile in d3pds:
   print "add %s" % inputFile
   eventTree.Add(inputFile)

          
#...........booking histograms..................................................#
                     
#...........booking histograms..................................................#
                     

bins = [90, 120, 160, 200, 250, 310, 390, 490, 620, 800, 1040, 1400, 2000]

run_num = [147910,147911,147912,147913,147914,147915,147916,147917]

#L = [ 166934/(7.2850E+10 *9.8554E-01), 1453589/(7.2850E+10 *1.2898E-04), 5998941/(2.6359E+07 *3.9939E-03), 5997211/(5.4419E+05 *1.2187E-03), 5987254/( 6.4453E+03 *7.0821E-04) ,  2996082/( 3.9739E+01 *2.1521E-03), 2993651/( 4.1609E-01 *4.6843E-03), 2991955/( 4.0636E-02 *1.4600E-02) ]


ptnbr = ['p70','p120','p200','p280']
delr = ['dr1','dr2','dr3','dr4']
histname =[]
histname.append('Denom_up')
histname.append('Denom_down')
for x in ptnbr:
   for y in delr:
      nam = x+'_'+y+'_up'
      namd = x+'_'+y+'_down'
      histname.append(nam)
      histname.append(namd)
print histname

folder_name=[]
folder_hist =[]
for inum in range(1,58):
   name = 'comp_'+str(inum)
   name1 = 'comp_'+str(inum)+'_hist'
   folder_name.append(name)
   folder_hist.append(name1)
print folder_name
print folder_hist

#......................................................................................
# booking histograms 
for xx in range(len(folder_name)):
   folder_hist[xx]=[]
   for yy in range(len(histname)):
      temp = folder_name[xx]+"_"+histname[yy]
      folder_hist[xx].append(temp)
      folder_hist[xx][yy] = ROOT.TH1F(folder_hist[xx][yy]," ",12,array('d',bins))
      folder_hist[xx][yy].Sumw2()


histtest = ROOT.TH1F("histtest","cutflow",15000,200799.5,215799.5)

#......................................................................................






#...........booking histograms done...............................................#


n_entries = eventTree.GetEntries()
print "Total entries: ",n_entries

if options.n_events: 
   n_entries = int(options.n_events)
   print "dpd  :   running on %i of these"%n_entries

#......event loop begins..........................................................#
for i in range(n_entries):
   eventTree.GetEntry(i)
   if i >0 and i%1000==0:
      print '%s: evt num:'%i
   run = eventTree.RunNumber
   histtest.Fill(run,1)
   if not m_treader.checkEvent(eventTree.RunNumber,eventTree.lbn,eventTree.EventNumber): 
      continue

   J55_passed = False
   J80_passed = False
   J110_passed = False
   J145_passed = False
   J180_passed = False   
   J220_passed = False
   J280_passed = False
   J360_passed = False

   

   for trigger in eventTree.triggers:
      #print '%s: is passed'%trigger
      if trigger == "EF_j55_a4tchad":
         J55_passed = True
      if trigger == "EF_j80_a4tchad":
         J80_passed = True
      if trigger == "EF_j110_a4tchad":
         J110_passed = True
      if trigger == "EF_j145_a4tchad":
         J145_passed = True
      if trigger == "EF_j180_a4tchad":
         J180_passed = True
      if trigger == "EF_j220_a4tchad":
         J220_passed = True
      if trigger == "EF_j280_a4tchad":
         J280_passed = True
      if trigger == "EF_j360_a4tchad":
         J360_passed = True
		 		 		 



   njet=len(eventTree.jet_pt)        
#......reference jet loop begins......#   
   #print 'num of jets: %s'%njet  


   for all in range(len(folder_name)):
      for j in range(njet): 
         jety = eventTree.jet_y[j]
         jeteta = eventTree.jet_eta[j]
         rel=0
         #print "jet pt before: %s"%(eventTree.jet_pt[j]/1000)
         if (all <= 50) and eventTree.jet_pt[j] >70000: rel = Jetuncertaintytool.getRelUncertComponent(all,eventTree.jet_pt[j], eventTree.jet_eta[j]) 
         if (all == 51) and eventTree.jet_pt[j] >70000: rel = Jetuncertaintytool.getRelFlavorCompUncert(eventTree.jet_pt[j], eventTree.jet_eta[j], True) 
         if (all == 52) and eventTree.jet_pt[j] >70000: rel = Jetuncertaintytool.getRelFlavorResponseUncert(eventTree.jet_pt[j], eventTree.jet_eta[j])
         if (all == 53) and eventTree.jet_pt[j] >70000: rel = Jetuncertaintytool.getRelNPVOffsetTerm(eventTree.jet_pt[j],eventTree.jet_eta[j],eventTree.n_pvtx)
         if (all == 54) and eventTree.jet_pt[j] >70000: rel = Jetuncertaintytool.getRelMuOffsetTerm(eventTree.jet_pt[j],eventTree.jet_eta[j],eventTree.mu)
         if (all == 55) and eventTree.jet_pt[j] >70000: rel = Jetuncertaintytool.getRelPileupPtTerm(eventTree.jet_pt[j],eventTree.jet_eta[j],eventTree.n_pvtx,eventTree.mu);
         if (all == 56) and eventTree.jet_pt[j] >70000: rel = Jetuncertaintytool.getRelPileupRhoTopology(eventTree.jet_pt[j],eventTree.jet_eta[j]); 


         jetpt=(eventTree.jet_pt[j])
         jetpt=jetpt*(1+rel)/1000
         #print "jet pt after: %s"%(jetpt)

         jetphi=eventTree.jet_phi[j]
         good=0 
         good=eventTree.jet_Badness[j]
      
         if ROOT.fabs(jety) <=1 and good<3 and ((J55_passed and (90 <= jetpt) and (jetpt <=120)) or  (J80_passed and (120 <= jetpt) and (jetpt <=160)) or(J110_passed and (160 <= jetpt) and (jetpt <=200)) or (J145_passed and(200<= jetpt) and (jetpt <=250)) or (J180_passed and(250<= jetpt) and (jetpt <=310)) or (J220_passed and(310<= jetpt) and (jetpt <=390)) or (J280_passed and(390<= jetpt) and (jetpt <=490)) or (J360_passed and (jetpt >=490))):
            folder_hist[all][0].Fill(jetpt,1)      


            for k in range(njet):
               if j!=k and eventTree.jet_Badness[k]<3:
                  jetphi1=eventTree.jet_phi[k]
                  dphi= Calculate_DeltaPhi(jetphi,jetphi1)
                  #print "the dphi val: %+5.2f ; phi1- %+5.2f; phi2- %+5.2f"%(dphi,jetphi,jetphi1)

                  yj=eventTree.jet_y[k]
                  dy=ROOT.fabs(jety-yj)
                  dr=ROOT.sqrt(dy*dy+dphi*dphi)  
                  #print "jet pt bfore: %s"%(eventTree.jet_pt[k]/1000.)
                  relnbr=0
                  if (all <= 50)  and eventTree.jet_pt[k] >70000 : relnbr = Jetuncertaintytool.getRelUncertComponent(all,eventTree.jet_pt[k], eventTree.jet_eta[k]) 
                  if (all == 51)  and eventTree.jet_pt[k] >70000 : relnbr = Jetuncertaintytool.getRelFlavorCompUncert(eventTree.jet_pt[k], eventTree.jet_eta[k], True) 
                  if (all == 52)  and eventTree.jet_pt[k] >70000 : relnbr = Jetuncertaintytool.getRelFlavorResponseUncert(eventTree.jet_pt[k], eventTree.jet_eta[k])
                  if (all == 53)  and eventTree.jet_pt[k] >70000 : relnbr = Jetuncertaintytool.getRelNPVOffsetTerm(eventTree.jet_pt[k],eventTree.jet_eta[k],eventTree.n_pvtx)
                  if (all == 54)  and eventTree.jet_pt[k] >70000 : relnbr = Jetuncertaintytool.getRelMuOffsetTerm(eventTree.jet_pt[k],eventTree.jet_eta[k],eventTree.mu)
                  if (all == 55)  and eventTree.jet_pt[k] >70000 : relnbr = Jetuncertaintytool.getRelPileupPtTerm(eventTree.jet_pt[k],eventTree.jet_eta[k],eventTree.n_pvtx,eventTree.mu);
                  if (all == 56)  and eventTree.jet_pt[k] >70000 : relnbr = Jetuncertaintytool.getRelPileupRhoTopology(eventTree.jet_pt[k],eventTree.jet_eta[k]); 
 
                  ptj=(eventTree.jet_pt[k])
                  ptj=ptj*(1+relnbr)/1000
                  #print "jet pt after: %s"%(ptj)
 
                  #print 'the delta r is %s'%dr                    
                  if ptj >=70:
                     if dr >=1 and dr <=1.4: 
                        folder_hist[all][2].Fill(jetpt,1)
                     if dr >=1.4 and dr <=1.8: 
                        folder_hist[all][4].Fill(jetpt,1)          
                     if dr >=1.8 and dr <=2.2: 
                        folder_hist[all][6].Fill(jetpt,1) 
                     if dr >=2.2 and dr <=2.6: 
                        folder_hist[all][8].Fill(jetpt,1) 
                  if ptj >=120:
                     if dr >=1 and dr <=1.4: 
                        folder_hist[all][10].Fill(jetpt,1)
                     if dr >=1.4 and dr <=1.8: 
                        folder_hist[all][12].Fill(jetpt,1)          
                     if dr >=1.8 and dr <=2.2: 
                        folder_hist[all][14].Fill(jetpt,1) 
                     if dr >=2.2 and dr <=2.6: 
                        folder_hist[all][16].Fill(jetpt,1) 
                  if ptj >=200:
                     if dr >=1 and dr <=1.4: 
                        folder_hist[all][18].Fill(jetpt,1)
                     if dr >=1.4 and dr <=1.8: 
                        folder_hist[all][20].Fill(jetpt,1)          
                     if dr >=1.8 and dr <=2.2: 
                        folder_hist[all][22].Fill(jetpt,1) 
                     if dr >=2.2 and dr <=2.6: 
                        folder_hist[all][24].Fill(jetpt,1) 

                  if ptj >=280:
                     if dr >=1 and dr <=1.4:
                        folder_hist[all][26].Fill(jetpt,1)
                     if dr >=1.4 and dr <=1.8:
                        folder_hist[all][28].Fill(jetpt,1)
                     if dr >=1.8 and dr <=2.2:
                        folder_hist[all][30].Fill(jetpt,1)
                     if dr >=2.2 and dr <=2.6:
                        folder_hist[all][32].Fill(jetpt,1)







      for j in range(njet): 
         jety = eventTree.jet_y[j]
         jeteta = eventTree.jet_eta[j]
         rel=0
         if (all <= 50) and eventTree.jet_pt[j] >70000 : rel = Jetuncertaintytool.getRelUncertComponent(all,eventTree.jet_pt[j], eventTree.jet_eta[j]) 
         if (all == 51) and eventTree.jet_pt[j] >70000 : rel = Jetuncertaintytool.getRelFlavorCompUncert(eventTree.jet_pt[j], eventTree.jet_eta[j], True) 
         if (all == 52) and eventTree.jet_pt[j] >70000 : rel = Jetuncertaintytool.getRelFlavorResponseUncert(eventTree.jet_pt[j], eventTree.jet_eta[j])
         if (all == 53) and eventTree.jet_pt[j] >70000 : rel = Jetuncertaintytool.getRelNPVOffsetTerm(eventTree.jet_pt[j],eventTree.jet_eta[j],eventTree.n_pvtx)
         if (all == 54) and eventTree.jet_pt[j] >70000 : rel = Jetuncertaintytool.getRelMuOffsetTerm(eventTree.jet_pt[j],eventTree.jet_eta[j],eventTree.mu)
         if (all == 55) and eventTree.jet_pt[j] >70000 : rel = Jetuncertaintytool.getRelPileupPtTerm(eventTree.jet_pt[j],eventTree.jet_eta[j],eventTree.n_pvtx,eventTree.mu);
         if (all == 56) and eventTree.jet_pt[j] >70000 : rel = Jetuncertaintytool.getRelPileupRhoTopology(eventTree.jet_pt[j],eventTree.jet_eta[j]); 


         jetpt=(eventTree.jet_pt[j])*(1-rel)
         jetpt=jetpt/1000

         jetphi=eventTree.jet_phi[j]
         good=0 
         good=eventTree.jet_Badness[j]
      
         if ROOT.fabs(jety) <=1 and good<3 and ((J55_passed and (90 <= jetpt) and (jetpt <=120)) or  (J80_passed and (120 <= jetpt) and (jetpt <=160)) or(J110_passed and (160 <= jetpt) and (jetpt <=200)) or (J145_passed and(200<= jetpt) and (jetpt <=250)) or (J180_passed and(250<= jetpt) and (jetpt <=310)) or (J220_passed and(310<= jetpt) and (jetpt <=390)) or (J280_passed and(390<= jetpt) and (jetpt <=490)) or (J360_passed and (jetpt >=490))):
            folder_hist[all][1].Fill(jetpt,1)      


            for k in range(njet):
               if j!=k and eventTree.jet_Badness[k]<3:
                  jetphi1=eventTree.jet_phi[k]
                  dphi= Calculate_DeltaPhi(jetphi,jetphi1)
                  #print "the dphi val: %+5.2f ; phi1- %+5.2f; phi2- %+5.2f"%(dphi,jetphi,jetphi1)

                  yj=eventTree.jet_y[k]
                  dy=ROOT.fabs(jety-yj)
                  dr=ROOT.sqrt(dy*dy+dphi*dphi)  
                  relnbr=0
                  if (all <= 50) and eventTree.jet_pt[k] >70000  : relnbr = Jetuncertaintytool.getRelUncertComponent(all,eventTree.jet_pt[k], eventTree.jet_eta[k]) 
                  if (all == 51) and eventTree.jet_pt[k] >70000  : relnbr = Jetuncertaintytool.getRelFlavorCompUncert(eventTree.jet_pt[k], eventTree.jet_eta[k], True) 
                  if (all == 52) and eventTree.jet_pt[k] >70000  : relnbr = Jetuncertaintytool.getRelFlavorResponseUncert(eventTree.jet_pt[k], eventTree.jet_eta[k])
                  if (all == 53) and eventTree.jet_pt[k] >70000  : relnbr = Jetuncertaintytool.getRelNPVOffsetTerm(eventTree.jet_pt[k],eventTree.jet_eta[k],eventTree.n_pvtx)
                  if (all == 54) and eventTree.jet_pt[k] >70000  : relnbr = Jetuncertaintytool.getRelMuOffsetTerm(eventTree.jet_pt[k],eventTree.jet_eta[k],eventTree.mu)
                  if (all == 55) and eventTree.jet_pt[k] >70000  : relnbr = Jetuncertaintytool.getRelPileupPtTerm(eventTree.jet_pt[k],eventTree.jet_eta[k],eventTree.n_pvtx,eventTree.mu);
                  if (all == 56) and eventTree.jet_pt[k] >70000  : relnbr = Jetuncertaintytool.getRelPileupRhoTopology(eventTree.jet_pt[k],eventTree.jet_eta[k]); 

                  ptj=(eventTree.jet_pt[k])*(1-relnbr)
                  ptj=ptj/1000

 
                  #print 'the delta r is %s'%dr                    
                  if ptj >=70:
                     if dr >=1 and dr <=1.4: 
                        folder_hist[all][3].Fill(jetpt,1)
                     if dr >=1.4 and dr <=1.8: 
                        folder_hist[all][5].Fill(jetpt,1)          
                     if dr >=1.8 and dr <=2.2: 
                        folder_hist[all][7].Fill(jetpt,1) 
                     if dr >=2.2 and dr <=2.6: 
                        folder_hist[all][9].Fill(jetpt,1) 
                  if ptj >=120:
                     if dr >=1 and dr <=1.4: 
                        folder_hist[all][11].Fill(jetpt,1)
                     if dr >=1.4 and dr <=1.8: 
                        folder_hist[all][13].Fill(jetpt,1)          
                     if dr >=1.8 and dr <=2.2: 
                        folder_hist[all][15].Fill(jetpt,1) 
                     if dr >=2.2 and dr <=2.6: 
                        folder_hist[all][17].Fill(jetpt,1) 
                  if ptj >=200:
                     if dr >=1 and dr <=1.4: 
                        folder_hist[all][19].Fill(jetpt,1)
                     if dr >=1.4 and dr <=1.8: 
                        folder_hist[all][21].Fill(jetpt,1)          
                     if dr >=1.8 and dr <=2.2: 
                        folder_hist[all][23].Fill(jetpt,1) 
                     if dr >=2.2 and dr <=2.6: 
                        folder_hist[all][25].Fill(jetpt,1) 

                  if ptj >=280:
                     if dr >=1 and dr <=1.4:
                        folder_hist[all][27].Fill(jetpt,1)
                     if dr >=1.4 and dr <=1.8:
                        folder_hist[all][29].Fill(jetpt,1)
                     if dr >=1.8 and dr <=2.2:
                        folder_hist[all][31].Fill(jetpt,1)
                     if dr >=2.2 and dr <=2.6:
                        folder_hist[all][33].Fill(jetpt,1)







#.......Writing histograms to root file.............................................#

f = TFile("out.root", "RECREATE")

histtest.Write()

for xx in range(len(folder_name)):
   f.mkdir(folder_name[xx])
   f.cd(folder_name[xx])
   for yy in range(len(histname)):
      folder_hist[xx][yy].Write()
   f.cd()



f.Write()
f.Close()
   
print 'done'
#...............................DEAD END............................................#



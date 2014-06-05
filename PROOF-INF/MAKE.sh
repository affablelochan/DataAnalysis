echo 'begin MAKE'
echo in directory:$PWD
root -l <<EOF
cout << "Start of ROOT Make Modules" << endl;
//gROOT->ProcessLine(".L LeptonTriggerSF_VV.cxx+");
cout << "End of ROOT Make Modules" << endl;
.q
EOF
echo 'end Make'

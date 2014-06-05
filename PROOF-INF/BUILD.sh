echo 'begin build'
echo in directory:$PWD
root -l <<EOF
cout << "Start of ROOT Build Modules" << endl;
//gROOT->ProcessLine(".L LeptonTriggerSF_VV.cxx++");
cout << "End of ROOT Build Modules" << endl;
.q
EOF
echo 'end build'

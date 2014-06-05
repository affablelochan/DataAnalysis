# stream/job specific parameters
ident=el.test.dilip

tree=qcd

# path to input root file containers


#data=/hdfs/users/dilip/analysis/data     
#data=/hdfs/users/dilip/analysis/datatest

#data=/work/rsu004/v4_A-L/datatest
data=/work/rsu004/v4_A-L/data

# location of input histograms
in=./input/

#jes

do_jes=false
jesvar=up
jescomp=0 
do_npv=false

#Monte Carlo parameters
do_muon=true
do_mc=true
do_50_50=false
left_50=false

# proof related
proof=true
build=true
proof_scream=false
dryrun=false
out=../output/

go="nice --adjustment=5 "
go1=
go2="nohup nice "

$go1 root -l <<END!
.x run_ok.C("$data","$in","$ident","$do_jes","$jesvar","$jescomp","$do_npv","$do_mc","$do_muon","$do_50_50","$left_50",$build,$proof,$proof_scream,$dryrun,"$tree","$out" );
.q
END!




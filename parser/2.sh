ROOT="/panfs/traces01/strides-analytics"
SRC="$ROOT/gs_prod2/sra-pub-logs-1/20200531/*.gz"
TOOL_PROF="bin/log2jsn-prof"
TOOL_REL="bin/log2jsn-rel"
LINES="1000000"

SRC2="$ROOT/gs_prod/20190403/sra-pub-run-1_usage_2019_04_03_15_00_00_047aefd35f918ba301_v0.gz"
SRC3="$ROOT/gs_prod/20200430/sra-pub-src-14_usage_2020_04_30_15_00_00_0052d2326de3dff9af_v0.gz"
SRC4="$ROOT/gs_prod/20200509/sra-pub-src-8_usage_2020_05_09_10_00_00_04aa52fd79d5b986eb_v0.gz"
SRC5="$ROOT/sra_prod/20200607/panfspan1.be-md.ncbi.nlm.nih.govapplog_db_tmpdatabaselogarchiveftp.httplocal_archive20200607srafiles35access.log_20200607.logbuff22.gz"
SRC6="$ROOT/sra_prod/20180815/panfstraces01.be-md.ncbi.nlm.nih.govsra_reviewscratchyaschenkFTPLOGS2018srafiles22.access.log_20180815.gz"

function prof
{
    PROF_RES="profres.txt"
    zcat $1 | head -n $LINES | $TOOL_PROF op  > /dev/null
    gprof $TOOL gmon.out > $PROF_RES
}

function rel
{
    zcat $1 | head -n $LINES | $TOOL_REL op > /dev/null
}

function just_zcat
{
    zcat $1 | head -n $LINES > /dev/null
}

#prof $SRC5
rel $SRC5
#just_zcat $SRC5

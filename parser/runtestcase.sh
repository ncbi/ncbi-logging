#
#
#
# $1 input file
# $2 tool
# $3 output directory
#
INPUT=$1
TOOL=$2
ACTUAL=$3

rm -rf ${ACTUAL}

#1 the command to run
#2 exit code if error
run()
{
    eval $1
    retVal=$?
    if [ $retVal -ne 0 ]; then
        echo "runtestcase.sh: $1 failed $retVal"
        exit $2
    fi
}

run "mkdir -p ${ACTUAL}" 1
run "test -f ${INPUT}" 2

echo "cat ${INPUT} | ${TOOL} ${ACTUAL}/out >${ACTUAL}/stdout 2>${ACTUAL}/stderr"

run "cat ${INPUT} | ${TOOL} ${ACTUAL}/out >${ACTUAL}/stdout 2>${ACTUAL}/stderr" 3

exit 0


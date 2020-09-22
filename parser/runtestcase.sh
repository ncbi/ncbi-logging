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
mkdir -p ${ACTUAL}  || ( echo "runtestcase.sh: mkdir -p ${ACTUAL} failed $?" && exit 1 )
test -f ${INPUT}    || ( echo "runtestcase.sh: test -f ${INPUT} failed $?"   && exit 2 )

echo "cat ${INPUT} | ${TOOL} ${ACTUAL}/out >${ACTUAL}/stdout 2>${ACTUAL}/stderr"

cat ${INPUT} | ${TOOL} ${ACTUAL}/out >${ACTUAL}/stdout 2>${ACTUAL}/stderr
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "runtestcase.sh: ${TOOL} failed $?"
    exit 3
fi

exit 0


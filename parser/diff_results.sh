# $1 (actual) output directory
# $2 expected directory
#
ACTUAL=$1
EXPECTED=$2

diff ${EXPECTED} ${ACTUAL}
retVal=$?
if [ $retVal -ne 0 ]; then
    echo "diff_results.sh: diff ${EXPECTED} ${ACTUAL} failed $?"
    exit 1
fi

echo "diff_results.sh: files in ${EXPECTED} and ${ACTUAL} match"
exit 0


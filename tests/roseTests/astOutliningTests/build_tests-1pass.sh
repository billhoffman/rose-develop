#!/bin/sh

# Default filename for output
MAKEFILE_DEF=Makefile-1pass

if test -z "$1" || test -z "$2" ; then
    echo \
"
usage: $0 <ROSE-source-tree> <test-list> [output-Makefile]

This script builds a 'Makefile', stored in [output-Makefile] for
testing the outliner. If [output-Makefile] is not specified,
'${MAKEFILE_DEF}' is used as the default.
"
    exit 1
fi

ROSE_DIR="$1"
TEST_LIST_RAW="$2"
MAKEFILE="$3"
if test -z "${MAKEFILE}" ; then
    MAKEFILE="${MAKEFILE_DEF}"
fi

echo \
"#
# Test cases for the Outliner.
#
# Auto-generated by $0 (`whoami`) at `hostname` on `date`
#
# Caller must define:
#   OUTLINER_BIN
#   TEST_DIR
#   TEST_CXXFLAGS
#
# Caller may specify environment variable, RUN_FLAGS, to add
# command-line flags to each test run.
#
" > "${MAKEFILE}"

# Filter test list
#   My bugs: test2004_162, test2006_97, test2006_98
#   ROSE bugs: test2005_04, test2005_06, test2005_88
#   Requires test-specific defines: test2005_56
#  Liao, 12/6/2007,
# 	test2006_98
#	test2006_99.C bug 135: GCC 4.x does not allow address of explicit register var.
# Liao, 3/2/2009, inputBug327: pointer to function as parameter, not handled yet
TEST_LIST=""
for f in `cat "${TEST_LIST_RAW}"` ; do
    base_name=`basename "${f}"`
    test_name=`echo "${base_name}" | sed 's,\.C$,,'`
    if \
        test x"${test_name}" != x"test2004_30" \
        && test x"${test_name}" != x"test2006_99" \
        && test x"${test_name}" != x"test2006_159" \
        && test x"${test_name}" != x"test2005_56" \
        && test x"${test_name}" != x"inputBug327" \
    ; then
        TEST_LIST="${TEST_LIST} ${f}"
    fi
done

# Generate Makefile rules
TEST_NAME_LIST=""
for f in ${TEST_LIST} ; do
    base_name=`basename "${f}"`
    test_name=`echo "${base_name}" | sed 's,\.C$,,'`
    if test -n "${test_name}" ; then
        TEST_NAME_LIST="${TEST_NAME_LIST} ${test_name}"
    fi
done
echo "one-pass: ${TEST_NAME_LIST}" >> "${MAKEFILE}"
echo "" >> "${MAKEFILE}"

for f in ${TEST_LIST} ; do
    base_name=`basename "${f}"`
    test_name=`echo "${base_name}" | sed 's,\.C$,,'`
    if test -n "${test_name}" ; then
        echo \
"${test_name}: rose_${test_name}.C
	@:

rose_${test_name}.C: \$(TEST_DIR)/${test_name}.C
	test -f ${test_name}.C || ln -s \$(TEST_DIR)/${test_name}.C .
	\$(OUTLINER_BIN) \$(TEST_CXXFLAGS) \$(RUN_FLAGS) -c ${test_name}.C
	test -f \$@ && rm -f ${test_name}.C
" >> "${MAKEFILE}"
    fi
done

echo \
"
clean-check:
	rm -f *.o rose_*.C *.C.pdf *~ core OUT*-rose_*.C

# eof
" >> "${MAKEFILE}"

# eof

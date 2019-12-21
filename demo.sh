#!/bin/bash

TOPDIR=`pwd`
BINDIR=${TOPDIR}/build/demo
DATDIR=${TOPDIR}/datasets
KEYOPT=-g

while getopts s OPT; do
    case $OPT in
	"s" ) KEYOPT="" ;;
    esac
done

INFOFILE=${DATDIR}/sample_info.csv
TESTFILE=${DATDIR}/sample_test.csv
MODELFILE=${DATDIR}/sample_model.csv

xterm -T "TA"       -e "/bin/bash -c 'cd ${BINDIR}/ta     && ./ta ${KEYOPT};                         exec /bin/bash -i'"&
xterm -T "CS"       -e "/bin/bash -c 'cd ${BINDIR}/cs     && ./cs;                                   exec /bin/bash -i'"&
xterm -T "MP"       -e "/bin/bash -c 'cd ${BINDIR}/mp     && ./mp -i ${INFOFILE} -m ${MODELFILE};    exec /bin/bash -i'"&
xterm -T "Client#1" -e "/bin/bash -c 'cd ${BINDIR}/client && time ./client -i ${INFOFILE} -t ${TESTFILE}; exec /bin/bash -i'"&
#xterm -T "Client#2" -e "/bin/bash -c 'cd ${BINDIR}/client && time ./client -i ${INFOFILE} -t ${TESTFILE}; exec /bin/bash -i'"&
#xterm -T "Client#3" -e "/bin/bash -c 'cd ${BINDIR}/client && time ./client -i ${INFOFILE} -t ${TESTFILE}; exec /bin/bash -i'"&
#xterm -T "Client#4" -e "/bin/bash -c 'cd ${BINDIR}/client && time ./client -i ${INFOFILE} -t ${TESTFILE}; exec /bin/bash -i'"&

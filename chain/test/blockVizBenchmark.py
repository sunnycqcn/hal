#!/usr/bin/env python

#Copyright (C) 2013 by Glenn Hickey
#
#Released under the MIT license, see LICENSE.txt
#!/usr/bin/env python

"""Simulate snake track browser queries and time them.  Ideally done with hgTracks
but it's easier to automate this way, as far as I can tell. 
"""
import argparse
import os
import sys
import copy
import subprocess
import time
import math
import random
from datetime import datetime

from hal.stats.halStats import runShellCommand
from hal.stats.halStats import getHalGenomes
from hal.stats.halStats import getHalNumSegments
from hal.stats.halStats import getHalStats


# Wrapper for blockVizTime
def getBlockVizCmd(options, tgtGenome):
    cmd = "./blockVizTime %s %s %s %s %d %d" % (options.lod, tgtGenome,
                                              options.refGenome, options.refSequence,
                                              options.refFirst, options.refLast)
    if options.refLength >= options.maxSnp:
        cmd += " 1"
    else:
        cmd += " 0"
    if options.doDupes is True:
        cmd += " 1"
    else:
        cmd += " 0"
    if options.udc is not None:
        cmd += " %s" % options.udc

    return cmd

def timeCmd(cmd):
    tstart = datetime.now()
    runShellCommand(cmd)
    tend = datetime.now()
    tdelta = tend - tstart
    tsecs = tdelta.seconds
    tsecs += tdelta.microseconds / 1000000.0
    return tsecs

def simulateLoad(options):
    cmds = [getBlockVizCmd(options, tgtGenome) for tgtGenome in options.tgtGenomes]
    elapsedTime = 0.
    for cmd in cmds:
        elapsedTime += timeCmd(cmd)
    return elapsedTime

def uniformRuns(options):
    trials = []
    for i in xrange(options.reps):
        size = random.uniform(1, options.refLength)
        start = random.uniform(0, options.refLength - size)
        trials.append((int(start), int(start + size)))
    return trials

def runSim(options):
    trials = uniformRuns(options)
    elapsedTimes = []
    for trial in trials:
        options.refFirst = trial[0]
        options.refLast = trial[1]
        elapsedTimes.append((options.refLast - options.refFirst, simulateLoad(options)))
    return elapsedTimes
        
def main(argv=None):
    if argv is None:
        argv = sys.argv

    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        description="Time some simulated browser queries. ")

    parser.add_argument("lod", help="input lod or hal path")
    parser.add_argument("refGenome", help="Name of reference genome")
    parser.add_argument("refSequence", help="Name of reference sequence")
    parser.add_argument("refLength", help="Length of reference sequence", type=int)
    parser.add_argument("tgtGenomes", help="Comma-separated list of target genomes")

    parser.add_argument("--reps", help="Number of queries to perform",
                        type=int, default=100)

    parser.add_argument("--udc", help="UDC path", default=None)

    parser.add_argument("--doDupes", help="Do duplications", action="store_true",
                        default=False)

    parser.add_argument("--maxSnp", help="Max query size to get bases",
                        type=int, default=50000)

    args = parser.parse_args()
    args.tgtGenomes = args.tgtGenomes.split(",")

    times = runSim(args)
    print times
    
if __name__ == "__main__":
    sys.exit(main())

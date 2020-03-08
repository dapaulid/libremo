#!/usr/bin/env python3
#-------------------------------------------------------------------------------
# @license
# Copyright (c) Daniel Pauli <dapaulid@gmail.com>
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.
#-------------------------------------------------------------------------------

import subprocess
import sys
import io
import time
import argparse


#-------------------------------------------------------------------------------
# constants
#-------------------------------------------------------------------------------

HRULE = '-' * 80


#-------------------------------------------------------------------------------
# helpers
#-------------------------------------------------------------------------------

def mean(numbers):
    return float(sum(numbers)) / max(len(numbers), 1)
# end def


#-------------------------------------------------------------------------------
# main
#-------------------------------------------------------------------------------

# parse arguments
parser = argparse.ArgumentParser()
parser.add_argument('-r', '--repetitions', type=int,
	help="number of executions. run forever if not specified.")
parser.add_argument('-e', '--exit-on-error', action="store_true",
	help="exit on first error")
parser.add_argument('cmd', nargs=argparse.REMAINDER)
args = parser.parse_args()

# repeatedly execute command
total_executions = 0
failed_executions = 0
exec_times = []
try:
	while args.repetitions == None or total_executions < args.repetitions:
		exec_time = None
		start = time.perf_counter()
		try:
			# run command
			subprocess.check_output(args.cmd, stderr=subprocess.STDOUT, encoding='cp1252')
			exec_time = time.perf_counter() - start
			# indicate progress
			sys.stdout.write('.')
			sys.stdout.flush()
		except subprocess.CalledProcessError as e:
			exec_time = time.perf_counter() - start
			# show output on error
			print("\nexecution #%d failed. output:" % (total_executions+1))
			print(HRULE)
			print(e.output)
			print(HRULE)			
			failed_executions += 1
		# end try
		# update statistics
		total_executions += 1
		exec_times.append(exec_time)
		# handle exit on first error
		if args.exit_on_error and failed_executions > 0:
			break
		# end if
	# end while
except KeyboardInterrupt:
	print("\nCancelled by user")
# end try

if not args.exit_on_error:
	print("\ntotal executions: %d, failed: %d (%0.2f%%)" %
		(total_executions, failed_executions, 100.0 / total_executions * failed_executions))
elif failed_executions == 0:
	print("\nno error during %d executions" % (total_executions))
else:
	print("\nFAILED after %d executions" % (total_executions-1))
# end if
if len(exec_times) > 0:
	print("execution time: mean=%0.3fs, min=%0.3fs, max=%0.3fs" %
		(mean(exec_times), min(exec_times), max(exec_times)))
# end if

# fail if at least one execution failed
if failed_executions > 0:
	sys.exit(1)
#end if


#-------------------------------------------------------------------------------

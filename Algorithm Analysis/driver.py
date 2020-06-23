#!/usr/bin/env python3

import argparse
import max_subarray
import timeit
import time


def timed_execute(function, input_data):
    """Execute passed function on passed input data and its running time in
    seconds"""
    return timeit.timeit(lambda: function(input_data), number = 1,
            timer=time.process_time)

# A mapping of Short names for the algorithms that will be passed via the
# arguments to the actual functions that will be executed
all_algorithms = {
        "cubic": max_subarray.cubic_algorithm,
        "quad1": max_subarray.quadratic_algorithm_1,
        "quad2": max_subarray.quadratic_algorithm_2,
        "divcon": max_subarray.divide_and_conquer_algorithm,
        "linear": max_subarray.linear_algorithm
        }

# Create argument parser object
parser = argparse.ArgumentParser(description="Run the timing tests on" +
        "implementations of the maximum subarray problem")

# Add the optional 'run' argument that specifies which algorithms to run
parser.add_argument('--run', nargs="+", help="Specifies which  algorithms " +
        "to run", choices=all_algorithms.keys())
# Add the sizes argument that tells what sizes of data to give as input to the
# algorithm implementations
parser.add_argument('--sizes', nargs="+", type=int, required=True, help=
        "Sizes of the input array to give to the algorithm implementations")

args = parser.parse_args()

# Eliminate duplicates from sizes argument and sort it
sizes = sorted(list(set(args.sizes)))
largest_size = sizes[-1]

# Open input numbers file
with open("input_numbers.txt") as infile:
    # and read only as much data as needed by largest sized input
    data = [int(infile.readline()) for _ in range(largest_size)]

# If no 'run' argument is provided, then we run all our algorithms
algorithms = all_algorithms.keys() if args.run is None else args.run

# Keep the run times in a dict so that we can display them later as a table
run_times = dict()

for algorithm in algorithms:
    if algorithm not in run_times:
        run_times[algorithm] = dict()
    
    for size in sizes:
        testNum = 5
        time_taken = 0.0
        for i in range(testNum):
            print("Running {} with input of size {} ... ".format(algorithm, size),
                    end='', flush=True)
            # Execute and record time taken
            time_taken += timed_execute(all_algorithms[algorithm], data[:size])
        run_times[algorithm][size] = time_taken/testNum
        print("done in average {} seconds".format(time_taken))

print()
print("Algorithm", end='')
for size in sizes:
    # Print the table header with appropriate spacing
    print("\t%11d" % size, end='')
print()
# Loop over the algorithms and sizes and display the run times as a table
for algorithm in algorithms:
    row = algorithm + "\t"
    for size in sizes:
        row += "\t%.8fs" % run_times[algorithm][size]
    print(row)

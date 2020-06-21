Jesse Hazard

The routine that takes the most time is Mem_map.
This is because it accesses the unmapped array,
which is a long block of memory that does not get
used frequently and thus gets pushed out of the cache.

2 hours analyzing the problems in this assignment.
~35 hours solving the problems.

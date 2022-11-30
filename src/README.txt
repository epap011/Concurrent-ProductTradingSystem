###############################
# Efthymios Papageorgiou 4340 #
###############################

----< The program works as expected !! >----

############################
# How to build the program #
############################
command: make OR make build


##########################
# How to run the program #
##########################
command: ./a.out N
command: ./a.out N --debug_mode


#############################
# How to clean object files #
#############################
command: make clean


##############
# Highlights #
##############

1. Accurate variable/function names
2. Concurrent HashTable, DLL and Stack are reusable via their interfaces/implementations
3. I have provide a way to reduce unessesary HTDeletions -> better performance


###################
# Recommendations #
###################

1. If debug_mode occurs it is highly recommended to give
a small number of N(3, 6, 9) because bigger numbers
produce too much information(chaos).If you enter a bigger N(>15)
be sure to increase the maximum number of lines that terminal keeps
in its buffer.In most cases default is something like 10000 lines.

2. Inputs bigger than 216 produce wrap around at the sum of integers.
   But for N = 213 there is no wrap around :)


####################
# Additional Notes #
####################

1. Hash table is implemented with:
   1.Double Hasing
   2.Ordered Hashing
   3.Open Addressing
   4.No expansion

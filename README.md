# ConcurrentProductTradingSystem
CS486 - Principles of Distributed Computing - Assignment

An implementation of a shared system product handling.The project is implemented in C programming language using pthreads library.

What i implemented (and learned):

**Concurrent Linked List**
The creation of a shared list, which will contain all its products
system that is for sale. The thread producers are responsible for this.

**Concurrent Hash Table (open addressing & double hashing)**
The concurrent creation of shared hash tables from the threads sellers, the
who are responsible to sell the products to consumers. The boards
segmentation represent consumers and products purchased by each one.

**Concurrent Stack**
The thread managers, pick up the defective products that have been returned by
consumers and store them on a shared stack. This stack
contains all products to be processed.
When products are repaired, they are returned back to the shared list and are
ready to be sold again.

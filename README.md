# Chinese Restaurant process

## Code for the Chinese Restaurant Process

This is a simple C implementation of the Chinese Restaurant Process. Its overall purpose is clarity and a certain amount of scalability. For example it should handle a million customers, with little problems. In its first commit, it's a single file, 'crp.c'.

To find out more about this stochastic process, here is the obligatory wikipedia link: [Chinese Restaurant Process](http://en.wikipedia.org/wiki/Chinese_restaurant_process).

It has no dependencies except the GNU C library and its general purpose was to fully understand this stochastic process, rather than any flamboyant use of c.

## Variables and data structures

* ta is a table container
* nels is the number of tables
* arr is the array saying the number of customers at each table.
* alph is unclear as yet .. a modifier to the probability
* cp is for cumulative probabilitis.

# table 0: the notional table.
The initial condition is of no tables, or the waiting notional one. The first customer has to occupy this one.
In fact, this notional table is the same as the customer wanting a new table. Intuitively this seems quite different, but computationally its not really all that different, given its approach of the "I want new table" to be a notional table. This table is never actually occupied of course, its table status is just for the probabilitiesafter the probabilities have been calculated

## alph
When this is set to 1.0, and when you run the debug version, the process is fairly clear.
* first customer, no tables, he must ask for a new one. 100% probability for notional table.
* second customer has choice of new table or joingint he first customer. The probabilities are equal for this: 50/50. Slightly unintuitive in the sense that the occupied table has 1 more customer than the notional empty table, but this intuition does not apply for comparing a 0-customer table and a 1-customer, only for when oth table have at least one customer.
* if second cutomer comes in and chooses new table, we shall have one notional empty table and two tables with one customer each. For the third customer all three will have equal probability, but of course now it is more liekly that he/she will choose one of the occupied tables.
* if the second customer comes in and chooses to join customer num 1, then this table will have a 0.66 probability of being chosen when the third customer arrives.

## To do

### Implementation Points
* Instead of command-line arguments
* For the throwdice() function use doubles, or change to integers instead.
* Instead of the C library's random number, incorporate the Mersenne Twister, which is what everybody else seems to do

### Algorithm Issues
* Limit the total number of tables, which will mean shutting down the ability to add new tables, so that the Process then starts to look like a Pólya urn process.
* In fact this should be generalised, by allowing any table to be shut down after a certain number of customers has been reached.
* Have customers arrive not only in 1's but in numbers over 1.
* Have customers arrive in terms of poisson process.


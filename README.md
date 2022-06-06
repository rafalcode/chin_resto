# Chinese Restaurant process

## Code for the Chinese Restaurant Process

This is a simple C implementation of the Chinese Restaurant Process. Its overall purpose is clarity and a certain amount of scalability. For example it should handle a million customers, with little problems. In its first commit, it's a single file, 'crp.c'.

To find out more about this stochastic process, here is the obligatory wikipedia link: [Chinese Restaurant Process](http://en.wikipedia.org/wiki/Chinese_restaurant_process).

It has no dependencies except the GNU C library and its general purpose was to fully understand this stochastic process, rather than any flamboyant use of c.

## Variables and data structures

* ta is a table container
* nta is the number of tables
* arr is the array saying the number of customers at each table.
* thet is unclear as yet .. a modifier to the probability
* cp is for cumulative probabilitis.

# table 0: the notional table.
The initial condition is of no tables, or the waiting notional one, this is actually the computing representation of the infinite tables in the wikipedia. The first customer has to occupy this one,
In fact, this notional table is the same as the customer wanting a new table. Intuitively this seems quite different, but computationally its not really all that different, given its approach of the "I want new table" to be a notional table. This table is never actually occupied of course, so it does have zero customers, but it behaves probabilistically as if it had one customer, therefore my use of the word "notional".

## thet
This is the so called discount parameter, and it basically bolsters the empty table's initial chance of claiming the next incoming customer. However, it's fairly short-lived, as it's probably steadily decreases. When this is set to 1, and when you run the debug version, the process is fairly clear. Its default value should be 1. 
* first customer, no tables, he must ask for a new one. 100% probability for notional table.
* second customer has choice of new table or joingint he first customer. The probabilities are equal for this: 50/50. Slightly unintuitive in the sense that the occupied table has 1 more customer than the notional empty table, but this intuition does not apply for comparing a 0-customer table and a 1-customer, only for when oth table have at least one customer.
* if second cutomer comes in and chooses new table, we shall have one notional empty table and two tables with one customer each. For the third customer all three will have equal probability, but of course now it is more liekly that he/she will choose one of the occupied tables.
* if the second customer comes in and chooses to join customer num 1, then this table will have a 0.66 probability of being chosen when the third customer arrives.
* the probability of choosing a new table decreases very regularly, but somethign can be done about this with the alpha parameter, which is called the concentration.

## To do

### Implementation Points
* Instead of command-line arguments
* For the throwdice() function use doubles, or change to integers instead.
* Instead of the C library's random number, incorporate the Mersenne Twister, which is what everybody else seems to do

### Algorithm Issues
* Limit the total number of tables, which will mean shutting down the ability to add new tables, so that the Process then starts to look like a Pólya urn process.
* In fact this should be generalised, by allowing any table to be shut down after a certain number of customers has been reached.
* Have customers arrive not only in 1's but in numbers over 1. Nah, this isn't very useful.
* Have customers arrive in terms of poisson process.
* Create indian buffet process from it?

# Indian Buffet process.
This time we are not talking tables, but dishes, and customer do not exclusively pick one, but a combination of dishes whihc are marked witht he number of times they've been tried and also an infinite number of new dishes.


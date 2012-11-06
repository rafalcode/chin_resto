## chin_resto

This is a simple C implementation of the Chinese Restaurant Process. It's overall purpose is clarity and a certain amount of scalability. For example it should handle a million customers, with little problems. In its first commit, it's a single file, 'crp.c'.

It has no dependencies except the GNU C library and it's general purpose was to fully understand this stochastic process, rather than any flamboyant use of c.

## To do

### Implementation Points
* Instead of command-line arguments
* For the throwdice() function use doubles, or change to integers instead.
* Instead of the C library's random number, incorporate the Mersenne Twister, which is what everybody else seems to do

### Algorithm Issues
* Limit the total number of tables, which will mean shutting down the ability to add new tables, and the Process then starts to look like a Pólya urn process.
* Have customers arrive not only in 1's but in numbers over 1.


# Chinese Restaurant process

## Code for the Chinese Restaurant Process

This is a simple C implementation of the Chinese Restaurant Process. Its overall purpose is clarity and a certain amount of scalability. For example it should handle a million customers, with little problems. In its first commit, it's a single file, 'crp.c'.

To find out more about this stochastic process, here is the obligatory wikipedia link: [Chinese Restaurant Process](http://en.wikipedia.org/wiki/Chinese_restaurant_process).

It has no dependencies except the GNU C library and its general purpose was to fully understand this stochastic process, rather than any flamboyant use of c.

## Most insightful example

There are various examples floating about: specieis discovery, topic modelling, gene pathway groups, but actually my favourite is of a new restaurant that has just moved into a neighbourhood and the chef has to decide what types of dishes to offer to the people in his locality, because he doesn't expect people to come from far, they will mostly be locals. The chef has a good idea of global interests, such as vegan, italian (pasta), well there's a series of eater profiles, but he suspects that not all will be demanded locally, it would be a terrible waste to prepare dishes for vegans if there's none locally. So he decides that he will learn as the customer come in, he will make the dishes on deamnd in the beginning before setlling down to a stable menu.

He may find that there are alot of vegans and he needs even two options for them, and this would be relfetected by a substantial cluster in the CRP, but on the other hand, maybe there is no deamnd for sushi at all. Lucky he didn't include it. SO it's that dilemma,
knowing what categories exist on a global level, but no being sure how they will be reflected at a local level. So thhis throws up the importance of several expression when thinking about CRP:
* concern about adpating properly and efficiently to the datasets one receives locally
* on demand creation of clusters

## Variables and data structures

* ta is a table container
* nta is the number of tables
* arr is the array saying the number of customers at each table.
* thet is theta the discount param.
* alph, is alpha the concentration or strength param .. these are just ways of tilting the favour with towards creation of new tables rather than further populating existing ones.
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


# Relation to stick breaking process

This should be smooth, but it isn't. if we take the wellknown Edwin Chen blostpost http://blog.echen.me/2012/03/20/infinite-mixture-models-with-nonparametric-bayes-and-the-dirichlet-process/
and the Michael I Jordan lectures. In any case the code is:

play around witht he alphas, 2,5 10 the hihgher they go the shorter the breaks will be and you could end up with very many little pieces.

 stick_breaking_process = function(num_weights, alpha) {
     betas = rbeta(num_weights, 1, alpha)
 # 1-betas introduces the breaking off stick effect.
     remaining_stick_lengths = c(1, cumprod(1 - betas))[1:num_weights]
 # note how the final beta draw is not used, it's cut off ... I suppose it's just a stopping criterion.
     weights = remaining_stick_lengths * betas
 # so these are the true weights, becuase the early breaks, especially the first one will likely be the biggest, or the winning piece
 # actually in CRP the first table is also at an adntage, But the differnece is that, as more tables appear, it's influence decreases
 # while in the stick breaking, the first piece never change its importance.
   }

However, I think he came up withthe local restauranteur analogy which I like alot, and also he made a good point which is ... which is
just got it and then lst it again. Oh yes, it's a minor point really, enitrely forgettable really, it's about the stick breaking, he is very brief on
and not very illustrative, he merely says, after discussing Chin rest, that stick breaking is a more direct way of generating the clusters. So
this might have something to do with CRP being a bit laborious perhaps, i.e. this choosing between new or existing table and then finally 
coming up with the probabilites (sizes/lengths) of each cluster .. and that perhaps stick breaking and drawing from Beta
which is more sophisticated is more direct, you can just get the clusters immediately. However his comment about setting the total number of arriving customers to
to infinity is not at all revealing. I haven't perhaps conquered that part so well.

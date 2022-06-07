/* crp.c after second revisit of June 2022, I copoed the original crp to crp0
 * and started working on a cleaner version */

/* Code for simulating the Chinese Restaurant Process
 * Only used on linux, and gcc compiler
 * Compile with "make crp" */

/* comments on the notional table: this is my terminology
 * for when the incoming customer decides not to sit with the others
 * but want a new table for himself.
 * Because this possibility has an associated probability
 * the same as all the other tables, the easiest way t deal with it
 * is a table like the rest, but with a special meaning, rahter like
 * a "standby new table" at the ready when the customer decides this way. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFINC 4

typedef struct /* tabarr_t, this is the operational table struct with a probabilities array, for internal purposes. The main() function doesn't see it */
{
    unsigned *arr; /* number of customers at each table */
    float *p;
    unsigned nta; /* number of tables, actually */
    unsigned totcusts;
    unsigned buf;
} tabarr_t;

void inita(tabarr_t **ta, float thet, float alph) /* must be called with 3rd arg = zero for no tblaes occupied */
{
    tabarr_t *tta=*ta;
    tta=malloc(sizeof(tabarr_t));
    tta->buf=BUFINC;

    tta->arr=malloc(tta->buf*sizeof(size_t));
    tta->p=malloc(tta->buf*sizeof(float));

    /* Now the first table is not like the other tables, because it represents the notional new table, an in fact the infinite set of tables in mathematical terms. 
    Note: this will have the odd effect of permitting 1-based indexing in some cases */
    tta->nta = 1;
    tta->arr[0]=0;
    tta->totcusts=0;
    tta->p[0] = (thet + alph*(tta->nta-1))/(thet+tta->totcusts); // we know totcusts will be always zero in this function, but you can link this with the equation in wikipedia entry.

    *ta=tta;
}

void freeta(tabarr_t **ta)
{
    tabarr_t *tta=*ta;
    free(tta->arr);
    free(tta->p);
    free(tta);
    *ta=tta;
}

void upta(tabarr_t **ta, unsigned nta, float thet, float alph) /* update table array */
{
    /* increase (usually) our table container for nta tables */
    int i;
    tabarr_t *tta=*ta;
    /* Deal with situ when an extra table has been added */
    if(nta > tta->nta) {
        if(nta == tta->buf-1) {
            tta->buf += BUFINC;
            tta->arr=realloc(tta->arr, tta->buf*sizeof(size_t));
            tta->p=realloc(tta->p, tta->buf*sizeof(float));
        }
        tta->nta=nta;
        tta->arr[tta->nta-1]=1; /* our new table gets its first customer */
    }
    tta->totcusts++;
    /* now recalc the probability scores */
    for(i=0;i<tta->nta;++i)
        tta->p[i] = (i==0)? (thet + alph*(tta->nta-1))/(thet+tta->totcusts) : ((float)(tta->arr[i])-alph)/(thet+tta->totcusts);
    *ta=tta;
}

void throwdice(tabarr_t *ta, unsigned *winner, int j) /* cp is the cumulative probscores (p for each table) */
{
    int i;
    float pn, *cp=malloc((ta->nta+1)*sizeof(float)); /* cumulative probability line, so to speak */

    cp[0]=.0;
    for(i=1;i<ta->nta+1;++i) 
        cp[i]=ta->p[i-1]+cp[i-1];
    /* Note above that ta->p[] means that a table has its own probability array */

    pn=(float)rand()/RAND_MAX;
#ifdef DEBUG
    printf("Cumul probs before arrival of Customer no. %i: ", j); 
    for(i=0;i<ta->nta+1;++i) 
        printf("%f ", cp[i]); 
    printf("/ diceroll=%f ", pn); 
#endif
    for(i=1;i<ta->nta+1;++i) 
        if(pn<cp[i])
            break;
    *winner=i-1; // the table winner, that is.
    free(cp);
}

void runrest(unsigned nc, float thet, float alph, tabarr_t **ta)
{
    unsigned j, winner;
    unsigned *custs=malloc(nc*sizeof(size_t)); // we rcord to which table each customer is sent to.

    inita(ta, thet, alph); // the simplified version of this function seems to work fine.
#ifdef DEBUG
    printf("Chinese Restaurant Scheme: Table Idx 0 is notional, it causes a new table to be\ncreated and that table to have a single customer.\n"); 
    printf("The first iteration is trivial, as Table Idx 0 must be chosen, it's the only one.\n"); 
    printf("----------------------------\n"); 
#endif
    throwdice(*ta, &winner, 1); // first one is trivial, but do it anyway.
#ifdef DEBUG
    (winner!=0)? printf(" / Result: %u\n", winner) : printf(" / winner=0, so a new (proper) table is created: T%02u\n", (*ta)->nta); /* redundant check, yes, for clarity */
#endif
    if(winner != 0) {
        (*ta)->arr[winner]++;
        upta(ta, (*ta)->nta, thet, alph);
    } else
        upta(ta, (*ta)->nta+1, thet, alph);

    custs[0]=winner+1; /* this will be table 1, the first customer always goes to table 1 */

    /* OK, now we're going to loop through each customer as they arrive in sequential fashio, so it's one customer at a time. */
    for(j=1;j<nc;++j) {

        throwdice(*ta, &winner, j+1);
#ifdef DEBUG
        (winner!=0)? printf(" / Result: %u\n", winner) : printf(" / winner=0, so,new table, num: %u\n", (*ta)->nta);
#endif
        custs[j] = (winner==0)? (*ta)->nta : winner; // i.e. if notional table0 wins, custoemr will go to final table, which is a new one.

        if(winner != 0) { // a current table was chosen, increase its size by 1.
            (*ta)->arr[winner]++;
            upta(ta, (*ta)->nta, thet, alph); // this will mostly be about recalculating probscores.
        } else
            upta(ta, (*ta)->nta+1, thet, alph); // a new tble is need, upta will update accordingly.

    }
    free(custs);
}

int main(int argc, char *argv[])
{
    if(argc!=5) {
        printf("Error. Pls supply 4 arguments: 1) integer for seed 2) theta float 3) alpha float 4) and number of customers.\n");
        printf("       theta (discount param) float should be =1 for default run.\n");
        printf("       alpha (strength/concentration param) float should be =0 for default run.\n");
        exit(EXIT_FAILURE);
    }
    srand(atoi(argv[1]));
    float thet=atof(argv[2]);
    float alph=atof(argv[3]);
    unsigned i, nc=atoi(argv[4]) /* number of customers */;

    tabarr_t *ta=NULL;
    runrest(nc, thet, alph, &ta); // this will be run nc number of times one suspects

    printf("Table listings (%u in total): ", ta->nta-1); /* Table Idx "0" is not a real table, so take it out. */
    for(i=1;i<ta->nta;++i)
        printf("T%02u:%u|", i, ta->arr[i]);
    printf("\n"); 

    freeta(&ta);
    return 0;
}

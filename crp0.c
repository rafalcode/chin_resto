/* Code for simulating the Chinese Restaurant Process
 * Only used on linux, and gcc compiler
 * Compile with gcc -Wall -g crp.c -o crp */

/* comments on the notional table: this is my terminology
 * for when the incoming customer decides not to sit with the others
 * but want a new table for himself.
 * Because this possibility has an associated probability
 * the same as all the other tables, the easiest way t deal with it
 * is a table like the rest, but with a special meaning, rahter like
 * a "standby new table" at the ready when the customer decides this way. */

/* crp0.c is mostly the original code that I cam up ages ago, in Vigo I believe */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFINC 4

typedef struct /* tabarr, a struct to hold an array of tables in sorted order, together with an array of indices of the element's previous positions for external, labelling purposes */
{
    size_t *els; /* elements in ranked form */
    size_t *movsa; /* the movs array, shows what's gone where */
    size_t nta; // number of tables.
} tabarr;

typedef struct /* tarr, this is the operational table struct with a probabilities array, for internal purposes. The main() function doesn't see it */
{
    size_t *arr; /* number of customers at each table */
    float *p;
    size_t nta; /* number of tables, actually */
    size_t sumels;
    size_t buf;
} tarr;

void inita(tarr **ta, size_t statetables[], size_t nta, float thet) /* must be called with 3rd arg = zero for no tblaes occupied */
{
    int i;
    tarr *tta=*ta;
    tta=malloc(sizeof(tarr));
    tta->nta = nta+1; /* at this point nta now includes table 0 as our notional, always empty table */
    for(tta->buf=BUFINC; ;tta->buf +=BUFINC)
        if(tta->buf > tta->nta)
            break;

    tta->arr=malloc(tta->buf*sizeof(size_t));
    tta->p=malloc(tta->buf*sizeof(float));
    tta->arr[0]=0; /* the notional empty table: it's always zero */
    if(tta->nta >1) /* at 1, our stabletables[] is NULL, empty */
        memcpy(tta->arr+1, statetables, (tta->nta-1)*sizeof(size_t));

    tta->sumels=0;
    for(i=1;i<tta->nta;++i)
        tta->sumels += tta->arr[i];
    for(i=0;i<tta->nta;++i)
        tta->p[i] = (i==0)? thet/(thet+tta->sumels) : (float)tta->arr[i]/(thet+tta->sumels);

    *ta=tta;
}

void inita2(tarr **ta, float thet) /* must be called with 3rd arg = zero for no tblaes occupied */
{
    tarr *tta=*ta;
    tta=malloc(sizeof(tarr));
    tta->nta = 1; /* at this point nta now includes table 0 as our notional, always empty table */
    tta->buf=BUFINC;

    tta->arr=malloc(tta->buf*sizeof(size_t));
    tta->p=malloc(tta->buf*sizeof(float));
    tta->arr[0]=0; /* the notional empty table: it's always zero: so arr holds number of customers? */
    tta->sumels=0;
    tta->p[0] = thet/(thet+tta->sumels); // we know sumels will be always zero in this function, but you can link this with the equation in wikipedia entry.

    *ta=tta;
}

void freeta(tarr **ta)
{
    tarr *tta=*ta;
    free(tta->arr);
    free(tta->p);
    free(tta);

    *ta=tta;
}

void upta(tarr **ta, size_t nta, float thet)
{
    /* increase (usually) our table container for nta tables */
    int i;
    tarr *tta=*ta;
    if(nta > tta->nta) {
        if(nta == tta->buf-1) {
            tta->buf += BUFINC;
            tta->arr=realloc(tta->arr, tta->buf*sizeof(size_t));
            tta->p=realloc(tta->p, tta->buf*sizeof(float));
        }
        tta->nta=nta;
        tta->arr[tta->nta-1]=1; /* our new table gets its first customer */
    }
    tta->sumels = 0;
    for(i=1;i<tta->nta;++i)
        tta->sumels += tta->arr[i];
    for(i=0;i<tta->nta;++i)
        tta->p[i] = (i==0)? thet/(thet+tta->sumels) : (float)tta->arr[i]/(thet+tta->sumels);

    *ta=tta;
}

void throwdice(tarr *ta, float *cp[], size_t *cpsz, size_t *winner, int j)
{
    int i;
    float pn, *tcp=*cp;

    if(*cpsz != ta->nta+1) {
        *cpsz= ta->nta+1;
        tcp=realloc(tcp, *cpsz*sizeof(float));
    }
    tcp[0]=.0;
    for(i=1;i<ta->nta+1;++i) 
        tcp[i]=ta->p[i-1]+tcp[i-1];
    /* Note above that ta->p[] means that a table has its own probability array */

    pn=(float)rand()/RAND_MAX;
#ifdef DEBUG
    printf("Cumul probs before arrival of Customer no. %i: ", j); 
    for(i=0;i<ta->nta+1;++i) 
        printf("%f ", tcp[i]); 
    printf("/ diceroll=%f ", pn); 
#endif
    for(i=1;i<ta->nta+1;++i) 
        if(pn<tcp[i])
            break;
    *winner=i-1;
    /* winner=0 actually is that the new entrant has not chosen any of the tables
     * with customers at it, but prefers a new table where they can sit alone. */

    *cp=tcp;
}

void insort(tabarr *tbarr) /* looks like an insertion sort */
{
    int i, j; /* don't make i unsigned, please */
    size_t heldv /* held value */, shadheld /* shadow held? */;

    for(j=1;j<tbarr->nta;++j) {

        heldv=tbarr->els[j];
        shadheld=tbarr->movsa[j];
        i=j-1;
        while( (i>=0) && (tbarr->els[i] < heldv) ) { /* push up all elements behind where heldv was by 1, as long as they are bigger than heldv */
            tbarr->els[i+1] = tbarr->els[i];
            tbarr->movsa[i+1] = tbarr->movsa[i];
            i -= 1;
        }
        tbarr->els[i+1] =heldv; /* i+1 is now heldv's rightful position, and it is now just behind the last elemtn that was moved up */
        tbarr->movsa[i+1] =shadheld;
    }
#ifdef DEBUG
    printf("Here is our movsa array: "); 
    for(j=0;j<tbarr->nta;++j)
        printf("%zu ", tbarr->movsa[j]);
    printf("\n"); 
#endif
}

void runrest(size_t nc, size_t statearr[], size_t nta, float thet, tabarr **tbarr)
{
    size_t j, w, cpsz=0, *custs=malloc(nc*sizeof(size_t));
    float *cp=malloc(cpsz*sizeof(float)); /* cumulative probability line, so to speak */

    tarr *ta=NULL;
    // inita(&ta, statearr, nta, thet); // only called once, excessive things being done here.
    inita2(&ta, thet); // the simplified version of this function seems to work fine.
#ifdef DEBUG
    printf("Chinese Restaurant Scheme: Table Idx 0 is notional, it causes a new table to be\ncreated and that table to have a single customer.\n"); 
    printf("The first iteration is trivial, as Table Idx 0 must be chosen, it's the only one.\n"); 
    printf("----------------------------\n"); 
#endif
    throwdice(ta, &cp, &cpsz, &w, 1); // first one is trivial, but do it anyway.
#ifdef DEBUG
    (w!=0)? printf(" / Result: %zu\n", w) : printf(" / w=0, so,new table, num: %zu\n", ta->nta); /* redundant check, yes, for clarity */
#endif
    if(w != 0) {
        ta->arr[w]++;
        upta(&ta, ta->nta, thet);
    } else
        upta(&ta, ta->nta+1, thet);

    custs[0]=w+1; /* OK, that's the trivial first customer dealt with */

    for(j=1;j<nc;++j) { /* now we consider the other customers */

        throwdice(ta, &cp, &cpsz, &w, j+1);
#ifdef DEBUG
        (w!=0)? printf(" / Result: %zu\n", w) : printf(" / w=0, so,new table, num: %zu\n", ta->nta);
#endif
        custs[j] = (w==0)? ta->nta : w;

        if(w != 0) {
            ta->arr[w]++;
            upta(&ta, ta->nta, thet);
        } else
            upta(&ta, ta->nta+1, thet);

    }

    tabarr *ttbarr=malloc(sizeof(tabarr));
    ttbarr->nta=ta->nta-1;
    ttbarr->els=malloc(ttbarr->nta*sizeof(size_t));
    memcpy(ttbarr->els, ta->arr+1, ttbarr->nta*sizeof(size_t));
    ttbarr->movsa=malloc(ttbarr->nta*sizeof(size_t));
    int i;
    for(i=0;i<ttbarr->nta;++i) 
        ttbarr->movsa[i] = i;

    insort(ttbarr);

    *tbarr=ttbarr;
    freeta(&ta);
    free(cp);
    free(custs);
}

int main(int argc, char *argv[])
{
    if(argc!=4) {
        printf("Error. Pls supply 3 arguments: 1) integer for seed 2) theta float 3) and number of customers.\n");
        printf("       theta float should be =1 for default run.\n");
        exit(EXIT_FAILURE);
    }
    srand(atoi(argv[1]));
    float thet=atof(argv[2]);
    size_t i, nc=atoi(argv[3]) /* number of customers */;

    tabarr *tbarr=NULL;
    runrest(nc, NULL, 0, thet, &tbarr); // this will be run nc number of times one suspects

    printf("Top tables (%zu in total): ", tbarr->nta); /* Table Idx "0" is not a real table */
    for(i=0;i<tbarr->nta;++i)
        printf("T%zu:%zu|", tbarr->movsa[i]+1, tbarr->els[i]);  /* note table 1 is indexed zero */
    printf("\n"); 

    free(tbarr->els);
    free(tbarr->movsa);
    free(tbarr);
    return 0;
}

/* Code for simulating the Chinese Restaurant Process
 * Only used on linux, and gcc compiler
 * Compile with gcc -Wall -g crp.c -o crp */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFINC 4

typedef struct /* tabarr, a struct to hold an array of tables in sorted order, together with an array of indices of the element's previous positions for external, labelling purposes */
{
    size_t *els; /* elements in ranked form */
    size_t *movsa; /* the movs array, shows what's gone where */
    size_t nels;
} tabarr;

typedef struct /* tarr, this is the operational table struct with a probabilities array, for internal purposes. The main() function doesn't see it */
{
    size_t *arr;
    float *p;
    size_t nels;
    size_t sumels;
    size_t buf;
} tarr;

void inita(tarr **ta, size_t statetables[], size_t nels, float alph) /* must be called with 3rd arg = zero for no tblaes occupied */
{
    int i;
    tarr *tta=*ta;
    tta=malloc(sizeof(tarr));
    tta->nels = nels+1; /* at this point nels now includes table 0 as our notional, always empty table */
    for(tta->buf=BUFINC; ;tta->buf +=BUFINC)
        if(tta->buf > tta->nels)
            break;

    tta->arr=malloc(tta->buf*sizeof(size_t));
    tta->p=malloc(tta->buf*sizeof(float));
    tta->arr[0]=0; /* the notional empty table: it's always zero */
    if(tta->nels >1) /* at 1, our stabletables[] is NULL, empty */
        memcpy(tta->arr+1, statetables, (tta->nels-1)*sizeof(size_t));

    tta->sumels=0;
    for(i=1;i<tta->nels;++i)
        tta->sumels += tta->arr[i];
    for(i=0;i<tta->nels;++i)
        tta->p[i] = (i==0)? alph/(alph+tta->sumels) : (float)tta->arr[i]/(alph+tta->sumels);

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

void upta(tarr **ta, size_t nels, float alph)
{
    int i;
    tarr *tta=*ta;
    if(nels > tta->nels) {
        if(nels == tta->buf-1) {
            tta->buf += BUFINC;
            tta->arr=realloc(tta->arr, tta->buf*sizeof(size_t));
            tta->p=realloc(tta->p, tta->buf*sizeof(float));
        }
        tta->nels=nels;
        tta->arr[tta->nels-1]=1; /* our new table gets its first customer */
    }
    tta->sumels = 0;
    for(i=1;i<tta->nels;++i)
        tta->sumels += tta->arr[i];
    for(i=0;i<tta->nels;++i)
        tta->p[i] = (i==0)? alph/(alph+tta->sumels) : (float)tta->arr[i]/(alph+tta->sumels);

    *ta=tta;
}

void throwdice(tarr *ta, float *cp[], size_t *cpsz, size_t *winner)
{
    int i;
    float pn, *tcp=*cp;

    if(*cpsz != ta->nels+1) {
        *cpsz= ta->nels+1;
        tcp=realloc(tcp, *cpsz*sizeof(float));
    }
    tcp[0]=.0;
    for(i=1;i<ta->nels+1;++i) 
        tcp[i]=ta->p[i-1]+tcp[i-1];

    pn=(float)rand()/RAND_MAX;
#ifdef DEBUG
    printf("Cumul probs before dice: "); 
    for(i=0;i<ta->nels+1;++i) 
        printf("%f ", tcp[i]); 
    printf("/ diceroll=%f ", pn); 
#endif
    for(i=1;i<ta->nels+1;++i) 
        if(pn<tcp[i])
            break;
    *winner=i-1;

    *cp=tcp;
}

void insort(tabarr *tbarr)
{
    int i, j; /* don't make i unsigned, please */
    size_t heldv /* held value */, shadheld;

    for(j=1;j<tbarr->nels;++j) {

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
    for(j=0;j<tbarr->nels;++j)
        printf("%zu ", tbarr->movsa[j]);
    printf("\n"); 
#endif
}

void runrest(size_t nc, size_t statearr[], size_t nels, float alph, tabarr **tbarr)
{
    size_t j, w, cpsz=0, *custs=malloc(nc*sizeof(size_t));
    float *cp=malloc(cpsz*sizeof(float));

    tarr *ta;
    inita(&ta, statearr, nels, alph);
#ifdef DEBUG
    printf("Chinese Restaurant Scheme: Table Idx 0 is notional, it causes a new table to be\ncreated and that table to have a single customer.\n"); 
    printf("The first iteration is trivial, as Table Idx 0 must be chosen, it's the only one.\n"); 
    printf("----------------------------\n"); 
#endif
    throwdice(ta, &cp, &cpsz, &w);
#ifdef DEBUG
    (w!=0)? printf(" / Result: %zu\n", w) : printf(" / w=0, so,new table, num: %zu\n", ta->nels); /* redundant check, yes, for clarity */
#endif
    if(w != 0) {
        ta->arr[w]++;
        upta(&ta, ta->nels, alph);
    } else
        upta(&ta, ta->nels+1, alph);

    custs[0]=w+1; /* OK, that's the trivial first customer dealt with */

    for(j=1;j<nc;++j) { /* now we consider the other customers */

        throwdice(ta, &cp, &cpsz, &w);
#ifdef DEBUG
        (w!=0)? printf(" / Result: %zu\n", w) : printf(" / w=0, so,new table, num: %zu\n", ta->nels);
#endif
        custs[j] = (w==0)? ta->nels : w;

        if(w != 0) {
            ta->arr[w]++;
            upta(&ta, ta->nels, alph);
        } else
            upta(&ta, ta->nels+1, alph);

    }

    tabarr *ttbarr=malloc(sizeof(tabarr));
    ttbarr->nels=ta->nels-1;
    ttbarr->els=malloc(ttbarr->nels*sizeof(size_t));
    memcpy(ttbarr->els, ta->arr+1, ttbarr->nels*sizeof(size_t));
    ttbarr->movsa=malloc(ttbarr->nels*sizeof(size_t));
    int i;
    for(i=0;i<ttbarr->nels;++i) 
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
        printf("Error. Pls supply 3 arguments: 1) integer for seed 2) alpha float 3) and number of customers.\n");
        exit(EXIT_FAILURE);
    }
    srand(atoi(argv[1]));
    float alph=atof(argv[2]);
    size_t i, nc=atoi(argv[3]);

    tabarr *tbarr;
    runrest(nc, NULL, 0, alph, &tbarr);

    printf("Top tables (%zu in total): ", tbarr->nels); /* Table Idx is not a real table */
    for(i=0;i<tbarr->nels;++i)
        printf("T%zu:%zu|", tbarr->movsa[i]+1, tbarr->els[i]);  /* note table 1 is indexed zero */
    printf("\n"); 

    free(tbarr->els);
    free(tbarr->movsa);
    free(tbarr);
    return 0;
}

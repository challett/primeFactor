#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>

#include <gmp.h>
#include <mpi.h>

int main(int argc, char** argv)
{

  int my_rank, p;
  int i, dest, source;
    int doneFlag = 0;
    mpz_t currentPrime;
    unsigned long int product = atoi(argv[1]);
    int secondFactor = 0;
    int iterations = 0;
    int bcastStatus;
    
    
    mpz_t N;
    mpz_t nextPrimeNumber;
    mpz_init_set_str (N, argv[1], 10);
    mpz_init(&nextPrimeNumber);
    mpz_init_set_ui(currentPrime, 2);
    mpz_nextprime(nextPrimeNumber, N);
    
    MPI_Request finalValue;
    
    //printf("N = %s , nextPrime: %d \n", mpz_get_str(NULL, 10, N), mpz_get_ui(nextPrimeNumber));
    
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


  MPI_Status status;
    
    clock_t start = clock(), diff;
    if (my_rank == 0) {
        MPI_Irecv(&secondFactor, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    } else {
        bcastStatus = MPI_Ibcast(&secondFactor, 1, MPI_INT, 0, MPI_COMM_WORLD, &finalValue);
    }
    
    while (!secondFactor) {
        if (!iterations) {
            iterations++;
            for (i=0 ; i < my_rank ; i++) {
                mpz_nextprime(currentPrime, currentPrime);
            }
        } else {
            for (i=0 ; i < p ; i++) {
                mpz_nextprime(currentPrime, currentPrime);
            }
        }
        
        //printf("currentPrime for %d = %d \n",my_rank,  mpz_get_ui(currentPrime));
        //printf("product \% mpz_get_ui(currentPrime) = %d \n", product % mpz_get_ui(currentPrime));
        if (product % mpz_get_ui(currentPrime) == 0) {
            doneFlag = 1;
            secondFactor = product / mpz_get_ui(currentPrime);
            printf("done by process %d, factors are %d and %d \n", my_rank, mpz_get_ui(currentPrime), secondFactor);
            MPI_Send(&secondFactor, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);

        }
//        MPI_Allreduce(
//                    &secondFactor,
//                    &secondFactor,
//                    1,
//                    MPI_INT,
//                    MPI_MAX,
//                    MPI_COMM_WORLD);
    }
    diff = clock() - start;
    if (my_rank == 0) {
        bcastStatus =MPI_Ibcast(&secondFactor, 1, MPI_INT, 0, MPI_COMM_WORLD, &finalValue);
    }
    
    int msec = diff * 1000 / CLOCKS_PER_SEC;
    printf("Time taken %d seconds %d milliseconds by process %d \n", msec/1000, msec%1000, my_rank);
    
    if (my_rank == 0) {
        MPI_Ibcast(&secondFactor, 1, MPI_INT, 0, MPI_COMM_WORLD, &finalValue);
    } else {
    
    }



  MPI_Finalize();
  return(0);
}

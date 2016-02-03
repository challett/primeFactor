#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

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
    int iterations =0;
    
    mpz_t N;
    mpz_t nextPrimeNumber;
    mpz_init_set_str (N, argv[1], 10);
    mpz_init(&nextPrimeNumber);
    mpz_init_set_ui(currentPrime, 1);
    mpz_nextprime(nextPrimeNumber, N);
    
    printf("N = %s , nextPrime: %d \n", mpz_get_str(NULL, 10, N), mpz_get_ui(nextPrimeNumber));
    
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


  MPI_Status status;
    
    while (!doneFlag) {
        for (i=0 ; i <= my_rank + iterations?p:0; i++) {
            mpz_nextprime(currentPrime, currentPrime);
        }
        printf("currentPrime for %d = %d \n",my_rank,  mpz_get_ui(currentPrime));
        printf("product \% mpz_get_ui(currentPrime) = %d \n", product % mpz_get_ui(currentPrime));
        if (product % mpz_get_ui(currentPrime) == 0) {
            doneFlag = 1;
            secondFactor = product / mpz_get_ui(currentPrime);
            printf("done by process %d, factors are %d and %d \n", my_rank, mpz_get_ui(currentPrime), secondFactor);
        }
            MPI_Allreduce(
                      &doneFlag,
                      &doneFlag,
                    1,
                      MPI_INT,
                      MPI_MAX,
                      MPI_COMM_WORLD);
        iterations++;
    }



  MPI_Finalize();
  return(0);
}

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
    mpz_t N;
    mpz_t nextPrimeNumber;
    mpz_init_set_str (N, argv[1], 10);
    mpz_init(&nextPrimeNumber);
    mpz_nextprime(nextPrimeNumber, N);
    
    printf("N = %s , nextPrime: %s \n", mpz_get_str (NULL, 10, N), mpz_get_str (NULL, 10, nextPrimeNumber));
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


  MPI_Status status;



  MPI_Finalize();
  return(0);
}

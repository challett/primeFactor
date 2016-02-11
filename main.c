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
  int i, dest;
  mpz_t currentPrime;
  unsigned long int product;
  sscanf(argv[1], "%lu", &product);
  int secondFactor = 0;
  int bcastStatus;
  int equals;

  /** GMP library variables **/
  mpz_t nextPrimeNumber;
  mpz_t testFactor;
  mpz_init(nextPrimeNumber);
  mpz_init_set_str (nextPrimeNumber, argv[1], 10);
  mpz_init(testFactor);
  mpz_init_set_ui(currentPrime, 2);
  mpz_nextprime(nextPrimeNumber, nextPrimeNumber);
  mpz_t testProduct;
  mpz_init(testProduct);

  /** MPI Initialization **/
  MPI_Request finalValue;
  MPI_File out;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Status status;

  /** Get Ready to receive a factor if another process finds one */
  MPI_Irecv(&secondFactor, 1, MPI_UNSIGNED_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &finalValue);
  
  /** Prepare initial offset for each process **/
  for (i=0 ; i < my_rank ; i++) {
    mpz_nextprime(currentPrime, currentPrime);
  }
  /** Start Timing **/
  double start = MPI_Wtime(), diff;
  while (!secondFactor) {
    /** Check if another process has found the factors **/
    MPI_Test (&finalValue, &bcastStatus, &status);
    if(bcastStatus) {
      /** Somebody else has found the factors, we are done **/
      MPI_Wait(&finalValue, &status);
      break;
    }
      /** Skip P primes before checking again **/
    for (i=0 ; i < p ; i++) {
      mpz_nextprime(currentPrime, currentPrime);
    }
    
    /** Brute force check if the current working prime is a factor of the input number **/
    for (mpz_set_ui(testFactor , 2) ; mpz_get_ui(testFactor) <= mpz_get_ui(currentPrime); mpz_nextprime(testFactor, testFactor)) {
      /** Check if another process has found the factors **/
      MPI_Test (&finalValue, &bcastStatus, &status);
      if(bcastStatus) {
        MPI_Wait(&finalValue, &status);
        break;
      }
      mpz_mul_ui(testProduct, currentPrime, mpz_get_ui(testFactor));
      equals = mpz_cmp_ui(testProduct, product);
      if (equals == 0){
        /** We've found the factor, find the second number, secnd it to the other processes  **/
        secondFactor = mpz_get_ui(testFactor);
        printf("done by process %d, factors are %lu and %d \n", my_rank, mpz_get_ui(currentPrime), secondFactor);
        fflush(stdout);
        for (dest = 0 ; dest < p ; dest++) {
          if (dest != my_rank) {
            MPI_Send(&secondFactor, 1, MPI_UNSIGNED_LONG, dest, 0, MPI_COMM_WORLD);
          }
        }
      }
    }
  }

  diff = MPI_Wtime() - start;
  /** End Timing **/

  /** Prepare file contents **/
  char fileName[200], fileContents[200];
  sprintf(fileName, "time_%lu", product);
  sprintf(fileContents, "%d\t%f\n", my_rank, diff);

  /** Write File **/
  MPI_File_open( MPI_COMM_WORLD, fileName, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &out );
  MPI_File_seek(out, my_rank*strlen ( fileContents ) , MPI_SEEK_SET);
  MPI_File_write_all(out , &fileContents, strlen ( fileContents ), MPI_CHAR, &status );
  MPI_File_close(&out);

  /** Fin **/
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return(0);
}

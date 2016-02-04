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
    int iterations = 0;
    int j;
    int bcastStatus;

    mpz_t N;
    mpz_t nextPrimeNumber;
    mpz_t testFactor;
    mpz_init_set_str (N, argv[1], 10);
    mpz_init(&nextPrimeNumber);
    mpz_init(&testFactor);
    mpz_init_set_ui(currentPrime, 2);
    mpz_nextprime(nextPrimeNumber, N);

    unsigned long int squareRoot;
    squareRoot =  sqrt(mpz_get_ui(N));

    MPI_Request finalValue;
    MPI_File out;

    //printf("N = %s , nextPrime: %d \n", mpz_get_str(NULL, 10, N), mpz_get_ui(nextPrimeNumber));

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


  MPI_Status status;

    MPI_Irecv(&secondFactor, 1, MPI_UNSIGNED_LONG, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &finalValue);
    double start = MPI_Wtime(), diff;
    while (!secondFactor) {
        MPI_Test (
             &finalValue,
             &bcastStatus,
             &status);
             if(bcastStatus) {
               MPI_Wait(&finalValue, &status);
               break;
             }
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

        for (mpz_set_ui(testFactor , 2) ; mpz_get_ui(testFactor) <= mpz_get_ui(currentPrime); mpz_nextprime(testFactor, testFactor)) {

          if (mpz_get_ui(currentPrime) * mpz_get_ui(testFactor) == product){
            fflush(stdout);
              secondFactor = product / mpz_get_ui(currentPrime);
              printf("done by process %d, factors are %d and %d \n", my_rank, mpz_get_ui(currentPrime), secondFactor);
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

    char fileName[200], fileContents[200];
    sprintf(fileName, "time_%d", product);
    sprintf(fileContents, "%d\t%f\n", my_rank, diff);

    printf("Time taken %f by %d \n", diff, my_rank);
    MPI_File_open( MPI_COMM_WORLD, fileName, MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &out );
    MPI_File_seek(out, my_rank*strlen ( &fileContents ) , MPI_SEEK_SET);

    MPI_File_write_all(out , &fileContents, strlen ( &fileContents ), MPI_CHAR, &status );

  MPI_Finalize();
  return(0);
}

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

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);


  MPI_Status status;



  MPI_Finalize();
  return(0);
}

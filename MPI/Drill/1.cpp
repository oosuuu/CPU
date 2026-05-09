#include <mpi.h>
#include <iostream>
using namespace std;

#define TS 16      //totalsize
#define MS (TS/1)  //mysize
#define STEPS 10

int main(int argc,char** argv){

    int myid,numprocs,i,j,n;
    int a[TS][TS], b[TS][TS];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    //Initialization
    for (i = 0; i < TS; i++){
        for (j = 0; j < TS; j++){
            a[i][j] = 0;
        }
    }
    for (i = 0; i < TS; i++){
        a[i][0] = 8.0;
        a[i][TS-1] = 8.0;
    }
    for (j = 0; j < TS; j++){
        a[0][j] = 8.0;
        a[TS-1][j] = 8.0;
    }

    //Jacobi
    for (n = 0; n < STEPS; n++){

        for (i = 1; i < TS - 1; i++){
            for (j = 1; j < TS - 1; j++){
                b[i][j] = (a[i][j + 1] + a[i][j - 1] + a[i + 1][j] + a[i - 1][j]) * 0.25;
            }
        }

        for (i = 1; i < TS - 1; i++){
            for (j = 1; j < TS - 1; j++){
                a[i][j] = b[i][j];
            }
        }

    }

    for(i = 0; i < TS; i++){
         for (j = 0; j < TS; j++){
             cout << a[i][j] << "\t";
         }
         cout << "\n"
              << endl;
     }

    MPI_Finalize();
    return 0;
}
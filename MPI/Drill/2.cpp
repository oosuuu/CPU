#include <mpi.h>
#include <iostream>
#include <iomanip>
using namespace std;

#define TS 16      //totalsize
#define MS (TS/2)  //mysize
#define STEPS 10

int main(int argc,char** argv){

    int myid,numprocs,i,j,n,k;
    int tag0 = 1;
    int tag1 = 2;
    int begin_col, end_col;
    double a[TS][MS + 2], b[TS][MS + 2];
    double send_buf0[TS], send_buf1[TS],recv_buf0[TS],recv_buf1[TS];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Status status;

    cout << "Process " << myid << " started" << endl;

    //Initialization
    for (i = 0; i < TS; i++){
        for (j = 0; j < MS+2; j++){
        a[i][j] = 0;
        }
    }
    if(myid==0){
        for (i = 0; i < TS; i++){
            a[i][0] = 8.0;
            a[i][1] = 8.0;
        }
    }
    if(myid==1){
        for (i = 0; i < TS; i++){
            a[i][MS+1] = 8.0;
            a[i][MS] = 8.0;
        }
    }
    for(j = 0; j < MS + 2; j++){
        a[0][j] = 8.0;
        a[TS-1][j] = 8.0;
    }

    // Jacobi
    for (n = 0; n < STEPS; n++){

       if (myid == 0){
           for (i = 0; i < TS; i++){
               send_buf0[i] = a[i][MS];
           }
           MPI_Send(send_buf0, TS, MPI_DOUBLE, 1, tag0,MPI_COMM_WORLD);
           MPI_Recv(recv_buf0, TS, MPI_DOUBLE, 1, tag1, MPI_COMM_WORLD, &status);
       }
       if (myid == 1){
           for (i = 0; i < TS; i++){
               send_buf1[i] = a[i][1];
           }
           MPI_Recv(recv_buf1, TS, MPI_DOUBLE, 0, tag0, MPI_COMM_WORLD, &status);
           MPI_Send(send_buf1, TS, MPI_DOUBLE, 0, tag1,MPI_COMM_WORLD);
       }

       if (myid == 0){
           for (i = 0; i < TS; i++){
               a[i][MS+1] = recv_buf0[i];
           }
       }
       if (myid == 1){
           for (i = 0; i < TS; i++){
               a[i][0] = recv_buf1[i];
           }
       }

       if(myid==0){
           begin_col = 2;
           end_col = 8;
       }
       if(myid==1){
           begin_col = 1;
           end_col = 7;
       }
       // update
       for (i = 1; i < TS - 1; i++){
           for (j = begin_col; j < end_col+1; j++){
               b[i][j] = (a[i][j + 1] + a[i][j - 1] + a[i + 1][j] + a[i - 1][j]) * 0.25;
           }
       }
       for (i = 1; i < TS - 1; i++){
           for (j = begin_col; j < end_col + 1; j++){
               a[i][j] = b[i][j];
           }
       }
    }

    //output
    for (k = 0; k < 2;k++){
        if(myid==k){
            for (i = 0; i < TS; i++){
                for (j = 1; j < MS + 1; j++){
                    cout <<setw(10)<< a[i][j] ;
                }cout << "\n"<< endl;
            }
        }MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

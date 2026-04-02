#include <mpi.h>
#include <iostream>
using namespace std;

#define TS 16      //totalsize
#define MS (TS/4)  //mysize
#define STEPS 10

int main(int argc,char** argv){

    int myid,numprocs,i,j,n;
    int begin_row,end_row;
    MPI_Status status;

    double a[MS + 2][TS];
    double b[MS + 2][TS];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    cout<<"Process "<<myid<<" of "<<numprocs<<" is alive!"<<endl;

    MPI_Barrier(MPI_COMM_WORLD);

    //Initialization
    for (i = 0; i < MS + 2; i++){
        for (j = 0; j < TS; j++){
            a[i][j] = 0.0;
        }
    }

    if (myid == 0){
        for (j = 0; j < TS; j++){
            a[1][j] = 8.0;         //row 1
        }
    }

    if (myid == 3){
        for (j = 0; j < TS; j++){
            a[MS][j] = 8.0;       //row 4
        }
    }

    for (j = 0; j < TS; j++){
        a[0][j] = 8.0;
        a[MS + 1][j] = 8.0;
    }

    //Jacobi
    for (n = 0; n < STEPS; n++){
        //down to up
        if (myid > 0){
            MPI_Send(&a[0][0], TS, MPI_DOUBLE, myid - 1, 10, MPI_COMM_WORLD);
        }
        if (myid < 3){
            MPI_Recv(&a[MS + 1][0], TS, MPI_DOUBLE, myid + 1, 10, MPI_COMM_WORLD, &status);
        }

        //up to down
        if (myid > 0){
            MPI_Recv(&a[0][0], TS, MPI_DOUBLE, myid - 1, 10, MPI_COMM_WORLD, &status);
        }
        if (myid < 3){
            MPI_Send(&a[MS][0], TS, MPI_DOUBLE, myid + 1, 10, MPI_COMM_WORLD);
        }

        //caculation range
        begin_row = 1;
        end_row = MS + 1;

        if (myid == 0){
            begin_row = 2;
        }
        if (myid == 3){
            end_row = MS;
        }

        //update
        for (i = begin_row; i < end_row; i++){
            for (j = 1; j < TS - 1; j++){
                b[i][j] = (a[i][j + 1] + a[i][j - 1] + a[i + 1][j] + a[i - 1][j]) / 4.0;
            }
        }
        for (i = begin_row; i < end_row; i++){
            for (j = 1; j < TS - 1; j++){
                a[i][j] = b[i][j];
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
        //output
        cout << "Process " << myid << ":\n";
        for (i = 1; i <= MS; i++){
            for (j = 0; j < TS; j++)
            {
                cout << "a[" << i << "][" << j << "] = " << a[i][j] << "\t";
            }cout<<"\n"<<endl;
        }
    

        MPI_Finalize();
    return 0;
}
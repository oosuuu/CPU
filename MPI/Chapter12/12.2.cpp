#include <mpi.h>
#include <iostream>
using namespace std;

#define TS 16      //totalsize
#define MS (TS/4)  //mysize
#define STEPS 10

int main(int argc,char** argv){
    int myid,numprocs,i,j,n;
    int begin_row,end_row;
    int up,down,tag1,tag2;
    MPI_Status status[4];
    MPI_Request req[4];

    double a[MS + 2][TS];
    double b[MS + 2][TS];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    cout<<"Process "<<myid<<" of "<<numprocs<<" is alive!"<<endl;

    // Initialization
    for (i = 0; i < MS + 2; i++){
        for (j = 0; j < TS; j++){
            a[i][j] = 0.0;
        }
    }
    //Boundary    
    for (j = 0; j < TS; j++){
        a[0][j] = 8.0;
        a[MS + 1][j] = 8.0;
    }

    for (i = 0; i < MS + 2; i++)
    {
        a[i][0] = 8.0;
        a[i][TS - 1] = 8.0;
    }

    if (myid == 0){
        for (j = 0; j < TS; j++)
        {
            a[1][j] = 8.0; // row 1
        }
    }
    if (myid == 3){
        for (j = 0; j < TS; j++){
            a[MS][j] = 8.0;       //row 4
        }
    }

    //for (i = 0; i < MS + 2;i++){
    //    for (j = 0; j < TS;j++){
    //        cout << a[i][j] << "\t";
    //    }
    //    cout << "\n"
    //         << endl;
    //}
    
    tag1 = 3;
    tag2 = 4;

    //set rank
    if (myid == 0){
        up = MPI_PROC_NULL;
    }
    else{
        up = myid - 1;
    }
    
    if (myid == 3){
        down = MPI_PROC_NULL;
    }
    else{
        down = myid + 1;
    }

    //set begin_row and end_row
    begin_row = 1;
    end_row = MS;
    if (myid == 0){
        begin_row = 2;
    }
    if (myid == 3){
        end_row = MS - 1;
    }

    MPI_Send_init(&b[end_row][1], TS-2, MPI_DOUBLE, down, tag1, MPI_COMM_WORLD, &req[0]);
    MPI_Send_init(&b[begin_row][1], TS-2, MPI_DOUBLE, up, tag2, MPI_COMM_WORLD, &req[1]);
    MPI_Recv_init(&a[0][1], TS-2, MPI_DOUBLE, up, tag1, MPI_COMM_WORLD, &req[2]);
    MPI_Recv_init(&a[MS+1][1], TS-2, MPI_DOUBLE, down, tag2, MPI_COMM_WORLD, &req[3]);

    for (n = 0; n < STEPS; n++){
        for (j = 1; j < TS - 1; j++){
            b[begin_row][j] = (a[begin_row][j + 1] + a[begin_row][j - 1] + a[begin_row + 1][j] + a[begin_row - 1][j]) * 0.25;
            b[end_row][j] = (a[end_row][j + 1] + a[end_row][j - 1] + a[end_row + 1][j] + a[end_row - 1][j]) * 0.25;
        }

        MPI_Startall(4, req);

        for (j = 1; j < TS-1; j++){
            for (i = begin_row + 1; i < end_row; i++){
                b[i][j] = (a[i][j + 1] + a[i][j - 1] + a[i + 1][j] + a[i - 1][j]) * 0.25;
            }
        }
        //update
        for (i = begin_row; i < end_row + 1; i++){
            for (j = 1; j < TS-1; j++){
                a[i][j] = b[i][j];
            }
        }

        MPI_Waitall(4, req, status);
    }
    for (i = 0; i < 4; i++){
        MPI_Request_free(&req[i]);
    }

    cout << "Process " << myid << ":" << endl;
   for (i = 1; i < MS + 1; i++)
    {
        for (j = 0; j < TS;j++){
            cout << a[i][j] << "\t";
        }
        cout << "\n"
             << endl;
    }
    
    MPI_Finalize();
}
#include <iostream>
#include <mpi.h>
#define SIZE 2000
using namespace std;
int main(int argc,char** argv){
    MPI_Init(&argc, &argv);
    int prev, next, count, tag, index, i, outcount;
    int myid, np;
    int send_buf[SIZE], recv_buf[SIZE];
    MPI_Status status;
    MPI_Request request;
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if(np!=2){
        cout << "This program uses exactly 2 processes!" << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    next = myid + 1;
    if(next>=np){
        next = 0;
    }
    if(prev<0){
        prev = np - 1;
    }

    tag = 1456;
    count = SIZE / 3;

    if(myid==0){
        MPI_Recv(MPI_BOTTOM, 0, MPI_INT, next, tag, MPI_COMM_WORLD, &status);
        cout << "Process " << myid << " post Ready send" << endl;
        MPI_Rsend(send_buf, count, MPI_INT, next, tag, MPI_COMM_WORLD);
    }else{
        cout << "Process " << myid << " post a receive call" << endl;
        MPI_Irecv(recv_buf, SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
        MPI_Send(MPI_BOTTOM, 0, MPI_INT, next, tag, MPI_COMM_WORLD);
        MPI_Wait(&request, &status);
        cout << "Process " << myid << " receive Rsend message from " << status.MPI_SOURCE << endl;
    }

    MPI_Finalize();
}
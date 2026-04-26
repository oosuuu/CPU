#include <iostream>
#include <mpi.h>
#define SIZE 10
using namespace std;

static int src = 0;
static int dest = 1;
int main(int argc,char** argv){
    int myid;
    int flag, nump;
    int buffer[SIZE];
    int act_size = 0;
    MPI_Status status, status1, status2;
    int count1,count2;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Comm_size(MPI_COMM_WORLD, &nump);

    if(nump!=2){
        cout << "This program uses exactly 2 processes!" << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    act_size = 5;
    //发送进程
    if(myid==src){
        act_size = 1;
        MPI_Ssend(buffer, act_size, MPI_INT, dest, 1, MPI_COMM_WORLD);
        cout << "MPI_Ssend sent " << act_size << " data,tag=1" << endl;

        act_size = 4;
        MPI_Ssend(buffer, act_size, MPI_INT, dest, 2, MPI_COMM_WORLD);
        cout << "MPI_Ssend sent " << act_size << " data,tag=2" << endl;
    }
    //接收进程
    else if(myid==dest){
        MPI_Recv(buffer, act_size, MPI_INT, src, 1, MPI_COMM_WORLD, &status1);
        MPI_Recv(buffer, act_size, MPI_INT, src, 2, MPI_COMM_WORLD, &status2);
        MPI_Get_count(&status1, MPI_INT, &count1);
        cout << "Receive " << count1 << " data,tag= " << status1.MPI_TAG << endl;
        MPI_Get_count(&status2, MPI_INT, &count2);
        cout << "Receive " << count2 << " data,tag= " << status2.MPI_TAG << endl;
    }
    MPI_Finalize();
}
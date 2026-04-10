#include <iostream>
#include <mpi.h>
using namespace std;

void call_master();   //漏
void call_slave();


int main(int argc,char**argv){
    int myid, numprocs;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if (myid == 0){
        call_master();
    }else{
        call_slave();
    }

    MPI_Finalize();
}

#define MSG_EXIT 1       //宏不加分号
#define MSG_ORDER 2
#define MSG_UNORDER 3

void call_master(){
    int i, j,numprocs, nslave, msg;
    char buf1[256], buf2[256];
    MPI_Status status;
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    nslave = numprocs - 1;
    while (nslave > 0){
        MPI_Recv(buf1, 256, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch(status.MPI_TAG){
            case 1:
                nslave--;
                break;
            case 3:
                fputs(buf1, stdout);
                break;
            case 2:
                msg = status.MPI_SOURCE;
                for (i = 1; i < numprocs; i++){
                    if (i == msg){
                        fputs(buf1, stdout);
                    }else{
                        MPI_Recv(buf2, 256, MPI_CHAR, i, MSG_ORDER, MPI_COMM_WORLD, &status);
                        fputs(buf2, stdout);
                    }
                }
                break;
        }
    }
}

void call_slave(){
    char buf[256];
    int myid;
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    sprintf(buf, "Hello from slave %d,ordered print\n", myid);
    MPI_Send(buf, strlen(buf) + 1, MPI_CHAR, 0, MSG_ORDER, MPI_COMM_WORLD);

    sprintf(buf, "Goodbye from slave %d, ordered print\n", myid);
    MPI_Send(buf, strlen(buf) + 1, MPI_CHAR, 0, MSG_ORDER, MPI_COMM_WORLD);

    sprintf(buf, "I am exiting %d,unordered print\n", myid);
    MPI_Send(buf, strlen(buf) + 1, MPI_CHAR, 0, MSG_UNORDER, MPI_COMM_WORLD);

    MPI_Send(buf, 0, MPI_CHAR, 0, MSG_EXIT, MPI_COMM_WORLD);
}
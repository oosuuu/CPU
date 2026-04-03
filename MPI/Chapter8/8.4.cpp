#include <mpi.h>
#include <iostream>
#include <algorithm>
using namespace std;

#define MAX_ROW 1000
#define MAX_COL 1000

int main(int argc,char** argv){
    double a[MAX_ROW][MAX_COL];
    double b[MAX_COL];
    double c[MAX_COL];
    double buf[MAX_COL];
    double ans;

    int rows = 100, cols = 100;
    int myid,numprocs;
    int numsent, numrcvd, i, j, sender;
    int master = 0;
    int anstype, row;

    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    if(myid==master){
        //主进程对矩阵AB赋值.A每行值为行数，B值全为1.
        for (i = 0; i < rows; i++){
            b[i] = 1;
            for (j = 0; j < cols; j++){
                a[i][j] = i;
            }
        }
        numsent = 0, numrcvd = 0;

        //广播B给从进程
        MPI_Bcast(b, cols, MPI_DOUBLE, master, MPI_COMM_WORLD);

        // 将矩阵A各行发送给从进程
        for (i = 0; i < min(numprocs - 1, rows); i++){
            MPI_Send(&a[i][0], cols, MPI_DOUBLE, i, i, MPI_COMM_WORLD);
            numsent++;
        }

        //接受从进程计算结果
        for (i = 0; i < rows; i++){
            MPI_Recv(&ans, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            sender = status.MPI_SOURCE;  //哪个从进程发过来的
            anstype = status.MPI_TAG;    //第几行的结果
            //将计算结果存入c
            c[anstype] = ans;

            if (numsent < rows){
                MPI_Send(&a[numsent][0], cols, MPI_DOUBLE, sender, numsent, MPI_COMM_WORLD);
                numsent++;
            }else{
                MPI_Send(buf, 0, MPI_DOUBLE, sender, 0, MPI_COMM_WORLD);
            }
        }
    }
    //从进程执行
    else{
        //接收B
        MPI_Bcast(b, cols, MPI_DOUBLE, master, MPI_COMM_WORLD);

        while(1){

            //接受A的某行
            MPI_Recv(buf, cols, MPI_DOUBLE, master, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            row = status.MPI_TAG;
            ans = 0.0;
    
            if (status.MPI_TAG == 0){
                break;}

            // 计算点积
            for (i = 0; i < cols; i++){
                    ans += b[i] * buf[i];
                }

            //将结果返回主进程
            MPI_Send(&ans, 1, MPI_DOUBLE, master, row, MPI_COMM_WORLD);
        }

    }
    MPI_Finalize();
    return 0;
}

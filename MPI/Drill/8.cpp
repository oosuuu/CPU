#include <mpi.h>
#include <iomanip>
#include <iostream>
using namespace std;

#define TS 16      //totalsize
#define CS (TS/2)  //col size lie
#define RS (TS/4)  //row size hang
#define STEPS 10

int main(int argc,char** argv){

    int myid,numprocs,i,j,n,k;
    int begin_row,end_row;
    int begin_col,end_col;
    int up, down, left, right;
    MPI_Status status;
    double send_buf[6], recv_buf[6];

    double a[RS+2][CS+2];
    double b[RS+2][CS+2];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    //Initialization
    for (i = 0; i < RS + 2; i++){
        for (j = 0; j < CS + 2; j++){
            a[i][j] = 0.0;
        }
    }
    if (myid == 0||myid==4){
        for (j = 0; j < CS + 2; j++){
            a[0][j] = 8.0;
            a[1][j] = 8.0;
        }
    }
    if (myid == 3||myid==7){
        for (j = 0; j < CS + 2; j++){
            a[RS+1][j] = 8.0;
            a[RS][j] = 8.0;
        }
    }
    if(myid==0||myid==1||myid==2||myid==3){
        for (i = 0; i < RS + 2; i++){
            a[i][0] = 8.0;
            a[i][1] = 8.0;
        }
    }
    if(myid==4||myid==5||myid==6||myid==7){
        for (i = 0; i < RS + 2; i++){
            a[i][CS] = 8.0;
            a[i][CS+1] = 8.0;
        }
    }

    //set rank
    up = myid - 1;
    down = myid + 1;
    left = myid - 4;
    right = myid + 4;
    begin_row = 1;
    end_row = RS;
    begin_col = 1;
    end_col = CS;
    if (myid == 0){
        begin_col = 2;
        begin_row = 2;
        up = MPI_PROC_NULL;
    }
    if(myid==1||myid==2){
        begin_col = 2;
    }
    if(myid==3){
        end_row = RS-1;
        begin_col = 2;
        down = MPI_PROC_NULL;
    }
    if(myid==4){
        begin_row = 2;
        end_col = CS - 1;
        up = MPI_PROC_NULL;
    }
    if(myid==5||myid==6){
        end_col = CS - 1;
    }
    if(myid==7){
        end_row = RS-1;
        end_col = CS - 1;
        down = MPI_PROC_NULL;
    }

    // Jacobi
    for (n = 0; n < STEPS; n++){

        //left with right
        for (k = 0; k < 4; k++){
            if(myid==k){
                for (i = 0; i < RS + 2; i++){
                    send_buf[i] = a[i][CS];
                }
                left = MPI_PROC_NULL;
                MPI_Sendrecv(send_buf,RS+2,MPI_DOUBLE,right,6,recv_buf,RS+2,MPI_DOUBLE,right,6,MPI_COMM_WORLD,&status);
                for (i = 0; i < RS + 2; i++){
                    a[i][CS + 1]=recv_buf[i];
                }
            }
        }
        for (k = 4; k < 8; k++){
            if(myid==k){
                for (i = 0; i < RS + 2; i++){
                    send_buf[i] = a[i][1];
                }
                right = MPI_PROC_NULL;
                MPI_Sendrecv(send_buf,RS+2,MPI_DOUBLE,left,6,recv_buf,RS+2,MPI_DOUBLE,left,6,MPI_COMM_WORLD,&status);
                for (i = 0; i < RS + 2; i++){
                    a[i][0]=recv_buf[i];
                }
            }
        }
        
        //down
        MPI_Sendrecv(&a[RS][0],CS+2,MPI_DOUBLE,down,10,&a[RS+1][0],CS+2,MPI_DOUBLE,down,10,MPI_COMM_WORLD,&status);
        //up
        MPI_Sendrecv(&a[1][0],CS+2,MPI_DOUBLE,up,10,&a[0][0],CS+2,MPI_DOUBLE,up,10,MPI_COMM_WORLD,&status);

        for (i = begin_row; i < end_row+1;i++){
            for (j = begin_col; j < end_col+1;j++){
                b[i][j] = (a[i][j + 1] + a[i][j - 1] + a[i + 1][j] + a[i - 1][j]) * 0.25;
            }
        }
        for (i = begin_row; i < end_row+1;i++){
            for (j = begin_col; j < end_col+1;j++){
                a[i][j] = b[i][j];
            }
        }
    }

    // output
    //for (k = 0; k < 4; k++){
    //    if (myid == k){
    //        for (i = 1; i < RS + 1; i++){
    //            for (j = 1; j < CS + 1; j++){
    //                cout << setw(10) << a[i][j];
    //            }
    //            cout << "\n"<< endl;
    //        }
    //    }MPI_Barrier(MPI_COMM_WORLD);
    //}
    //for (k = 4; k < 8;k++){
    //    if(myid==k){
    //        for (i = 1; i < RS + 1; i++){
    //            for (j = 1; j < CS + 1; j++){
    //                cout <<setw(10)<< a[i][j] ;
    //            }cout << "\n"<< endl;
    //        }
    //    }
    //    MPI_Barrier(MPI_COMM_WORLD);
    //}

    if(myid==7){
        for (i = 1; i < RS + 1; i++){
            for (j = 1; j < CS + 1; j++){
                  cout << setw(10) << a[i][j];
            }cout << "\n"<< endl;
        }
    }

    MPI_Finalize();
    return 0;
}
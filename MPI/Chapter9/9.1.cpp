#include <iostream>
#include <mpi.h>
using namespace std;

#define SIZE 6
static int src = 0;
static int dest = 1;

void Gererate(double *, int);
void Normal_recv(double *, int);
void Buffered_send(double *, int);

void Generate(double* buffer,int buf_size){
    for (int i = 0; i < buf_size; i++){
        buffer[i] = (double)i + 1;
    }
}

void Normal_recv(double*buffer,int buf_size){
    MPI_Status status;

    double *b;
    b = buffer;
    MPI_Recv(b, (buf_size - 1), MPI_DOUBLE, src, 2000, MPI_COMM_WORLD, &status);
    cout << "standard receive a message of " << buf_size - 1 << " data" << endl;
    for (int j = 0; j < buf_size - 1;j++){
        cout << "buf[" << j << "] = " << b[j] << endl;
    }
    b += buf_size - 1;
    MPI_Recv(b, 1, MPI_DOUBLE, src, 2000, MPI_COMM_WORLD, &status);
    cout << "standard receive a message of one data" << endl;
    cout << "buf[0] = " << *b << endl;
}

void Buffered_send(double*buffer,int buf_size){
    void **bbuffer;
    int size;
    //先发送5个
    cout << "buffered send message of " << buf_size - 1 << " data" << endl;
    for (int j = 0; j < buf_size - 1;j++){
        cout << "buf[" << j << "] = " << buffer[j] << endl;
    }
    MPI_Bsend(buffer, (buf_size - 1), MPI_DOUBLE, dest, 2000, MPI_COMM_WORLD);

    //再发送1个
    buffer += buf_size - 1;
    cout << "buffered send message of one data"<<endl;
    cout << "buf[0] = " << *buffer << endl;
    MPI_Bsend(buffer, 1, MPI_DOUBLE, dest, 2000, MPI_COMM_WORLD);

    MPI_Buffer_detach(&buffer, &size);
    MPI_Buffer_attach(bbuffer, size);
}

int main(int argc,char** argv){
    int myid;
    double buffer[SIZE], *tmpbuf;
    int tsize, bsize;
    char *Current = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if(myid==src){
        Generate(buffer, SIZE);        //产生数据
        MPI_Pack_size(SIZE, MPI_DOUBLE, MPI_COMM_WORLD, &bsize);    //计算所需字节数并存放在bsize
        //tmpbuffer =(double*) malloc(bsize + 2 * MPI_BSEND_OVERHEAD);
        char *tmpbuffer = new char[bsize + 2 * MPI_BSEND_OVERHEAD];
        // 检查tmpbuffer是否被分配内存空间
        if(!tmpbuffer){
            cout << "Could not allocate bsend buffer of size " << bsize << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        MPI_Buffer_attach(tmpbuffer, bsize + MPI_BSEND_OVERHEAD);   //申请空间
        Buffered_send(buffer, SIZE);
        MPI_Buffer_detach(&tmpbuf, &tsize);
    }else if(myid==dest){
        Normal_recv(buffer, SIZE);
    }else{
        cout << "This program uses exactly 2 processses!" << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Finalize();
}
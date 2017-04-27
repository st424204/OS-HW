#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <mpi.h>
#include <iostream>

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    if( argc != 2)
    {
        cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
        return -1;
    }

    int my_rank,num_procs;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&num_procs);

    if(my_rank == 0){

		Mat image;
		image = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);   // Read the file

		if(! image.data )                              // Check for invalid input
		{
			cout <<  "Could not open or find the image" << std::endl ;
			return -1;
		}

		int height = image.rows;
		int width = image.cols;
		int type = image.type();

		int block_h = height / (num_procs-1) ;

		for (int other_rank = 1; other_rank < num_procs-1; other_rank++)
		{
			MPI_Send(&block_h,1, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
			MPI_Send(&width,1, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
			MPI_Send(&type,1, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
			MPI_Send(image.data+block_h*(other_rank-1)*width*1,block_h*width*1, MPI_BYTE, other_rank, 0, MPI_COMM_WORLD);

			if(other_rank==1) MPI_Send(image.data,width*1, MPI_BYTE, other_rank, 0, MPI_COMM_WORLD);
			else MPI_Send(image.data+block_h*(other_rank-1)*width*1-width*1,width*1, MPI_BYTE, other_rank, 0, MPI_COMM_WORLD);
			
			MPI_Send(image.data+block_h*(other_rank)*width*1,width*1, MPI_BYTE, other_rank, 0, MPI_COMM_WORLD);
			
		}

		// last process	
		int last_h = height-(block_h*(num_procs-2));
		MPI_Send(&last_h,1, MPI_INT, num_procs-1, 0, MPI_COMM_WORLD);
		MPI_Send(&width,1, MPI_INT, num_procs-1, 0, MPI_COMM_WORLD);
		MPI_Send(&type,1, MPI_INT, num_procs-1, 0, MPI_COMM_WORLD);
		MPI_Send(image.data+block_h*(num_procs-2)*width*1,last_h*width*1, MPI_BYTE, num_procs-1, 0, MPI_COMM_WORLD);
		
		MPI_Send(image.data+block_h*(num_procs-2)*width*1-width*1,width*1, MPI_BYTE, num_procs-1, 0, MPI_COMM_WORLD);
		MPI_Send(image.data+(height-1)*width*1,width*1, MPI_BYTE, other_rank, 0, MPI_COMM_WORLD);


		Mat image2(Size(width,height),type);

		for (int other_rank = 1; other_rank < num_procs-1; other_rank++)
			MPI_Recv(image2.data+block_h*(other_rank-1)*width*1,block_h*width*1, MPI_BYTE, other_rank,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		// last process	
		MPI_Recv(image2.data+block_h*(num_procs-2)*width*1,last_h*width*1, MPI_BYTE, num_procs-1,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		imwrite("gray_blur.jpg",image2);

    }
    else{

		int height=0,width=0,type=0;
		MPI_Recv(&height, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&width, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&type, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		Mat refer(Size(width,height),type);
		MPI_Recv(refer.data+width*1, height*width*1, MPI_BYTE, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
				
		MPI_Recv(refer.data, 1*width*1, MPI_BYTE, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(refer.data+(height-1)*width*1, 1*width*1, MPI_BYTE, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		
		Mat image(Size(width,height),type);
		int weight[3][3]={1,2,1,2,4,2,1,2,1};
		for(int i=0; i < height;i++){
			for(int j=0;j<width;j++){
				int val = 0,total=0;
				for(int a=0;a<3;a++){
					for(int b=0;b<3;b++){
						int x=i-1+1+a,y=j-1+b;
						if(y >= width) y = width-1;
						if(y<0) y=0;
						val+=refer.at<uchar>(x,y)*weight[a][b];
						total+=weight[a][b];
					}
				}
				image.at<uchar>(i,j) = val/total;
			}
		}
		MPI_Send(image.data,height*width*1, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}

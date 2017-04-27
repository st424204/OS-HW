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
    	image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

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
			MPI_Send(image.data+block_h*(other_rank-1)*width*3,block_h*width*3, MPI_BYTE, other_rank, 0, MPI_COMM_WORLD);
		}

		// last process	
		int last_h = height-(block_h*(num_procs-2));
		MPI_Send(&last_h,1, MPI_INT, num_procs-1, 0, MPI_COMM_WORLD);
		MPI_Send(&width,1, MPI_INT, num_procs-1, 0, MPI_COMM_WORLD);
		MPI_Send(&type,1, MPI_INT, num_procs-1, 0, MPI_COMM_WORLD);
		MPI_Send(image.data+block_h*(num_procs-2)*width*3,last_h*width*3, MPI_BYTE, num_procs-1, 0, MPI_COMM_WORLD);


		Mat image2(Size(width,height),type);

		for (int other_rank = 1; other_rank < num_procs-1; other_rank++)
			MPI_Recv(image2.data+block_h*(other_rank-1)*width*3,block_h*width*3, MPI_BYTE, other_rank,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		// last process	
		MPI_Recv(image2.data+block_h*(num_procs-2)*width*3,last_h*width*3, MPI_BYTE, num_procs-1,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		imwrite("color_blur.jpg",image2);

    }
    else{

		int height=0,width=0,type=0;
		MPI_Recv(&height, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&width, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&type, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		Mat refer(Size(width,height),type);
		MPI_Recv(refer.data, height*width*3, MPI_BYTE, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		Mat image(Size(width,height),type);
		int weight[3][3]={1,2,1,2,4,2,1,2,1};
		for(int i=0; i < height;i++){
			for(int j=0;j<width;j++){
				int val[3] = {0,0,0},total=0;
				for(int a=0;a<3;a++){
					for(int b=0;b<3;b++){
						int x=i-1+a,y=j-1+b;
						if(x >= height) x = height-1;
						if(y >= width) y = width-1;
						if(x<0) x=0;
						if(y<0) y=0;
						val[0]+=refer.at<Vec3b>(x,y)[0]*weight[a][b];
						val[1]+=refer.at<Vec3b>(x,y)[1]*weight[a][b];
						val[2]+=refer.at<Vec3b>(x,y)[2]*weight[a][b];
						total+=weight[a][b];
					}
				}
				image.at<Vec3b>(i,j)[0] = val[0]/total;
				image.at<Vec3b>(i,j)[1] = val[1]/total;
				image.at<Vec3b>(i,j)[2] = val[2]/total;
			}
		}
		MPI_Send(image.data,height*width*3, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}

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

	int block_w = width / (num_procs-1) ;

	for (int other_rank = 1; other_rank < num_procs-1; other_rank++)
        {	
		Mat im(Size(block_w,height),type);
		
		for(int i=0;i<height;i++)
			for(int j=0;j<block_w;j++){
				im.at<uchar>(i,j) = image.at<uchar>(i,j+(other_rank-1)*block_w);
                          
			}
				
		MPI_Send(&height,1, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
		MPI_Send(&block_w,1, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
		MPI_Send(&type,1, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
		MPI_Send(im.data,block_w*height*1, MPI_BYTE, other_rank, 0, MPI_COMM_WORLD);
	}

	// last process	
	int last_w = width-(block_w*(num_procs-2));
	Mat im(Size(last_w,height),type);
		
	for(int i=0;i<height;i++)
		for(int j=0;j<last_w;j++){
			im.at<uchar>(i,j) = image.at<uchar>(i,j+(num_procs-2)*block_w);
                    	
		}
	MPI_Send(&height,1, MPI_INT, num_procs-1, 0, MPI_COMM_WORLD);
	MPI_Send(&last_w,1, MPI_INT, num_procs-1, 0, MPI_COMM_WORLD);
	MPI_Send(&type,1, MPI_INT, num_procs-1, 0, MPI_COMM_WORLD);
	MPI_Send(im.data,last_w*height*1, MPI_BYTE, num_procs-1, 0, MPI_COMM_WORLD);


	Mat image2(Size(height,width),type);

	for (int other_rank = 1; other_rank < num_procs-1; other_rank++)
		MPI_Recv(image2.data+block_w*(other_rank-1)*height*1,block_w*height*1, MPI_BYTE, other_rank,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	// last process	
	MPI_Recv(image2.data+block_w*(num_procs-2)*height*1,last_w*height*1, MPI_BYTE, num_procs-1,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	imwrite("gray_rotation.jpg",image2);

    }
    else{

	int height=0,width=0,type=0;
        MPI_Recv(&height, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&width, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Recv(&type, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        Mat refer(Size(width,height),type);
        MPI_Recv(refer.data, height*width*1, MPI_BYTE, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	
	Mat image(Size(height,width),type);

	for(int i=0; i < height;i++){
                    for(int j=0;j<width;j++){
                            image.at<uchar>(j,height-i-1) = refer.at<uchar>(i,j);
                            
                    }
        }

	MPI_Send(image.data,height*width*1, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}

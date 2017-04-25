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

    if(my_rank == 0 ){

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

	for (int other_rank = 1; other_rank < num_procs; other_rank++)
        {
		MPI_Send(&height,1, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
		MPI_Send(&width,1, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
		MPI_Send(&type,1, MPI_INT, other_rank, 0, MPI_COMM_WORLD);
		MPI_Send(image.data,height*width*1, MPI_BYTE, other_rank, 0, MPI_COMM_WORLD);
	}


	Mat image2(Size(width,height),type);

        for(int i=0; i < height;i++){
		    for(int j=0;j<width;j++){
			    image2.at<Vec3b>(i,j)[0] = 255 - image.at<Vec3b>(i,j)[0];
		    }
        }

        imwrite("0.jpg",image2);

	for (int other_rank = 1; other_rank < num_procs; other_rank++)
        {
		if(other_rank==1) swap(height,width);

		Mat tmp(Size(width,height),type);
		MPI_Recv(tmp.data, height*width*1, MPI_BYTE, other_rank,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		char filename[256];
		sprintf(filename,"%d.jpg",other_rank);
		imwrite(filename,tmp);

		if(other_rank==1) swap(height,width);
	}

    }
    else if(my_rank == 1){

	int height=0,width=0,type=0;
	MPI_Recv(&height, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Recv(&width, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Recv(&type, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        Mat image(Size(width,height),type);
	MPI_Recv(image.data, height*width*1, MPI_BYTE, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	Mat image3(Size(height,width),type);

        for(int i=0; i < height;i++){
                    for(int j=0;j<width;j++){
                            image3.at<uchar>(j,height-i-1) = image.at<uchar>(i,j);
                    }
        }

        MPI_Send(image3.data,height*width*1, MPI_BYTE, 0, 0, MPI_COMM_WORLD);

    }
    else{


	int height=0,width=0,type=0;
        MPI_Recv(&height, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&width, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Recv(&type, 1, MPI_INT, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        Mat image(Size(width,height),type);
        MPI_Recv(image.data, height*width*1, MPI_BYTE, 0,0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        Mat image4 = image.clone();

	int weigth[3][3]={1,2,1,2,4,2,1,2,1};
	for(int i=0; i < height;i++){
                    for(int j=0;j<width;j++){
			    int val = 0;
                            for(int a=0;a<3;a++){
			    	for(int b=0;b<3;b++){
					int x=i-1+a,y=j-1+b;
					if(x >= height) x = height-1;
					if(y >= width) y = width-1;
					if(x<0) x=0;
					if(y<0) y=0;
					val+=image.at<uchar>(x,y)*weigth[a][b];
				}
			    }
		            image4.at<uchar>(i,j) = val/16;
                    }
        }

	MPI_Send(image4.data,height*width*1, MPI_BYTE, 0, 0, MPI_COMM_WORLD);

    }

    MPI_Finalize();
    return 0;
}

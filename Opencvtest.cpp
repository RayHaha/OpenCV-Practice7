// Opencvtest.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include<fstream>
#include<string.h>
#include <time.h>
#include <math.h>

using namespace cv;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	Mat image = imread("lena.bmp");
	Mat grayimage;
	Mat smallimage = Mat(64,64,CV_8UC1);
	int pixel = 0;
	int yo[64][64];
	int marked[64][64];
	int origin[64][64];
	int afterdelete[64][64];
	int yoplus[66][66];// +框框
	cvtColor(image,grayimage, CV_BGR2GRAY);
	//initial
	for(int i=0; i<smallimage.rows; i++){
		for(int j=0; j<smallimage.cols; j++){
			smallimage.at<uchar>(i,j) = grayimage.at<uchar>(i*8,j*8);
		}
	}
	for(int i=0; i<64; i++){
		for(int j=0; j<64; j++){
			yo[i][j] = 0;
			afterdelete[i][j] = 0;
		}
	}
	for(int i=0; i<66; i++){
		for(int j=0; j<66; j++){
			yoplus[i][j] = 0;
		}
	}
	//binary
	for(int i=0; i<smallimage.rows; i++){
		for(int j=0; j<smallimage.cols; j++){
			pixel = smallimage.at<uchar>(i,j);
			if(pixel >= 128){
				smallimage.at<uchar>(i,j) = 255;
			}else{
				smallimage.at<uchar>(i,j) = 0;
			}
		}
	}
	int again = 1;

	for(int i=1; i<65; i++){
		for(int j=1; j<65; j++){
			pixel = smallimage.at<uchar>(i-1,j-1);
			if(pixel == 255){
				yoplus[i][j] = 1;
				afterdelete[i-1][j-1] = 1;
			}
		}
	}

	while(again==1){

		again = 0;
		for(int i=0; i<64; i++){
			for(int j=0; j<64; j++){
				origin[i][j] = afterdelete[i][j];
			}
		}


		for(int i=1; i<65; i++){
			for(int j=1; j<65; j++){

				int h[4] = {0,0,0,0};  // 1:q 2:r 3:s
				//a1
				if(yoplus[i][j]==yoplus[i][j+1]){
					if(yoplus[i][j]!=yoplus[i-1][j] || yoplus[i][j]!=yoplus[i-1][j+1]){
						h[0] = 1;
					}else{
						h[0] = 2;
					}
				}else{
					h[0] = 3;
				}
				//a2
				if(yoplus[i][j]==yoplus[i-1][j]){
					if(yoplus[i][j]!=yoplus[i][j-1] || yoplus[i][j]!=yoplus[i-1][j-1]){
						h[1] = 1;
					}else{
						h[1] = 2;
					}
				}else{
					h[1] = 3;
				}
				//a3
				if(yoplus[i][j]==yoplus[i][j-1]){
					if(yoplus[i][j]!=yoplus[i+1][j] || yoplus[i][j]!=yoplus[i+1][j-1]){
						h[2] = 1;
					}else{
						h[2] = 2;
					}
				}else{
					h[2] = 3;
				}
				//a4
				if(yoplus[i][j]==yoplus[i+1][j]){
					if(yoplus[i][j]!=yoplus[i][j+1] || yoplus[i][j]!=yoplus[i+1][j+1]){
						h[3] = 1;
					}else{
						h[3] = 2;
					}
				}else{
					h[3] = 3;
				}

				if(yoplus[i][j]==0){
					yo[i-1][j-1] = -1;
				}else{
					if(h[0]==2 && h[1]==2 && h[2]==2 && h[3]==2){
						yo[i-1][j-1] = 5;
					}else{
						int count = 0;
						for(int k=0; k<4; k++){
							if(h[k]==1){
								count++;
							}
						}
						yo[i-1][j-1] = count;
					}
				}

			}
		}
		//pair relationship operator
		for(int i=0; i<64; i++){
			for(int j=0; j<64; j++){
				int x0 = yo[i][j];
				int x1 = yo[i][j+1];
				int x2 = yo[i-1][j];
				int x3 = yo[i][j-1];
				int x4 = yo[i+1][j];
				int neighbor=0; // whether neighbor equal to 1 
				if(x1==5 || x2==5 || x3==5 || x4==5){
					neighbor = 1;
				}
				if(x0==-1){
					marked[i][j] = 0;
				}else if(x0==1 && neighbor==1){
					marked[i][j] = 1;  // marked==1 -> deletable
				}else{
					marked[i][j] = 2;  // marked==2 -> nondeletable
				}
			}
		}

		// shrink
		for(int i=0; i<64; i++){
			for(int j=0; j<64; j++){
				if(afterdelete[i][j]==1){
					if(yo[i][j]==1 && marked[i][j]==1){
						afterdelete[i][j] = 0;
						yo[i][j] = -1;
						yoplus[i+1][j+1] = 0;
						// calculate yokoinumber
						int a = i;
						int b = j;
						int c = i+3;
						int d = j+3;
						if(i==0){
							a = i + 1;
						}
						if(j==0){
							b = j + 1;
						}
						if(i==63){
							c = i + 2;
						}
						if(j==63){
							d = j + 2;
						}
						for(int k=a; k<c; k++){     // index of yoplus
							for(int l=b; l<d; l++){
								int h[4] = {0,0,0,0};  // 1:q 2:r 3:s
								//a1
								if(yoplus[k][l]==yoplus[k][l+1]){
									if(yoplus[k][l]!=yoplus[k-1][l] || yoplus[k][l]!=yoplus[k-1][l+1]){
										h[0] = 1;
									}else{
										h[0] = 2;
									}
								}else{
									h[0] = 3;
								}
								//a2
								if(yoplus[k][l]==yoplus[k-1][l]){
									if(yoplus[k][l]!=yoplus[k][l-1] || yoplus[k][l]!=yoplus[k-1][l-1]){
										h[1] = 1;
									}else{
										h[1] = 2;
									}
								}else{
									h[1] = 3;
								}
								//a3
								if(yoplus[k][l]==yoplus[k][l-1]){
									if(yoplus[k][l]!=yoplus[k+1][l] || yoplus[k][l]!=yoplus[k+1][l-1]){
										h[2] = 1;
									}else{
										h[2] = 2;
									}
								}else{
									h[2] = 3;
								}
								//a4
								if(yoplus[k][l]==yoplus[k+1][l]){
									if(yoplus[k][l]!=yoplus[k][l+1] || yoplus[k][l]!=yoplus[k+1][l+1]){
										h[3] = 1;
									}else{
										h[3] = 2;
									}
								}else{
									h[3] = 3;
								}

								if(yoplus[k][l]==0){
									yo[k-1][l-1] = -1;
								}else{
									if(h[0]==2 && h[1]==2 && h[2]==2 && h[3]==2){
										yo[k-1][l-1] = 5;
									}else{
										int count = 0;
										for(int m=0; m<4; m++){
											if(h[m]==1){
												count++;
											}
										}
										yo[k-1][l-1] = count;
									}
								}
							}
						}// end for k (calculate yokoinumber)

					}
				} // end if

			}
		}

		// check change or not
		for(int i=0; i<64; i++){
			for(int j=0; j<64; j++){
				if(origin[i][j]!=afterdelete[i][j]){
					again = 1;
				}
			}
		}
		//cout<<"aa"<<endl; 
	}

	for(int i=0; i<64; i++){
		for(int j=0; j<64; j++){
			if(afterdelete[i][j]==1){
				smallimage.at<uchar>(i,j) = 255;
			}else{
				smallimage.at<uchar>(i,j) = 0;
			}
		}
	}
	
	imshow("image",image);
	imshow("smallimage",smallimage);
	imwrite("thinning.bmp",smallimage);

	system("pause");
	waitKey(0);
	return 0;
}
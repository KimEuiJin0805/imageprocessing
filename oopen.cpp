#include <opencv\cv.h>
#include<opencv\highgui.h>
#include<opencv\cxcore.h>

#include<stdio.h>

#define unsigned char uchar


uchar** uc_alloc(int size_x, int size_y)
{

	uchar** m;
	int i;

	if ((m = (uchar * *)calloc(size_y, sizeof(uchar*))) == NULL)
	{
		printf("uc_alloc error 1\7\n");
		exit(0);
	}

	for (i = 0; i < size_y; i++)
		if ((m[i] = (uchar*)calloc(size_x, sizeof(uchar))) == NULL) {
			printf("uc_alloc error 2\7\n");
			exit(0);

		}
	return m;
}

void read_ucmatrix(int size_x, int size_y, uchar** ucmatrix, char* filename)
{
	int i;
	FILE* f;
	if ((fopen_s(&f, filename, "rb")) != NULL)
	{
		printf("%s File open Error! \n", filename);
		exit(0);

	}
	for (i = 0; i < size_y; i++)
		if (fread(ucmatrix[i], sizeof(uchar), size_x, f) != size_x)
		{
			printf("Data Read Error! \n");
			exit(0);

		}
	fclose(f);
}

void write_ucmatrix(int size_x, int size_y, uchar** ucmatrix, char* filename)
{
	int i;
	FILE* f;

	if ((fopen_s(&f, filename, "wb")) != NULL)
	{
		printf("%s File open Error! \n", filename);
		exit(0);
	}

	for (i = 0; i < size_y; i++)
		if (fwrite(ucmatrix[i], sizeof(uchar), size_x, f) != size_x)
		{
			printf("Data Write Error! \n");
			exit(0);
		}
	fclose(f);
}

/*void Negative(uchar** img, uchar** Result, int Row, int Col)
{
	int i, j;
	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
			Result[i][j] = 255 - img[i][j];
}*/

/*void mosaic(uchar** img, uchar** out, int Row, int Col, int Block)
{
	int i, j, x, y, tmp, c;
	for (i = 0; i < Row; i += Block)
	{
		for (j = 0; j < Col; j += Block)
		{
			tmp = 0;
			c = 0;
			for (y = 0; y < Block; y++)
			{
				for (x = 0; x < Block; x++)
				{
					tmp += img[i + y][j + x];
					c++;
				}
			}
			tmp /= c;
			for (y = 0; y < Block; y++)
			{
				for (x = 0; x < Block; x++)
				{
					out[i + y][j + x] = tmp;
				}
			}
		}
	}
}*/

double average(uchar** img, int Row, int Col)
{
	double sum = 0, avg;
	int i, j;

	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			sum += img[i][j];
		}
	}
	avg = sum / ((double)Row * Col);
	printf("Average of Image %lf \n", avg);
	return avg;
}

double chance(uchar** img, int Row, int Col)
{
	int i, j, t;
	double tmp[256] = { 0 };
	double avg =0;

	for (t = 0; t < 256; t++) {
		for (i = 0; i < Row; i++) {
			for (j = 0; j < Col; j++) {
				if (img[i][j] == t) tmp[t] += 1;
			}
		}
		avg += t * (tmp[t] / (double)(Row * Col));
	}
	
	printf("Average of Image %lf \n", avg);
	return avg;
}

double powImg(uchar** img, uchar** Result, int Row, int Col, double gamma)
{
	int i, j;
	double tmp;
	int sum = 0;
	double avg;

	for(i=0;i<Row;i++)
		for (j = 0; j < Col; j++) {
			tmp = pow(img[i][j] / 255., 1 / gamma);

			if (tmp * 255 > 255) tmp = 1;
			else if (tmp * 255 < 0) tmp = 0;
			Result[i][j] = tmp * 255;
			sum += Result[i][j];  //gamma를 적용한 각각 픽셀의 값을 변수 sum에 다 더해준다
		}

	avg = sum / ((double)Row * Col);  //다 더한 값을 가로 세로의 픽셀 곱으로 나누어 평균을 구해준다
	printf("Average of Image %lf \n", avg);

	return avg;  //평균을 반환해 준다
}

void makeBinary(uchar** img, uchar** out, int Row, int Col, double avg)
{
	int i, j;

	for (i = 0; i < Row; i++) {
		for (j = 0; j < Col; j++) {
			if (img[i][j] > avg) out[i][j] = 255;
			else out[i][j] = 0;
		}
	}
}

void BitSlicing(uchar** img, uchar** Result, int Row, int Col, int position)
{
	int i, j;
	uchar mask = 0x01;
	mask <<= position;

	for(i=0; i<Row; i++)
		for (j = 0; j < Col; j++)
		{
			if ((mask & img[i][j]) == pow(2, position))
			{
				Result[i][j] = 255;
			}
			else
			{
				Result[i][j] = 0;
			}
		}
}

void BitImage(uchar** img, uchar** Result, int Row, int Col, int sum)
{
	int i, j, k;
	uchar mask = 0x80;
	
	for (k = 1; k < sum; k++)
	{
		mask = mask + (0x80 >> k);
	}

	for (i = 0; i < Row; i++)
		for (j = 0; j < Col; j++)
		{
			Result[i][j] = img[i][j] & mask;
		}
}

void addImage(uchar** img, uchar** Result, int Row, int Col, int con)
{
	int i, j;

	for(i=0; i<Row; i++)
		for (j = 0; j < Col; j++)
		{
			Result[i][j] = img[i][j] + con;
		}
}


int main(int argc, char* argv[])
{

	double avg = 0;
	double gamma = 0.5;
	IplImage* cvImg;
	CvSize imgSize;
	uchar** img;
	uchar** rimg;

	if (argc != 5)
	{
		printf("Exe imgData x_size y_size resultimg \n");
		exit(0);
	}
	imgSize.width = atoi(argv[2]);
	imgSize.height = atoi(argv[3]);
	img = uc_alloc(imgSize.width, imgSize.height);
	rimg = uc_alloc(imgSize.width, imgSize.height);  //결과 이미지를 저장할 곳을 할당해줌
	read_ucmatrix(imgSize.width, imgSize.height, img, argv[1]);
	//BitImage(img, rimg, imgSize.width, imgSize.height, 3);
	addImage(img, rimg, imgSize.width, imgSize.height, 3);
	write_ucmatrix(imgSize.width, imgSize.height, rimg, argv[4]);  //결과값 저장
	average(rimg, imgSize.width, imgSize.height);

	/*cvImg = cvCreateImage(imgSize, 8, 1);
	for (i = 0; i < imgSize.height; i++)
		for (j = 0; j < imgSize.width; j++)
		{

			((uchar*)(cvImg->imageData + cvImg->widthStep * i))[j] = img[i][j];

		}
		cvNamedWindow(argv[1], 1);
		cvShowImage(argv[1], cvImg);


	cvWaitKey(0);

	cvDestroyWindow("image");
	cvReleaseImage(&cvImg);

	getchar();
	getchar();*/

	return 0;

}
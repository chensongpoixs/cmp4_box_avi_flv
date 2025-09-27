#define _CRT_SECURE_NO_WARNINGS  

#include <iostream>  
using namespace std;
#include <windows.h>  
#include <stdlib.h>  
#include <math.h>  
#include <iomanip>   
#include <fstream>  

//以下模块是完成BMP图像(彩色图像是24bit RGB各8bit)的像素获取，并存在文件名为xiang_su_zhi.txt中  

unsigned char *pBmpBuf;//读入图像数据的指针  

int bmpWidth;//图像的宽  
int bmpHeight;//图像的高  
RGBQUAD *pColorTable;//颜色表指针  

int biBitCount;//图像类型，每像素位数  

			   //读图像的位图数据、宽、高、颜色表及每像素位数等数据进内存，存放在相应的全局变量中  
bool readBmp(char *bmpName)
{
	FILE *fp = fopen(bmpName, "rb");//二进制读方式打开指定的图像文件  

	if (fp == 0)
		return 0;

	/*
	typedef struct tagBITMAPFILEHEADER {
        WORD    bfType; 2
        DWORD   bfSize; 4
        WORD    bfReserved1; 2
        WORD    bfReserved2; 2
        DWORD   bfOffBits; 4
} BITMAPFILEHEADER,  14个字节
	*/
	//跳过位图文件头结构BITMAPFILEHEADER  
	fseek(fp, sizeof(BITMAPFILEHEADER), 0);


	//定义位图信息头结构变量，读取位图信息头进内存，存放在变量head中  
	BITMAPINFOHEADER head;
	/*
	typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;//typedef unsigned long       DWORD;//4
        LONG       biWidth;//typedef long LONG; //4
        LONG       biHeight;
        WORD       biPlanes;//typedef unsigned short      WORD; 2
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER,
	*/
	/*头部信息的大小: 40个字节
	*/
	cout << "头部信息的大小:" << sizeof(BITMAPINFOHEADER) << endl;

	fread(&head, sizeof(BITMAPINFOHEADER), 1, fp); //获取图像宽、高、每像素所占位数等信息  



	bmpWidth = head.biWidth;
	cout << "LONG:" << sizeof(head.biWidth) << endl;
	cout << "typedef unsigned long :" << sizeof(DWORD) << endl;
	printf("二进制:%p\n", head.biWidth);
	bmpHeight = head.biHeight;
	printf("二进制:%p\n", head.biPlanes);
	biBitCount = head.biBitCount;//定义变量，计算图像每行像素所占的字节数（必须是4的倍数）  
	//24个字节    采用色彩
	cout << "bitBitCount :" << "[ " << sizeof(head.biBitCount) << "] "<< head.biBitCount << endl;
	printf("二进制:%p\n", head.biBitCount);
	int lineByte = (bmpWidth * biBitCount / 8 + 3) / 4 * 4;//灰度图像有颜色表，且颜色表表项为256  

	if (biBitCount == 8)
	{
		/*
		typedef struct tagRGBQUAD {
        BYTE    rgbBlue;typedef unsigned char       BYTE;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;
		*/
		//申请颜色表所需要的空间，读颜色表进内存  
		pColorTable = new RGBQUAD[256];
		cout << "颜色表字节数:" << sizeof(RGBQUAD) << endl;
		fread(pColorTable, sizeof(RGBQUAD), 256, fp);
	}
	//颜色表4个字节
	cout << "颜色表字节数:" << sizeof(RGBQUAD) << endl;
	//申请位图数据所需要的空间，读位图数据进内存  
	pBmpBuf = new unsigned char[lineByte * bmpHeight];
	fread(pBmpBuf, 1, lineByte * bmpHeight, fp);

	fclose(fp);//关闭文件  

	return 1;//读取文件成功  
}


//给定一个图像位图数据、宽、高、颜色表指针及每像素所占的位数等信息,将其写到指定文件中  
bool saveBmp(char *bmpName, unsigned char *imgBuf, int width, int height, int biBitCount, RGBQUAD *pColorTable)
{
	//如果位图数据指针为0，则没有数据传入，函数返回  
	if (!imgBuf)
		return 0;
	//GetDeviceCaps();
	//颜色表大小，以字节为单位，灰度图像颜色表为1024字节，彩色图像颜色表大小为0  
	int colorTablesize = 0;

	if (biBitCount == 8)
		colorTablesize = 1024;

	//待存储图像数据每行字节数为4的倍数  
	int lineByte = (width * biBitCount / 8 + 3) / 4 * 4;

	//以二进制写的方式打开文件  
	FILE *fp = fopen(bmpName, "wb");

	if (fp == 0)
		return 0;

	//申请位图文件头结构变量，填写文件头信息  
	BITMAPFILEHEADER fileHead;

	fileHead.bfType = 0x4D42;//bmp类型  

							 //bfSize是图像文件4个组成部分之和  
	fileHead.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTablesize + lineByte*height;

	fileHead.bfReserved1 = 0;
	fileHead.bfReserved2 = 0;

	//bfOffBits是图像文件前3个部分所需空间之和  
	fileHead.bfOffBits = 54 + colorTablesize;

	//写文件头进文件  
	fwrite(&fileHead, sizeof(BITMAPFILEHEADER), 1, fp);
	//申请位图信息头结构变量，填写信息头信息  
	BITMAPINFOHEADER head;

	head.biBitCount = biBitCount;
	head.biClrImportant = 0;
	head.biClrUsed = 0;
	head.biCompression = 0;
	head.biHeight = height;
	head.biPlanes = 1;
	head.biSize = 40;
	head.biSizeImage = lineByte*height;
	head.biWidth = width;
	head.biXPelsPerMeter = 0;
	head.biYPelsPerMeter = 0;

	//写位图信息头进内存  
	fwrite(&head, sizeof(BITMAPINFOHEADER), 1, fp);

	//如果灰度图像，有颜色表，写入文件   
	if (biBitCount == 8)
		fwrite(pColorTable, sizeof(RGBQUAD), 256, fp);

	//写位图数据进文件  
	fwrite(imgBuf, height*lineByte, 1, fp);

	//关闭文件  
	fclose(fp);

	return 1;

}


//以下为像素的读取函数  
void doIt()
{

	//读入指定BMP文件进内存，注意要将"nv.BMP"文件和“源.cpp"代码放在同一个文件夹下  
	char readPath[] = "palette.bmp";
	readBmp(readPath);

	//输出图像的信息  
	cout << "width=" << bmpWidth << " height=" << bmpHeight << " biBitCount=" << biBitCount << endl;

	//循环变量，图像的坐标  
	//每行字节数  

	int lineByte = (bmpWidth*biBitCount / 8 + 3) / 4 * 4;

	//循环变量，针对彩色图像，遍历每像素的三个分量  
	int m = 0, n = 0, count_xiang_su = 0;

	//将图像左下角1/4部分置成黑色  
	ofstream outfile("图像像素.txt", ios::in | ios::trunc);

	if (biBitCount == 8) //对于灰度图像  
	{
		//以下完成图像的分割成8*8小单元，并把像素值存储到指定文本中。由于BMP图像的像素数据是从  
		//左下角：由左往右，由上往下逐行扫描的  
		int L1 = 0;
		int hang = 63;
		int lie = 0;
		//int L2=0;  
		//int fen_ge=8;  
		for (int fen_ge_hang = 0; fen_ge_hang<8; fen_ge_hang++)//64*64矩阵行循环  
		{
			for (int fen_ge_lie = 0; fen_ge_lie<8; fen_ge_lie++)//64*64列矩阵循环  
			{
				for (L1 = hang; L1>hang - 8; L1--)//8*8矩阵行  
				{
					for (int L2 = lie; L2<lie + 8; L2++)//8*8矩阵列  
					{
						m = *(pBmpBuf + L1*lineByte + L2);
						outfile << m << " ";
						count_xiang_su++;
						if (count_xiang_su % 8 == 0)//每8*8矩阵读入文本文件  
						{
							outfile << endl;
						}
					}
				}

				hang = 63 - fen_ge_hang * 8;//64*64矩阵行变换  
				lie += 8;//64*64矩阵列变换,该一行（64）由8个8*8矩阵的行组成  
			}
			hang -= 8;//64*64矩阵的列变换  
			lie = 0;//64*64juzhen  
		}
	}

	//double xiang_su[2048];  
	//ofstream outfile("xiang_su_zhi.txt",ios::in|ios::trunc);  
	if (!outfile)
	{
		cout << "open error!" << endl;
		exit(1);
	}
	else if (biBitCount == 24)
	{   //彩色图像  
		for (int i = 0; i<bmpHeight; i++)
		{
			for (int j = 0; j<bmpWidth; j++)
			{
				for (int k = 0; k<3; k++)//每像素RGB三个分量分别置0才变成黑色  
				{
					//*(pBmpBuf+i*lineByte+j*3+k)-=40;  
					m = *(pBmpBuf + i*lineByte + j * 3 + k);
					outfile << m << " ";
					count_xiang_su++;
					if (count_xiang_su % 8 == 0)
					{
						outfile << endl;
					}

				}
				n++;
			}

		}
		cout << "总的像素个素为(即大小):" << n << endl;

	}

	//将图像数据存盘  
	char writePath[] = "nvcpy.BMP";//图片处理后再存储  
	saveBmp(writePath, pBmpBuf, bmpWidth, bmpHeight, biBitCount, pColorTable);

	//清除缓冲区，pBmpBuf和pColorTable是全局变量，在文件读入时申请的空间  
	delete[]pBmpBuf;

	if (biBitCount == 8)
		delete[]pColorTable;
}

void main()
{
	doIt();
	system("pause");
}






/*
	功能：在图片的第50行画一条黑线
	为简化代码，只支持24位色的图片
	codeblocks下正确运行。VC下需要将二维数组img改为malloc动态分配。
	*/
#include <stdio.h>
#include <stdlib.h>
	typedef struct {
		BYTE b;
		BYTE g;
		BYTE r;
}RGB;

int ffmain(void)
{
BITMAPFILEHEADER fileHeader;
BITMAPINFOHEADER infoHeader;
FILE* pfin = fopen("原始图像.bmp", "rb");
FILE* pfout = fopen("修改后的图像.bmp", "wb");
//Read the Bitmap file header;
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, pfin);
//Read the Bitmap info header;
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, pfin);
//为简化代码，只处理24位彩色
	if (infoHeader.biBitCount == 24)
	{
int size = infoHeader.biWidth*infoHeader.biHeight;
RGB img[infoHeader.biHeight][infoHeader.biWidth];
fread(img, sizeof(RGB), size, pfin);
//把第50行染成黑色
	int i = 0;
for (; i < infoHeader.biWidth; i++)
	{
	img[50][i].b = img[50][i].g = img[50][i].r = 0;
	}
//将修改后的图片保存到文件
	fwrite(&fileHeader, sizeof(fileHeader), 1, pfout);
fwrite(&infoHeader, sizeof(infoHeader), 1, pfout);
fwrite(img, sizeof(RGB), size, pfout);
}
	}
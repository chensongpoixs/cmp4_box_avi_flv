/*
**  converts *.BMP File to NV12 pixelformat Frame and vice versa
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>  // POSIX
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

 
typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;
typedef signed long		LONG;
typedef int 			BOOL;

#pragma pack(push, 2) // alignment to WORD

typedef struct  
{
	WORD    bfType;					// The file type; must be BM.
	DWORD   bfSize;					// The size, in bytes, of the bitmap file.
	WORD    bfReserved1;
	WORD    bfReserved2;
	DWORD   bfOffBits;				// The offset, in bytes, to the bitmap bits.
									//  for us = sizeof(BITMAPFILEHEADER) + 
									//  sizeof(BITMAPINFOHEADER)
} BITMAPFILEHEADER;

typedef struct 
{
	DWORD      biSize;				// sizeof(BITMAPINFOHEADER)
	LONG       biWidth;				// The width of the bitmap, in pixels.
	LONG       biHeight;			// The height of the bitmap, in pixels.
									//  If biHeight is positive, the bitmap is a 
									//  bottom-up DIB. If biHeight is negative, 
									//  the bitmap is a top-down DIB.
	WORD       biPlanes;			// This value must be set to 1.
	WORD       biBitCount;          // Bits-per-pixel (for us = 24).
	DWORD      biCompression;		// The type of compression (for us = BI_RGB,
									//  an uncompressed format.)
	DWORD      biSizeImage;			// The size, in bytes, of the image. This may 
									//  be set to zero for BI_RGB bitmaps.
	LONG       biXPelsPerMeter;		// must be 0;
	LONG       biYPelsPerMeter;     // must be 0;
	DWORD      biClrUsed;			// must be 0;
	DWORD      biClrImportant;		// must be 0;
} BITMAPINFOHEADER;

#pragma pack(pop)


#define BI_RGB 		0x0000

#ifndef MAX_PATH
#define MAX_PATH	256
#endif

#define DEBUG		0

#define HELP "\nrgb2yuv - BMP File (24 bits-per-pixel) to YUV raw Frame \
\n          (nv12 pixelformat) and vice versa Converter.\n\
Uses:\n  rgb2yuv image.bmp\nor\n  rgb2yuv WxH frame.yuv\n\
where:\n  W  - frame width;\n  H  - frame height.\n" 


// converts bitmap file to yuv (nv12) frame file
int bmp_to_nv12(char *fname);
// converts yuv (nv12) frame to bitmap file
int nv12_to_bmp(char *fname, int width, int height);

// reads RGB data from BMP file, returns not NULL if success
BYTE* get_rgb(char *fname, LONG *width, LONG *height);
// converts 24 bits-per-pixel bitmap RGB data to NV12 pixelformat frame
BYTE* set_nv12(BYTE *rgb,  LONG width, LONG height, BOOL is_bottom_up);
// reads NV12 frame from specified file, returns not NULL if success
BYTE* get_nv12(char *fname, LONG width, LONG height);
// converts NV12 pixelformat frame to 24 bits-per-pixel bitmap RGB data 
BYTE* set_rgb(BYTE *nv12, LONG width, LONG height, DWORD *rgb_bytes);


////////////////////////////////////////////////////////////////////////////////////

// returns exit code = 0 if success
int main(int argc, char** argv)
{
    int	unWidth = 729, unHeight = 484;
	//int	unWidth , unHeight;

#if 1



	char fout[MAX_PATH];
	char filename[] = "image/songli.bmp";
	fout[0] = 0x00;
	// prepare the NV12 file name
	strcpy(fout, filename);
	char *dot;
	if ((dot = strrchr(fout, '.')) != NULL) *dot = 0x00;
	strcat(fout, ".yuv");
	fprintf(stderr, "Saving NV12 raw frame to file: '%s'\n", fout);
	int i;
	char buf[90] = { 0 };
	FILE *fo;
	// save NV12 frame to a file
	if (!(fo = fopen(fout, "wb")))
	{
		fprintf(stderr, "\nError: Unable to open file %s\n", fout);
		return 1;
	}

	for (i = 0; i < 11; i++) {
		memset(buf, 0x00, sizeof(char) * 90);

		sprintf(buf, "image/%d.bmp", i);
		printf("buf = %s\n", buf);
		bmp_to_nv12(buf, &fout, &fo);
	}

	if (fo != NULL)
		fclose(fo);

#endif // 0

	//nv12_to_bmp("729_484.yuv", unWidth, unHeight);
	system("pause");
	return NULL;
	// check for user specified parameters
	/*if (argc < 2 || argc > 3)  
	{
		fprintf(stderr, "%s", HELP);
		return 1;
	}

	if (argc == 3) 	// YUV to BMP mode
	{
		char *x;
		if ((x = strchr(argv[1], 'x')) == NULL)
		{
			fprintf(stderr, "%s", HELP);
			return 1;
		}

		unWidth = atoi(argv[1]);
		unHeight = atoi(x + 1);
		if (unWidth == 0 || unHeight == 0)
		{
			fprintf(stderr, "%s", HELP);
			return 1;
		}

		return nv12_to_bmp(argv[2], unWidth, unHeight);
	}
	else 			// BMP to YUV mode
	{
		return bmp_to_nv12(argv[1]);
	}*/
}

////////////////////////////////////////////////////////////////////////////////////

// converts bitmap file to yuv (nv12) frame file
int bmp_to_nv12(char *fname, char **out, FILE** in)
{
    LONG  unWidth, unHeight;
    BYTE  *pRGBData, *pNV12Frame;
	DWORD unNV12Size;
	char *fout = out;
	FILE *fo = *in;
	

	/*if (access(fname, 0) == -1)
	{
		fprintf(stderr, "\nError: File %s not exists!\n", fname);
		return 1;
	}*/

	// get RGB data from 24 bits-per-pixel bitmap file
	pRGBData = get_rgb(fname, &unWidth, &unHeight);
	//GetDeviceCaps();
	if (pRGBData == NULL || unWidth == 0 || unHeight == 0) 
	{
		fprintf(stderr, "\nError: Incorrect Bitmap Data!\n");
		return 1;
	}

	// convert RGB data to NV12 frame with size = unWidth * unHeight * 1.5
	pNV12Frame = set_nv12(pRGBData, unWidth, abs(unHeight), (unHeight > 0));
	unNV12Size = (DWORD)(unWidth * abs(unHeight) * 1.5);

	free(pRGBData);

	if ((pNV12Frame == NULL) || (unNV12Size == 0))
	{
		fprintf(stderr, "\nError: Wrong RGB to NV12 convertion!\n");
		return 1;
	}

	
	if (DEBUG) 
	{	// print NV12 frame data to stdout
		int i;	
		for (i = 0; i < unNV12Size; ++i) 
		{
			if (pNV12Frame[i] < 0x10) printf("0");
			printf("%X ", (int)pNV12Frame[i]);
		}
		printf("\n");
	}

	
	//fseek(fo, sizeof(pNV12Frame), 0);

	//文件描述符到文件末尾
	int chen = fseek(fo, 0, SEEK_END);  //定位的文件的末尾
	printf("chen = %d\n", chen);
	long len = ftell(fo);  //获取到当前文件的位置   查看文件的当前位置的信息
	printf("len = %d\n", len);

	//fwrite(buf, sizeof(char *), length, file);

    fwrite(pNV12Frame, sizeof(BYTE), unNV12Size, fo);

	//刷新存储
	fflush(fo);
	
	if (pNV12Frame == NULL) 
		free(pNV12Frame);
	return 0;
}

// converts yuv (nv12) frame to bitmap file
int nv12_to_bmp(char *fname, int width, int height)
{
    BYTE  				*pRGBData, *pNV12Frame;
	BITMAPFILEHEADER 	kFileHeader;
	BITMAPINFOHEADER 	kInfoHeader;
	DWORD				unRGBDataBytes;

	FILE *fo; // file to save bitmap data 
	char fout[MAX_PATH];
	fout[0] = 0x00;

	/*if (fopen(fname, 0) == -1)
	{
		fprintf(stderr, "\nError: File %s not exists!\n", fname);
		return 1;
	}*/

	// get RGB data from 24 bits-per-pixel bitmap file
	pNV12Frame = get_nv12(fname, width, height);

	if (pNV12Frame == NULL) 
	{
		fprintf(stderr, "\nError: Incorrect NV12 Frame Data!\n");
		return 1;
	}

	// convert NV12 frame to RGB data 
	pRGBData = set_rgb(pNV12Frame, width, height, &unRGBDataBytes);
	free(pNV12Frame);

	if ((pRGBData == NULL) || (unRGBDataBytes == 0))
	{
		fprintf(stderr, "\nError: Wrong NV12 to RGB convertion!\n");
		return 1;
	}

	// prepare the new BMP file name
	strcpy(fout, fname);
	char *dot;
	if ((dot = strrchr(fout, '.')) != NULL) *dot = 0x00;
	strcat(fout, ".bmp");
	fprintf(stderr, "Saving RGB data to file: '%s'\n", fout);

	// prepare BMP file header
	kFileHeader.bfType = 0x4D42;  // "BM"
	kFileHeader.bfSize =	sizeof(BITMAPFILEHEADER) + 
		sizeof(BITMAPINFOHEADER) + unRGBDataBytes;
	kFileHeader.bfReserved1 = 0;
	kFileHeader.bfReserved2 = 0;
	kFileHeader.bfOffBits =	sizeof(BITMAPFILEHEADER) + 
		sizeof(BITMAPINFOHEADER);

	// prepare BMP info header
	kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	kInfoHeader.biWidth = width;
	kInfoHeader.biHeight = height;
	kInfoHeader.biPlanes = 1;
	kInfoHeader.biBitCount = 24;
	kInfoHeader.biCompression = BI_RGB;
	kInfoHeader.biSizeImage = 0;
	kInfoHeader.biXPelsPerMeter = 0;
	kInfoHeader.biYPelsPerMeter = 0;
	kInfoHeader.biClrUsed = 0;
	kInfoHeader.biClrImportant = 0;

	// save to BMP file
	if ( !(fo = fopen(fout, "wb")))
	{
	    fprintf(stderr, "\nError: Unable to open file %s\n", fout);
		return 1;
	}


    fwrite(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, fo);
    fwrite(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, fo);
    fwrite(pRGBData, sizeof(BYTE), unRGBDataBytes, fo);
	    
	fflush(fo);
	fclose(fo);
	free(pRGBData);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////

// conversion functions (based on http://www.fourcc.org/fccyvrgb.php)

BYTE RGBtoY (BYTE R, BYTE G, BYTE B) 
{
	return (BYTE)((0.257 * R) + (0.504 * G) + (0.098 * B) + 16);
}

BYTE RGBtoU (BYTE R, BYTE G, BYTE B) 
{
	return (BYTE)(-(0.148 * R) - (0.291 * G) + (0.439 * B) + 128);
}

BYTE RGBtoV (BYTE R, BYTE G, BYTE B) 
{
	return (BYTE)((0.439 * R) - (0.368 * G) - (0.071 * B) + 128);
}

BYTE Clamp (int n) 
{
	n &= -(n >= 0);
	return n | ((255 - n) >> 31);
}

BYTE YUVtoR (BYTE Y, BYTE U, BYTE V) 
{
	int res = (1.164 * (Y - 16) + 1.596 * (V - 128));
    return Clamp(res);
}
BYTE YUVtoG (BYTE Y, BYTE U, BYTE V) 
{
	int res = (1.164 * (Y - 16) - 0.813 * (V - 128) - 0.391 * (U - 128));
    return Clamp(res);
}
BYTE YUVtoB (BYTE Y, BYTE U, BYTE V) 
{
	int res = (1.164 * (Y - 16) + 2.018 * (U - 128));
    return Clamp(res);
}

////////////////////////////////////////////////////////////////////////////////////

// reads RGB data from BMP file, returns not NULL if success
BYTE* get_rgb(char* fname, LONG* width, LONG* height)
{
    FILE				*fi;
	BITMAPFILEHEADER 	kFileHeader;
	BITMAPINFOHEADER 	kInfoHeader;
	DWORD				unWidthAdjusted, unDataBytes;
    BYTE 				*pRGBData;

	if ( !(fi = fopen(fname, "rb")) )
	{
	    fprintf(stderr, "\nError: Unable to open file %s\n", fname);
		return NULL;
	}


	// read bitmap file header
    if (fread(&kFileHeader, sizeof(BITMAPFILEHEADER), 1, fi) != 1)
	{
	    fprintf(stderr, "\nError: Unable to read file header\n");
	    fclose(fi);
		return NULL;
	}
	//   文件格式的判断
	if (kFileHeader.bfType != (WORD)0x4D42)  // "BM"
	{
	    fprintf(stderr, "\nError: Invalid bitmap file type: %X\n", 
			kFileHeader.bfType);
	    fclose(fi);
		return NULL;
	}

	// read bitmap info header
    if (fread(&kInfoHeader, sizeof(BITMAPINFOHEADER), 1, fi) != 1)
	{
	    fprintf(stderr, "\nError: Unable to read info header\n");
	    fclose(fi);
		return NULL;
	}
	//每个像素所需的位数
	if (kInfoHeader.biBitCount != 24)
	{
	    fprintf(stderr, "\nError: Unsupported bitmap pixel format: %d\n", 
			(int)kInfoHeader.biBitCount);
		system("puase");
	    fclose(fi);
		return NULL;
	}
	//位图压缩类型
	if (kInfoHeader.biCompression != BI_RGB)
	{
	    fprintf(stderr, "\nError: Unsupported bitmap compression: %d\n", 
			(int)kInfoHeader.biCompression);
	    fclose(fi);
		return NULL;
	}

	fprintf(stderr, " Widht: %d;\t\t Height: %d;\n", (int)kInfoHeader.biWidth, 
		(int)kInfoHeader.biHeight);
	if (kInfoHeader.biHeight > 0)
			fprintf(stderr, " DIB: Bottom-UP;");
	else 
			fprintf(stderr, " DIB: Top-Down;");
	// 位图的大小
	fprintf(stderr, "\t Image Size: %d;\n", (int)kInfoHeader.biSizeImage);


	// the Width must be adjusted on DWORD (4 Bytes) boundary
	unWidthAdjusted = kInfoHeader.biWidth * 3;  // line RGB data bytes
	if (unWidthAdjusted % 4) 
	{   // if there is reminder from division on 4,
		// then adjust the width with zero bytes
	    unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;
	}
	unDataBytes = unWidthAdjusted * abs(kInfoHeader.biHeight);

	fprintf(stderr, " Raw RGB Data: %d;\t Adjusted RGB Data: %d\n", 
		(int)(kInfoHeader.biWidth * 3 * abs(kInfoHeader.biHeight)), 
		(int)unDataBytes);
	fprintf(stderr, " Bitmap file size: %d\n", (int)(sizeof(BITMAPFILEHEADER) + 
		sizeof(BITMAPINFOHEADER) + unDataBytes));

	// allocate memory for RGB data
	if ( !(pRGBData = (BYTE*)malloc(unDataBytes)) )
	{
    	fprintf(stderr, "Error: Out of memory");
	    fclose(fi);
		return NULL;
	}
	memset(pRGBData, 0xFF, unDataBytes);
	
	// read bitmap RGB data
    if (fread(pRGBData, 1, unDataBytes, fi) != unDataBytes)
	{
	    fprintf(stderr, "\nError: Unable to read RGB data\n");
	    fclose(fi);
		return NULL;
	}

	// everything ok
	fclose(fi);
	*width = kInfoHeader.biWidth;
	*height = kInfoHeader.biHeight;

	return pRGBData;
}

// converts 24 bits-per-pixel bitmap RGB data to NV12 pixelformat frame.
// if is_bottom_up is true - the rgb data is turned upside down.
// returns a pointer to nv12 frame witch size = width * height * 1.5
BYTE* set_nv12(BYTE* rgb,  LONG width, LONG height, BOOL is_bottom_up)
{
	LONG	unDataPixels, unFrameBytes, x, y;
	// rgb data vars
    BYTE 	*pRGBeven, *pRGBodd;
	BYTE 	R, G, B;
	// nv12 data vars
	BYTE	*pYPlane,  *pUVPlane;
	BYTE 	Y, U, V;
	BYTE	U00, U01, U10, U11, V00, V01, V10, V11;
	BYTE 	*pNV12Frame = NULL;

	// check out the rgb data size conditions
	if ((width % 4) || (height % 4))
	{
    	fprintf(stderr, "Error: The Width and Height must be multiple of Four");
		return NULL;
	}

	unDataPixels = width * height;
	// Y plane + UV plane
	unFrameBytes  = (DWORD)(unDataPixels + (unDataPixels / 2));
	fprintf(stderr, " Raw NV12 Frame Size: %d\n", (int)unFrameBytes);

	// allocate memory for NV12 frame data
	if ( !(pNV12Frame = (BYTE*)malloc(unFrameBytes)) )
	{
    	fprintf(stderr, "Error: Out of memory");
		return NULL;
	}
	memset(pNV12Frame, 0x00, unFrameBytes);

	pYPlane = pNV12Frame;
	pUVPlane = pNV12Frame + unDataPixels;

	// the algorithm below converts nv12 frame to implement 
	// the following on-screen line scheme:
	//
	//          Y00   Y01  Y02   Y03  ... ...
	//           U00,V00    U01,V01     ...
	//          Y10   Y11  Y12   Y13  ... ...
	//     
	// in other words, it processes the input rgb data by 
	// pair of its lines (even & odd at time) to get U & V 
	// color values from 4 neighbor pixels.


	for (y = 0; y < height; y += 2) 
	{
		// set current even and odd line pointers
		if (is_bottom_up)
		{
			pRGBeven = rgb + ((height - y - 1) * (width * 3));
			pRGBodd  = rgb + ((height - y - 2) * (width * 3));
		}
		else
		{
			pRGBeven = rgb + ((y + 0) * (width * 3));
			pRGBodd  = rgb + ((y + 1) * (width * 3));
		}

		// process even Y pixels and all U, V pixels
		for (x = 0; x < width; x += 2) 
		{
			B = *pRGBeven; 	++pRGBeven;
			G = *pRGBeven;	++pRGBeven;
			R = *pRGBeven;  ++pRGBeven;

			Y =   RGBtoY(R, G, B);
			U00 = RGBtoU(R, G, B);
			V00 = RGBtoV(R, G, B);

			*pYPlane = Y;  ++pYPlane;

			B = *pRGBeven; 	++pRGBeven;
			G = *pRGBeven;	++pRGBeven;
			R = *pRGBeven;  ++pRGBeven;

			Y =   RGBtoY(R, G, B);
			U01 = RGBtoU(R, G, B);
			V01 = RGBtoV(R, G, B);

			*pYPlane = Y;  ++pYPlane;

			B = *pRGBodd; 	++pRGBodd;
			G = *pRGBodd;	++pRGBodd;
			R = *pRGBodd;	++pRGBodd;

			U10 = RGBtoU(R, G, B);
			V10 = RGBtoV(R, G, B);

			B = *pRGBodd; 	++pRGBodd;
			G = *pRGBodd;	++pRGBodd;
			R = *pRGBodd;	++pRGBodd;

			U11 = RGBtoU(R, G, B);
			V11 = RGBtoV(R, G, B);

			U = (BYTE)((U00 + U01 + U10 + U11) / 4);
			V = (BYTE)((V00 + V01 + V10 + V11) / 4);

			*pUVPlane = U;	++pUVPlane;
			*pUVPlane = V;	++pUVPlane;
		}

		// set current odd line pointer back
		if (is_bottom_up)
		{
			pRGBodd  = rgb + ((height - y - 2) * (width * 3));
		}
		else
		{
			pRGBodd  = rgb + ((y + 1) * (width * 3));
		}

		// process odd Y pixels 
		for (x = 0; x < width; ++x) 
		{ 
			B = *pRGBodd; 	++pRGBodd;
			G = *pRGBodd;	++pRGBodd;
			R = *pRGBodd;	++pRGBodd;

			Y =  RGBtoY(R, G, B);				

			*pYPlane = Y;  ++pYPlane;
		}

	} // end of for(y)

	return pNV12Frame;
}


////////////////////////////////////////////////////////////////////////////////////

// reads NV12 frame from specified file, returns not NULL if success
BYTE* get_nv12(char *fname, LONG width, LONG height) 
{
    FILE 	*fi;
	BYTE 	*pNV12Frame;
	DWORD	unDataBytes;
	struct 	stat st;

	if ( !(fi = fopen(fname, "rb")) )
	{
	    fprintf(stderr, "\nError: Unable to open file %s\n", fname);
		return NULL;
	}

	unDataBytes = (DWORD)(width * height * 1.5);
	// check out the input file size
	stat(fname, &st); // use posix way
	if (st.st_size != unDataBytes)
	{
	    fprintf(stderr, "\nError: Invalid file size %s, or specified WxH\n", 
			fname);
	    fclose(fi);
		return NULL;
	}

	// allocate memory for RGB data
	if ( !(pNV12Frame = (BYTE*)malloc(unDataBytes)) )
	{
    	fprintf(stderr, "Error: Out of memory");
	    fclose(fi);
		return NULL;
	}
	memset(pNV12Frame, 0x00, unDataBytes);

	// read bitmap file header
    if (fread(pNV12Frame, unDataBytes, 1, fi) != 1)
	{
	    fprintf(stderr, "\nError: Unable to read file %s\n", fname);
	    fclose(fi);
		return NULL;
	}

	return pNV12Frame;
}

// converts NV12 pixelformat frame to 24 bits-per-pixel bitmap RGB data,
// returns not NULL if success and RGB data size in 'rgb_bytes'
BYTE* set_rgb(BYTE *nv12, LONG width, LONG height, DWORD *rgb_bytes)
{
	LONG 	x, y; 
	BYTE	*pRGBData, *pYPeven, *pYPodd, *pUVPlane, *pRGBeven, *pRGBodd;


	// check out the nv12 frame size conditions
	if ((width % 4) || (height % 4))
	{
    	fprintf(stderr, "Error: The Width and Height must be multiple of Four");
		return NULL;
	}

	*rgb_bytes = (DWORD)(width * 3 * height);

	if (!(pRGBData = (BYTE*)malloc(*rgb_bytes)))
	{
    	fprintf(stderr, "Error: Out of memory");
		return NULL;
	}
	memset(pRGBData, 0xFF, *rgb_bytes);

	pUVPlane = nv12 + (width * height);

	for(y = 0; y < height; y += 2)
	{
		// even line
	    pRGBeven = pRGBData + (width * 3 * (height - 1 - y));
		pYPeven = nv12 + (width * y);
		// odd line
	    pRGBodd = pRGBData + (width * 3 * (height - 2 - y)); 
		pYPodd = nv12 + (width * (y + 1));

	    for(x = 0; x < width; x++)
	    {
		    *(pRGBeven + 0 + x * 3) = YUVtoB(*pYPeven, *pUVPlane, *(pUVPlane + 1));
		    *(pRGBeven + 1 + x * 3) = YUVtoG(*pYPeven, *pUVPlane, *(pUVPlane + 1));
		    *(pRGBeven + 2 + x * 3) = YUVtoR(*pYPeven, *pUVPlane, *(pUVPlane + 1));

		    *(pRGBodd + 0 + x * 3) = YUVtoB(*pYPodd, *pUVPlane, *(pUVPlane + 1));
		    *(pRGBodd + 1 + x * 3) = YUVtoG(*pYPodd, *pUVPlane, *(pUVPlane + 1));
		    *(pRGBodd + 2 + x * 3) = YUVtoR(*pYPodd, *pUVPlane, *(pUVPlane + 1));

			pYPeven++; 	pYPodd++;
			if (x % 2) pUVPlane += 2;
	    }
	}

	return pRGBData;
}



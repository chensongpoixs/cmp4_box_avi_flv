<font face="微软雅黑"  >

###  一, yuv420p分析

YUV420P在内存是这样存放的

<font color = red size = 4>一帧的YUV420P数据</font>

|Y|Y|Y|Y|Y|Y|Y|......|宽度
|-|-|-|-|-|-|
|Y|Y|Y|Y|Y|Y|Y|......
|Y|Y|Y|Y|Y|Y|Y|......
|Y|Y|Y|Y|Y|Y|Y|......
|U|U|U|U|U|U|U|......
|V|V|V|V|V|V|V|......


    unsigned char *pic=(unsigned char *)malloc(w*h*3/2);  
  	//num是帧数默认是1
    for(int i=0;i<num;i++){  
  
        fread(pic,1,w*h*3/2,fp);  
        //Y  
        fwrite(pic,1,w*h,fp1);  
        //U  
        fwrite(pic+w*h,1,w*h/4,fp2);  
        //V  
        fwrite(pic+w*h*5/4,1,w*h/4,fp3);  
    }  
  

 <font size = 3 > 如果视频帧的宽和高分别为w和h，那么一帧YUV420P像素数据一共占用w*h*3/2 Byte的数据。
其中前w*h Byte存储Y，接着的w*h*1/4 Byte存储U，最后w*h*1/4 Byte存储V。
上述调用函数的代码运行后，将会把一张分辨率为256x256的名称为lena_256x256_yuv420p.yuv
的YUV420P格式的像素数据文件分离成为三个文件

</font>

### 二, YUV444P分析

	unsigned char *pic=(unsigned char *)malloc(w*h*3);  
  
    for(int i=0;i<num;i++){  
        fread(pic,1,w*h*3,fp);  
        //Y  
        fwrite(pic,1,w*h,fp1);  
        //U  
        fwrite(pic+w*h,1,w*h,fp2);  
        //V  
        fwrite(pic+w*h*2,1,w*h,fp3);  
    }  

 <font size = 3 > 如果视频帧的宽和高分别为w和h，那么一帧YUV444P像素数据一共占用w*h*3 Byte的数据。其中前w*h Byte存储Y，接着的w*h Byte存储U，最后w*h Byte存储V。上述调用函数的代码运行后，将会把一张分辨率为256x256的名称为lena_256x256_yuv444p.yuv的YUV444P格式的像素数据文件分离成为三个文件：
</font>


### 三, 将YUV420P像素数据去掉颜色（变成灰度图）


	unsigned char *pic=(unsigned char *)malloc(w*h*3/2);  
  
    for(int i=0;i<num;i++){  
        fread(pic,1,w*h*3/2,fp);  
        //Gray    末尾添加该U,和V的数据
        memset(pic+w*h,128,w*h/2);  
        fwrite(pic,1,w*h*3/2,fp1);  
    } 



|Y|Y|Y|Y|U|V|
|-|-|-|-|-|-|
|Y|Y|Y|Y|128|128|

<font size = 3  > 如果想把YUV格式像素数据变成灰度图像，只需要将U、V分量设置成128即可。这是因为U、V是图像中的经过偏置处理的色度分量。色度分量在偏置处理前的取值范围是-128至127，这时候的无色对应的是“0”值。经过偏置后色度分量取值变成了0至255，因而此时的无色对应的就是128了。上述调用函数的代码运行后，将会把一张分辨率为256x256的名称为lena_256x256_yuv420p.yuv的YUV420P格式的像素数据文件处理成名称为output_gray.yuv的YUV420P格式的像素数据文件。输入的原图如下所示。</font>


### 四, 将YUV420P像素数据的亮度减半

	unsigned char *pic=(unsigned char *)malloc(w*h*3/2);  
  
    for(int i=0;i<num;i++){  
        fread(pic,1,w*h*3/2,fp);  
        //Half  
        for(int j=0;j<w*h;j++){  
            unsigned char temp=pic[j]/2;  
            //printf("%d,\n",temp);  
            pic[j]=temp;  
        }  
        fwrite(pic,1,w*h*3/2,fp1);  
    }  

修改过后的结果

|Y|Y|Y|Y|U|V|
|-|-|-|-|-|-|
|Y/2|Y/2|Y/2|Y/2|U|V|

<font size = 3> 如果打算将图像的亮度减半，只要将图像的每个像素的Y值取出来分别进行除以2的工作就可以了。图像的每个Y值占用1 Byte，取值范围是0至255，对应C语言中的unsigned char数据类型。上述调用函数的代码运行后，将会把一张分辨率为256x256的名称为lena_256x256_yuv420p.yuv的YUV420P格式的像素数据文件处理成名称为output_half.yuv的YUV420P格式的像素数据文件。输入的原图如下所示。</font>

### 五, 将YUV420P像素数据的周围加上边框

	unsigned char *pic=(unsigned char *)malloc(w*h*3/2);  
  	
    for(int i=0;i<num;i++){  
        fread(pic,1,w*h*3/2,fp);  
        //Y  
        for(int j=0;j<h;j++){  
            for(int k=0;k<w;k++){  
				//border是边框的宽度
                if(k<border||k>(w-border)||j<border||j>(h-border)){  
                    pic[j*w+k]=255;  
                    //pic[j*w+k]=0;  
                }  
            }  
        }  
        fwrite(pic,1,w*h*3/2,fp1);  
    }  

<font size = 3>图像的边框的宽度为border，本程序将距离图像边缘border范围内的像素的亮度分量Y的取值设置成了亮度最大值255。上述调用函数的代码运行后，将会把一张分辨率为256x256的名称为lena_256x256_yuv420p.yuv的YUV420P格式的像素数据文件处理成名称为output_border.yuv的YUV420P格式的像素数据文件</font>

### 六, 分离RGB24像素数据中的R、G、B分量

	unsigned char *pic=(unsigned char *)malloc(w*h*3);  
  
    for(int i=0;i<num;i++){  
  
        fread(pic,1,w*h*3,fp);  
  
        for(int j=0;j<w*h*3;j=j+3){  
            //R  
            fwrite(pic+j,1,1,fp1);  
            //G  
            fwrite(pic+j+1,1,1,fp2);  
            //B  
            fwrite(pic+j+2,1,1,fp3);  
        }  
    }  

RGB24文件存放格式

|R|G|B|R|G|B|R|G|B|R|G|B|
|-|-|-|-|-|-|-|-|-|-|-|-|
|R|G|B|R|G|B|R|G|B|R|G|B|
|..|.|.|.|.|.|.|.|.|.|

<font size = 3> 与YUV420P三个分量分开存储不同，RGB24格式的每个像素的三个分量是连续存储的。一帧宽高分别为w、h的RGB24图像一共占用w*h*3 Byte的存储空间。RGB24格式规定首先存储第一个像素的R、G、B，然后存储第二个像素的R、G、B…以此类推。类似于YUV420P的存储方式称为Planar方式，而类似于RGB24的存储方式称为Packed方式。上述调用函数的代码运行后，将会把一张分辨率为500x500的名称为cie1931_500x500.rgb的RGB24格式的像素数据文件分离成为三个文件：</font>



### 七,将RGB24格式像素数据封装为BMP图像


	int songli_rgb24_to_bmp(const char *rgb24path,int width,int height,const char *bmppath){  
    typedef struct   
    {    
        long imageSize;  
        long blank;  
        long startPosition;  
    }BmpHead;  
  
    typedef struct  
    {  
        long  Length;  
        long  width;  
        long  height;  
        unsigned short  colorPlane;  
        unsigned short  bitColor;  
        long  zipFormat;  
        long  realSize;  
        long  xPels;  
        long  yPels;  
        long  colorUse;  
        long  colorImportant;  
    }InfoHead;  
  
    int i=0,j=0;  
    BmpHead m_BMPHeader={0};  
    InfoHead  m_BMPInfoHeader={0};  
    char bfType[2]={'B','M'};  //文件类型(BMP)
    int header_size=sizeof(bfType)+sizeof(BmpHead)+sizeof(InfoHead);  //文件头大小是14 字节, 位图信息头(40个字节) 
    unsigned char *rgb24_buffer=NULL;  
    FILE *fp_rgb24=NULL,*fp_bmp=NULL;  
  
    if((fp_rgb24=fopen(rgb24path,"rb"))==NULL){  
        printf("Error: Cannot open input RGB24 file.\n");  
        return -1;  
    }  
    if((fp_bmp=fopen(bmppath,"wb"))==NULL){  
        printf("Error: Cannot open output BMP file.\n");  
        return -1;  
    }  
  
    rgb24_buffer=(unsigned char *)malloc(width*height*3);  
    fread(rgb24_buffer,1,width*height*3,fp_rgb24);  
    //文件的大小四个字节
    m_BMPHeader.imageSize=3*width*height+header_size;
  	//位图数据的起始位置四个字节
    m_BMPHeader.startPosition=header_size;  
  	//本结构所占用字节数40个字节
    m_BMPInfoHeader.Length=sizeof(InfoHead);
	//位图的宽度像素   
    m_BMPInfoHeader.width=width;  
    //BMP storage pixel data in opposite direction of Y-axis (from bottom to top).  
    m_BMPInfoHeader.height=-height;  
    m_BMPInfoHeader.colorPlane=1;  
	//
    m_BMPInfoHeader.bitColor=24;  
    m_BMPInfoHeader.realSize=3*width*height;  
  
    fwrite(bfType,1,sizeof(bfType),fp_bmp);  
    fwrite(&m_BMPHeader,1,sizeof(m_BMPHeader),fp_bmp);  
    fwrite(&m_BMPInfoHeader,1,sizeof(m_BMPInfoHeader),fp_bmp);  
  
    //BMP save R1|G1|B1,R2|G2|B2 as B1|G1|R1,B2|G2|R2  
    //It saves pixel data in Little Endian  
    //So we change 'R' and 'B'  
    for(j =0;j<height;j++){  
        for(i=0;i<width;i++){  
            char temp=rgb24_buffer[(j*width+i)*3+2];  
            rgb24_buffer[(j*width+i)*3+2]=rgb24_buffer[(j*width+i)*3+0];  
            rgb24_buffer[(j*width+i)*3+0]=temp;  
        }  
    }  
    fwrite(rgb24_buffer,3*width*height,1,fp_bmp);  
    fclose(fp_rgb24);  
    fclose(fp_bmp);  
    free(rgb24_buffer);  
    printf("Finish generate %s!\n",bmppath);  
    return 0;  
    return 0;  
	}  

BMP文件RGB的排序

|R|B|G|R|B|G|R|B|G|
|-|-|-|-|-|-|-|-|
|R|B|G|R|B|G|R|B|G|
|..|..|..|..|..|..|..|..|..|


<font size = 3> 改程序完成了主要完成了两个工作：
	1)将RGB数据前面加上文件头。
	2)将RGB数据中每个像素的“B”和“R”的位置互换。
BMP文件是由BITMAPFILEHEADER、BITMAPINFOHEADER、RGB像素数据共3个部分构成，它的结构如下图所示。</font>

|BITMAPFILEHEADER(14个字节)|
|-|
|BITMAPINFOHEADER(40个字节)|
|RGB像素数据|


</font>
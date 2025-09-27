#include <unistd.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <stdio.h>  
#include <sys/ioctl.h>  
#include <stdlib.h>  
#include <linux/types.h>  
#include <linux/videodev2.h>  
#include <malloc.h>  
#include <math.h>  
#include <string.h>  
#include <sys/mman.h>  
#include <errno.h>  
#include <assert.h>  
#include <sys/time.h>  
#include "utils.h"  

#define WIDTH       640  
#define HIGHT       480  
#define FILE_VIDEO  "/dev/video0"  
#define JPG "./out/image%d"  

typedef struct {
	voidvoid *start;
	int length;
}BUFTYPE;
BUFTYPE *usr_buf;

static unsigned int n_buffer = 0;
struct timeval time;
unsigned char* mjpeg_buff;
unsigned char* yuyv_buff;
unsigned char* yuv_buff;

float get_main_time(struct timeval* start, int update)
{
	float dt;
	struct timeval now;
	gettimeofday(&now, NULL);
	dt = (float)(now.tv_sec - start->tv_sec);
	dt += (float)(now.tv_usec - start->tv_usec) * 1e-6;

	if (update > 0) {
		start->tv_sec = now.tv_sec;
		start->tv_usec = now.tv_usec;
	}

	return dt;
}


int init_mmap(int fd)
{
	/*to request frame cache, contain requested counts*/
	struct v4l2_requestbuffers reqbufs;

	memset(&reqbufs, 0, sizeof(reqbufs));
	reqbufs.count = 4;
	reqbufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbufs.memory = V4L2_MEMORY_MMAP;

	if (-1 == ioctl(fd, VIDIOC_REQBUFS, &reqbufs))
	{
		perror("Fail to ioctl 'VIDIOC_REQBUFS'");
		exit(EXIT_FAILURE);
	}

	n_buffer = reqbufs.count;
	printf("n_buffer = %d\n", n_buffer);

	usr_buf = calloc(reqbufs.count, sizeof(BUFTYPE));
	if (usr_buf == NULL)
	{
		printf("Out of memory\n");
		exit(-1);
	}

	/*map kernel cache to user process*/
	for (n_buffer = 0; n_buffer < reqbufs.count; ++n_buffer)
	{
		//stand for a frame  
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffer;

		/*check the information of the kernel cache requested*/
		if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buf))
		{
			perror("Fail to ioctl : VIDIOC_QUERYBUF");
			exit(EXIT_FAILURE);
		}

		usr_buf[n_buffer].length = buf.length;
		usr_buf[n_buffer].start = (charchar *)mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, buf.m.offset);

		if (MAP_FAILED == usr_buf[n_buffer].start)
		{
			perror("Fail to mmap");
			exit(EXIT_FAILURE);
		}
	}
}

int open_camera(void)
{
	int fd;
	/*open video device with block */
	fd = open(FILE_VIDEO, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "%s open err \n", FILE_VIDEO);
		exit(EXIT_FAILURE);
	};
	return fd;
}

void init_mjpeg_encode(void)
{
	mjpeg_buff = (unsigned char*)malloc(WIDTH * HIGHT * 2);
	if (mjpeg_buff == NULL)
	{
		perror("mjpeg_encode malloc err\n");
	};

	yuyv_buff = (unsigned char*)malloc(WIDTH * HIGHT * 2);
	if (yuyv_buff == NULL)
	{
		perror("mjpeg_encode malloc err\n");
	};

	yuv_buff = (unsigned char*)malloc(WIDTH * HIGHT * 2);
	if (yuyv_buff == NULL)
	{
		perror("mjpeg_encode malloc err\n");
	};

	memset(yuv_buff, 0, WIDTH * HIGHT * 2);

}

void release_mjpeg(void)
{
	free(mjpeg_buff);
	free(yuyv_buff);
	free(yuv_buff);
}

int init_camera(int fd)
{
	struct v4l2_capability  cap;    /* decive fuction, such as video input */
	struct v4l2_format  tv_fmt; /* frame format */
	struct v4l2_fmtdesc     fmtdesc;/* detail control value */
	struct v4l2_control     ctrl;
	int ret;

	/*show all the support format*/
	memset(&fmtdesc, 0, sizeof(fmtdesc));
	fmtdesc.index = 0;                 /* the number to check */
	fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	/* check video decive driver capability */
	if (ret = ioctl(fd, VIDIOC_QUERYCAP, &cap)<0)
	{
		fprintf(stderr, "fail to ioctl VIDEO_QUERYCAP \n");
		exit(EXIT_FAILURE);
	}

	/*judge wherher or not to be a video-get device*/
	if (!(cap.capabilities & V4L2_BUF_TYPE_VIDEO_CAPTURE))
	{
		fprintf(stderr, "The Current device is not a video capture device \n");
		exit(EXIT_FAILURE);
	}

	/*judge whether or not to supply the form of video stream*/
	if (!(cap.capabilities & V4L2_CAP_STREAMING))
	{
		printf("The Current device does not support streaming i/o\n");
		exit(EXIT_FAILURE);
	}

	printf("\ncamera driver name is : %s\n", cap.driver);
	printf("camera device name is : %s\n", cap.card);
	printf("camera bus information: %s\n", cap.bus_info);

	/*display the format device support*/
	while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1)
	{
		printf("\nsupport device %d.%s\n\n", fmtdesc.index + 1, fmtdesc.description);
		fmtdesc.index++;
	}


	/*set the form of camera capture data*/
	tv_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;      /*v4l2_buf_typea,camera must use V4L2_BUF_TYPE_VIDEO_CAPTURE*/
	tv_fmt.fmt.pix.width = 680;
	tv_fmt.fmt.pix.height = 480;
	tv_fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG; /*V4L2_PIX_FMT_YYUV*/
	tv_fmt.fmt.pix.field = V4L2_FIELD_NONE;     /*V4L2_FIELD_NONE V4L2_FIELD_INTERLACED */
	if (ioctl(fd, VIDIOC_S_FMT, &tv_fmt)< 0)
	{
		fprintf(stderr, "VIDIOC_S_FMT set err\n");
		exit(-1);
		close(fd);
	}

	init_mmap(fd);
}

int start_capture(int fd)
{
	unsigned int i;
	enum v4l2_buf_type type;

	/*place the kernel cache to a queue*/
	for (i = 0; i < n_buffer; i++)
	{
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == ioctl(fd, VIDIOC_QBUF, &buf))
		{
			perror("Fail to ioctl 'VIDIOC_QBUF'");
			exit(EXIT_FAILURE);
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(fd, VIDIOC_STREAMON, &type))
	{
		printf("i=%d.\n", i);
		perror("VIDIOC_STREAMON");
		close(fd);
		exit(EXIT_FAILURE);
	}

	return 0;
}

int process_image(voidvoid *addr, int length)
{
	FILEFILE *fp;
	static int num = 0;
	char image_name[20];

	sprintf(image_name, JPG, num++);
	if ((fp = fopen(image_name, "w")) == NULL)
	{
		perror("Fail to fopen");
		exit(EXIT_FAILURE);
	}
	fwrite(addr, WIDTH*HIGHT * 2, 1, fp);
	usleep(500);
	fclose(fp);
	return 0;
}



int read_frame(int fd)
{
	struct v4l2_buffer buf;
	unsigned int i;
	int k = WIDTH;
	int j = HIGHT;

	memset(&buf, 0, sizeof(struct v4l2_buffer));
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == ioctl(fd, VIDIOC_DQBUF, &buf))
	{
		perror("Fail to ioctl 'VIDIOC_DQBUF'");
		exit(EXIT_FAILURE);
	}

	assert(buf.index < n_buffer);
	memcpy(mjpeg_buff, usr_buf[buf.index].start, usr_buf[buf.index].length);

	jpeg_decode(&yuyv_buff, mjpeg_buff, &k, &j);
	process_image(yuyv_buff, WIDTH * HIGHT * 2);

	if (-1 == ioctl(fd, VIDIOC_QBUF, &buf))
	{
		perror("Fail to ioctl 'VIDIOC_QBUF'");
		exit(EXIT_FAILURE);
	}
	return 1;
}


int mainloop(int fd)
{
	int count = 20;
	while (count-- > 0)
	{
		for (;;)
		{
			fd_set fds;
			struct timeval tv;
			int r;

			FD_ZERO(&fds);
			FD_SET(fd, &fds);

			/*Timeout*/
			tv.tv_sec = 0;
			tv.tv_usec = 200000;
			r = select(fd + 1, &fds, NULL, NULL, &tv);

			if (-1 == r)
			{
				if (EINTR == errno)
					continue;
				perror("Fail to select");
				exit(EXIT_FAILURE);
			}
			if (0 == r)
			{
				fprintf(stderr, "select Timeout\n");
				exit(-1);
			}

			if (read_frame(fd))
			{
				break;
			}
		}
	}
	return 0;
}

void stop_capture(int fd)
{
	enum v4l2_buf_type type;
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(fd, VIDIOC_STREAMOFF, &type))
	{
		perror("Fail to ioctl 'VIDIOC_STREAMOFF'");
		exit(EXIT_FAILURE);
	}
}

void close_camera_device(int fd)
{
	unsigned int i;
	for (i = 0; i < n_buffer; i++)
	{
		if (-1 == munmap(usr_buf[i].start, usr_buf[i].length))
		{
			exit(-1);
		}
	}

	free(usr_buf);

	if (-1 == close(fd))
	{
		perror("Fail to close fd");
		exit(EXIT_FAILURE);
	}
}

void main(void)
{
	int fd;
	float ret = 0;
	fd = open_camera();
	init_mjpeg_encode();
	init_camera(fd);
	start_capture(fd);
	ret = get_main_time(&time, 1);
	mainloop(fd);
	ret = get_main_time(&time, 1);
	printf("encode spend time = %f\n", ret);
	stop_capture(fd);
	release_mjpeg();
	close_camera_device(fd);
}
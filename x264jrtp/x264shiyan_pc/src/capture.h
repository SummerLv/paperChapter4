//#include <stdio.h>  
//#include <stdlib.h>  
//#include <string.h>  
//#include <assert.h>  
  
//#include <getopt.h>             /* getopt_long() */  
#ifndef CAPTURE_H
#define CAPTURE_H
#ifdef __cplusplus
extern "C"
{
#endif
#define CLEAR(x) memset (&(x), 0, sizeof (x))  
  
typedef enum {  
    IO_METHOD_READ, IO_METHOD_MMAP, IO_METHOD_USERPTR,  
} io_method;  
  
struct buffer {  
    void * start;  
    size_t length;  
};  
  
extern char * dev_name ; 
extern int frame_num; 
extern io_method io ;  
extern int fd ;  
extern struct buffer * buffers;  
extern unsigned int n_buffers ;  
  
extern FILE *fp;  
extern const char *filename; 


extern FILE *fpYUYV ;
extern FILE *fpYUV420 ;
extern FILE *fpH264 ;

extern FILE *tmpFP264;


void openFiles(void);

void errno_exit(const char * s);
int xioctl(int fd, int request, void * arg);
void process_image(const void * p, int size);
int read_frame(void);
void mainloop(int frame_num);
void stop_capturing(void);
void start_capturing(void);
void uninit_device(void);
void init_read(unsigned int buffer_size);
void init_mmap(void);
void init_userp(unsigned int buffer_size);
void init_device(void);
void close_device(void);
void open_device(void);
#ifdef __cplusplus
}


#endif
#endif


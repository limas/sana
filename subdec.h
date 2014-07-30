#ifndef _SUB_DEC_H_
#define _SUB_DEC_H_

#ifndef BOOL
#define BOOL unsigned char
#endif

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

typedef enum
{
    CLR_FMT_ARGB8888 = 0,
    CLR_FMT_I8,

    CLR_FMT_ALL
}eClrFmt;

struct buf_info
{
    unsigned int size;
    unsigned char *buf;
};

typedef void (*sub_draw_cb)(unsigned char *data, eClrFmt color,unsigned int width, unsigned int height, unsigned int pitch);

extern BOOL dvb_sub_init(sub_draw_cb func);
extern BOOL dvb_sub_deinit(void);
extern int dvb_sub_dec(unsigned char *pes_buf, unsigned int pes_len);


#endif

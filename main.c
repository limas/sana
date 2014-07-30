#include <stdlib.h>
#include <stdio.h>

#include "subdec.h"

#define MAX_PES_LEN (4096)

static unsigned char _gdata_buf[MAX_PES_LEN];

#define _debug_ (1)

static void _sub_draw(unsigned char *data, eClrFmt color, unsigned int width, unsigned int height, unsigned int pitch)
{
    /* save subtitle page as bitmap */
    printf(
        "%s:\n"
        "data = 0x%p\n"
        "color = %d\n"
        "width = %u\n"
        "height = %u\n"
        "pitch = %u\n",
        __FUNCTION__,
        data,
        (unsigned int)color,
        width,
        height,
        pitch);
}

int main(int argc, char **argv)
{
    unsigned int read_len;
    unsigned char *data;
    FILE *fp_in = NULL;

    /* trigger dvb sutitle decoder */
    dvb_sub_init(_sub_draw);
    
#if _debug_
    {
        /* test data for DDS without display_window_flag */
        unsigned char test_data[]={0x20, 0x00, 0x0F, 0x14, 0x55, 0x66, 0x00, 0x05, 0x10, 0x07, 0x80, 0x04, 0x38, 0xFF};
        /* test data for DDS with display_window_flag */
        unsigned char test_data2[]={0x20, 0x00, 0x0F, 0x14, 0x55, 0x66, 0x00, 0x0D, 0x18, 0x07, 0x80, 0x04, 0x38, 0x00, 0x10, 0x00, 0x20, 0x00, 0x20, 0x00, 0x40, 0xFF};

        #define test_data_sel test_data2

        if(dvb_sub_dec(test_data_sel, sizeof(test_data_sel)) == 0)
        {
            printf("decode error.\n");
            return -1;
        }
        printf("decode success.\n");
        return 0;
    }
#else
    if(argc < 2)
    {
        printf("no input file specified, use stdin as input source.\n");
        fp_in = stdin;
    }
    else
    {
        fp_in = fopen(argv[1], "rb");
        if(!fp_in)
        {
            printf("can not open file.\n");
            return -1;
        }
    }

    read_len = 0;
    while(!feof(fp_in))
    {
        read_len = (unsigned int)fread((void *)_gdata_buf, 1, MAX_PES_LEN, fp_in);
        if(read_len)
            continue;

        if(dvb_sub_dec(_gdata_buf, read_len) == 0)
        {
            printf("decode subtitle error.\n");
            
        }
    }
#endif
    return 0;
}


#include <string.h>

#include "subdec.h"
#include "substream.h"
#include "submisc.h"

static const unsigned char sub_def_2_to_4_map[]=
{0x00, 0x07, 0x08, 0x0F};
static const unsigned char sub_def_2_to_8_map[]=
{0x00, 0x77, 0x88, 0xFF};
static const char sub_def_4_to_8_map[]=
{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

unsigned int sub_2bit_code_str_dec(
    struct stream *strm,
    struct buf_info *buf
)
{
    unsigned char val;
    unsigned char run_len = 1;
    unsigned char pix;
    unsigned int pos = 0;
    unsigned int ret = 0;

    if(!strm || !buf)
    {
        return 0;
    }

    do
    {
        if(!stream_get_bits(strm, &val, 2))
        {
            break;
        }

        if(val != 0x00)
        {
            pix = val;
        }
        else
        {
            if(!stream_get_bits(strm, &val, 1))
            {
                break;
            }

            if(val == 1) /* switch 1 */
            {
                if(!stream_get_bits(strm, &run_len, 3) || !stream_get_bits(strm, &pix, 2))
                {
                    break;
                }

                run_len += 3;
            }
            else
            {
                if(!stream_get_bits(strm, &val, 1))
                {
                    break;
                }

                if(val == 0) /* switch 2 */
                {
                    if(!stream_get_bits(strm, &val, 2))
                    {
                        break;
                    }
                    
                    if(val == 0x00) /* end of 2-bit pixel code */
                    {
                        run_len = 0;
                        /* do byte alignment */
                    }
                    else if(val == 0x01)
                    {
                        run_len = 2;
                        pix = 0;
                    }
                    else if(val == 0x02)
                    {
                        if(!stream_get_bits(strm, &run_len, 4) || !stream_get_bits(strm, &pix, 2))
                        {
                            break;
                        }

                        run_len += 12;
                    }
                    else if(val == 0x03)
                    {
                        if(!stream_get_bits(strm, &run_len, 8) || !stream_get_bits(strm, &pix, 2))
                        {
                            break;
                        }

                        run_len += 29;
                    }
                }
                else
                {
                    run_len = 1;
                    pix = 0;
                }
            }
        }

        /* fill data into input buffer */
        if(run_len)
        {
            if(buf->buf)
            {
                memset((void *)&buf->buf[pos], pix, (pos+run_len>buf->size)?(buf->size-pos):(run_len));
            }
        }
        else
        {
            break;
        }

        ret = TRUE;
    }while(0);

    return ret;
}

unsigned int sub_4bit_code_str_dec(
    struct stream *strm,
    struct buf_info *buf
)
{
    unsigned int ret = 0;

    return ret;
}

unsigned int sub_8bit_code_str_dec(
    struct stream *strm,
    struct buf_info *buf
)
{
    unsigned int ret = 0;

    return ret;
}

struct color sub_YCbCrT_to_ARGB(struct color clr)
{
    int a;

}


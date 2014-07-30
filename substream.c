#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "subdec.h"
#include "substream.h"

void stream_reset(struct stream *strm)
{
    strm->copy = FALSE;
    strm->len = 0;
    strm->oft = 0;
    strm->bit_oft = 0;
}

void stream_destroy(struct stream *strm)
{
    if(strm->copy == STREAM_CREATE_COPY)
    {
        if(strm->buf)
        {
            free(strm->buf);
        }
    }

    strm->buf = NULL;
    stream_reset(strm);
}

BOOL stream_create(struct stream *strm, eSTREAM_CREATE_TYPE type, unsigned char *buf, unsigned int len)
{
    if(!buf || len ==0)
    {
        return FALSE;
    }

    do
    {
        if(type == STREAM_CREATE_COPY)
        {
            /* allocate buffer to store stream data */
            strm->buf = (unsigned char *)malloc(len);
            if(!strm->buf)
            {
                break;
            }

            memcpy((void *)strm->buf, (void *)buf, len);
            strm->copy = TRUE;
        }
        else
        {
            strm->buf = buf;
            strm->copy = FALSE;
        }

        strm->len = len;
        strm->oft = 0;
        strm->bit_oft = 0;

        return TRUE;
    }while(0);

    stream_destroy(strm);

    return FALSE;
}

BOOL stream_get_bits(struct stream *strm, unsigned char *buf, unsigned char bit)
{
    unsigned char val;
    unsigned char res;

    res = 8 - strm->bit_oft;
    if((strm->oft == strm->len) && (bit > res))
        return FALSE;

    val = strm->buf[strm->oft];
    if(bit > res)
    {
        strm->bit_oft = bit-res;
        
        val &= ((1<<res)-1);
        val <<= (strm->bit_oft);
        val |= ((strm->buf[strm->oft+1] >> (8-strm->bit_oft)) & ((1<<strm->bit_oft)-1));

        strm->oft++;
    }
    else
    {
        strm->bit_oft+=bit;

        val >>= (8 - strm->bit_oft);
        val &= ((1<<bit)-1);

        if(strm->bit_oft == 8)
        {
            strm->bit_oft = 0;
            strm->oft++;
        }
    }

    *buf = val;
    return TRUE;
}

BOOL stream_get_bytes(struct stream *strm, unsigned char *buf, unsigned char byte)
{
    unsigned char idx = 0;

    if(strm->bit_oft || ((strm->len - strm->oft)<byte))
    {
        return FALSE;
    }

    while(idx < byte)
    {
        buf[idx] = strm->buf[strm->oft+idx];
        idx++;
    }

    strm->oft+=byte;

    return TRUE;
}

BOOL stream_get_short(struct stream *strm, unsigned short *buf)
{
    if(strm->bit_oft || ((strm->len - strm->oft)<2))
    {
        return FALSE;
    }

    *buf = ((strm->buf[strm->oft])<<8) + strm->buf[strm->oft+1];
    strm->oft+=2;

    return TRUE;
}

BOOL stream_peek_bits(struct stream *strm, unsigned char *buf, unsigned char bit)
{
    unsigned char val;
    unsigned char oft;
    unsigned char res;

    oft = strm->bit_oft;
    res = 8 - oft;
    if((oft == strm->len) && (bit > res))
        return FALSE;

    val = strm->buf[strm->oft];
    if(bit > res)
    {
        oft = bit-res;
        
        val &= ((1<<res)-1);
        val <<= (oft);
        val |= ((strm->buf[strm->oft+1] >> (8-oft)) & ((1<<oft)-1));
    }
    else
    {
        oft+=bit;

        val >>= (8 - oft);
        val &= ((1<<bit)-1);
    }

    *buf = val;
    return TRUE;
}

BOOL stream_drop_bits(struct stream *strm, unsigned char bit)
{
    if((((strm->len - strm->oft)<<3) + (8 - strm->bit_oft)) < bit)
        return FALSE;

    strm->bit_oft += (bit%8);
    strm->oft += (bit>>3);
    if(strm->bit_oft>=8)
    {
        strm->oft ++;
        strm->bit_oft %= 8;
    }
    
    return TRUE;
}

void stream_align_bytes(struct stream *strm, unsigned char byte)
{
    if(byte)
    {
        if(strm->bit_oft)
        {
            strm->oft++;
            strm->bit_oft = 0;
        }

        if((strm->oft%byte)!=0)
        {
            strm->oft+=(byte-1);
            strm->oft/=byte;
            strm->oft*=byte;
        }

        if(strm->oft>strm->len)
        {
            strm->oft=strm->len;
        }
    }
}

int stream_get_oft(struct stream *strm)
{
    return strm->oft;
}

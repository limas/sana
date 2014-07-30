#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "subdec.h"
#include "substream.h"
#include "submisc.h"

/* define macro */
#define SEG_PAGE_COMP (0x10)
#define SEG_REG_COMP (0x11)
#define SEG_CLUT_DEF (0x12)
#define SEG_OBJ_DATA (0x13)
#define SEG_DISP_DEF (0x14)
#define SEG_DISP_SIG (0x15)
#define SEG_END_DISP_SET (0x80)
#define SEG_STUF (0xFF)

#define MAX_NUM_REG (32)
#define MAX_NUM_OBJ (64)

#define MAX_REG_PER_PAGE (16)
#define MAX_OBJ_PER_REGION (32)

#define _DEBUG(x) x

/* enum, structure, union */
struct seg_header
{
    unsigned char sync;
    unsigned char seg_type;
    unsigned short page_id;
    unsigned short seg_len;
};

struct disp_info
{
    unsigned char version;
    unsigned short win_width;
    unsigned short win_height;
    unsigned short canv_x;
    unsigned short canv_y;
    unsigned short canv_width;
    unsigned short canv_height;
    unsigned short canv_pitch;
    eClrFmt canv_color;
};

struct reg_lst
{
    unsigned char reg_id;
    unsigned short reg_h_addr;
    unsigned short reg_v_addr;
};

struct obj_lst
{
    unsigned short id;
    unsigned char type;
    unsigned char prvdr;
    unsigned short h_pos;
    unsigned short v_pos;
    unsigned char forgnd_pix_code;
    unsigned char bakgnd_pix_code;
};

struct page_info
{
    unsigned char version;
    unsigned char timeout;
    unsigned char state;
    unsigned int num_reg;
    struct reg_lst act_reg_lst[MAX_REG_PER_PAGE];
};

struct reg_info
{
    unsigned char id;
    unsigned char version;
    BOOL fill;
    unsigned short width;
    unsigned short height;
    unsigned char compt_level;
    unsigned char reg_depth;
    unsigned char clut_id;
    unsigned char reg_8bit_pix_code;
    unsigned char reg_4bit_pix_code;
    unsigned char reg_2bit_pix_code;
    unsigned int num_obj;
    struct obj_lst obj_lst[MAX_OBJ_PER_REGION];
};

struct obj_info
{
    unsigned short id;
    unsigned char version;
    unsigned char code_methd;
    BOOL non_modify;
};

typedef enum
{
    PAGE_STATE_NORMAL = 0,
    PAGE_STATE_ACQ_POINT,
    PAGE_STATE_MODE_CHNG,

    PAGE_STATE_ALL
}ePAGE_STATE;

/* internal global variable */
static sub_draw_cb _gpf_draw_cb;

static struct seg_header _gseg_hdr;
static struct disp_info _gdisp_info;
static struct page_info _gpage_info;
static struct reg_info _greg_info[MAX_NUM_REG];
static struct obj_info _gobj_info[MAX_NUM_OBJ];

static unsigned int _gnum_reg;
static unsigned int _gnum_obj;

static struct stream _gstream;

static unsigned char _gsub_2_to_4_map[4];
static unsigned char _gsub_2_to_8_map[4];
static unsigned char _gsub_4_to_8_map[16];
/* external global variable */


/* internal function */

static BOOL _sub_get_seg_hdr(struct seg_header *header)
{
    if(!stream_get_bits(&_gstream, &header->sync, 8))
        return FALSE;

    if(!stream_get_bits(&_gstream, &header->seg_type, 8))
        return FALSE;

    if(!stream_get_short(&_gstream, &header->page_id))
        return FALSE;

    if(!stream_get_short(&_gstream, &header->seg_len))
        return FALSE;

    return TRUE;
}

/*

return: total proccessed bytes
*/

static unsigned int _sub_subblock_dec(
    unsigned int top_len,
    unsigned int btm_len
)
{
    unsigned char data_type;
    unsigned int num_pix=0;
    unsigned int line_buf_size;
    BOOL ret=FALSE;
    struct buf_info buf;

    if(!stream_get_bits(&_gstream, &data_type, 8))
    {
        return FALSE;
    }

    /* allocate line buffer for RLD */
    line_buf_size = 0;
    buf.buf=(char *)malloc(line_buf_size);
    buf.size=(line_buf_size-num_pix);

    switch(data_type)
    {
        case 0x10:
            num_pix += sub_2bit_code_str_dec(&_gstream, &buf);
            break;
        case 0x11:
            num_pix += sub_4bit_code_str_dec(&_gstream, &buf);
            break;
        case 0x12:
            num_pix += sub_8bit_code_str_dec(&_gstream, &buf);
            break;
        case 0x20:
            break;
        case 0x21:
            break;
        case 0x22:
            break;
        case 0xF0: /* end of object line code */
            /* fill pixel line into each region which references current object */
            break;
        default:
            printf("unknown data type.\n");
    }

    stream_align_bytes(&_gstream, 1);

    return ret;
}

#define OBJ_CODE_METHOD_PIX (0x00)
#define OBJ_CODE_METHOD_CHR (0x01)

static BOOL _dvb_sub_page_dec(void)
{
    int idx;
    struct reg_lst *reg;

    if(!stream_get_bytes(&_gstream, (unsigned char *)&_gpage_info.timeout, 1))
    {
        return FALSE;
    }

    if(!stream_get_bits(&_gstream, (unsigned char *)&_gpage_info.version, 4))
    {
        return FALSE;
    }

    if(!stream_get_bits(&_gstream, (unsigned char *)&_gpage_info.state, 2))
    {
        return FALSE;
    }

    if(!stream_drop_bits(&_gstream, 2))
    {
        return FALSE;
    }

    _gpage_info.num_reg = ((_gseg_hdr.seg_len-2)/6); /* 6:number of bytes to region loop */

    if(_gseg_hdr.seg_len > (_gpage_info.num_reg * 6))
    {
        _DEBUG(printf("invalid segment length to page composition.\n"););
        return FALSE;
    }

    for(idx=0; idx<_gpage_info.num_reg; idx++)
    {
        reg = &_gpage_info.act_reg_lst[idx]; 
        if(!stream_get_bytes(&_gstream, &reg->reg_id, 1))
        {
            return FALSE;
        }

        if(!stream_drop_bits(&_gstream, 8))
        {
            return FALSE;
        }

        if(!stream_get_short(&_gstream, &reg->reg_h_addr))
        {
            return FALSE;
        }
        
        if(!stream_get_short(&_gstream, &reg->reg_v_addr))
        {
            return FALSE;
        }
    }

    return TRUE;
}

static BOOL _dvb_sub_reg_dec(void)
{

    return TRUE;
}

static BOOL _dvb_sub_clut_dec(void)
{

    return TRUE;
}

static BOOL _dvb_sub_obj_dec(void)
{
    unsigned char *data;
    unsigned short top_fld_data_len;
    unsigned short btm_fld_data_len;
    unsigned int proc_len=0;

    /* find which region that this object belongs to, and decode the data into the pixel buffer to that region */

    if(!stream_get_bytes(&_gstream, (unsigned char *)&_gobj_info[_gnum_obj].id, 2))
    {
        return FALSE;
    }

    if(!stream_get_bits(&_gstream, (unsigned char *)&_gobj_info[_gnum_obj].version, 4))
    {
        return FALSE;
    }

    if(!stream_get_bits(&_gstream, (unsigned char *)&_gobj_info[_gnum_obj].code_methd, 2))
    {
        return FALSE;
    }

    if(!stream_get_bits(&_gstream, (unsigned char *)&_gobj_info[_gnum_obj].code_methd, 1))
    {
        return FALSE;
    }

    if(!stream_drop_bits(&_gstream, 1))
    {
        return FALSE;
    }

    if(_gobj_info[_gnum_obj].code_methd == 0x00) /* coding of pixel */
    {
        if(!stream_get_bytes(&_gstream, (unsigned char *)&top_fld_data_len, 2))
        {
            return FALSE;
        }

        if(!stream_get_bytes(&_gstream, (unsigned char *)&btm_fld_data_len, 2))
        {
            return FALSE;
        }

        /* length integrity check */
        if(_gseg_hdr.seg_len < (top_fld_data_len + btm_fld_data_len + 7))
        {
            _DEBUG(printf("invalid object data segment length.\n"););
            return FALSE;
        }

        if(_sub_subblock_dec(top_fld_data_len, btm_fld_data_len))
        {
            printf("decode top field fail.\n");
            return FALSE;
        }

        stream_align_bytes(&_gstream, 2);
    }
    else /* coded as a string of characters */
    {
        /* not support yet */
        return FALSE;
    }
    
    _gnum_obj++;

    return TRUE;
}

static BOOL _dvb_sub_disp_def_dec(void)
{
    unsigned char disp_win_flag;
    unsigned short val;

    if(!stream_get_bits(&_gstream, &_gdisp_info.version, 4))
        return FALSE;

    if(!stream_get_bits(&_gstream, &disp_win_flag, 1))
        return FALSE;

    if(!stream_drop_bits(&_gstream, 3)) /* reserve */
        return FALSE;

    if(!stream_get_short(&_gstream, &_gdisp_info.win_width))
        return FALSE;

    if(!stream_get_short(&_gstream, &_gdisp_info.win_height))
        return FALSE;

    if(disp_win_flag)
    {
        if(!stream_get_short(&_gstream, &_gdisp_info.canv_x))
            return FALSE;

        if(!stream_get_short(&_gstream, &_gdisp_info.canv_width))
            return FALSE;

        if(!stream_get_short(&_gstream, &_gdisp_info.canv_y))
            return FALSE;

        if(!stream_get_short(&_gstream, &_gdisp_info.canv_height))
            return FALSE;

        _gdisp_info.canv_width -= _gdisp_info.canv_x;
        _gdisp_info.canv_height -= _gdisp_info.canv_y;
    }
    else
    {
        _gdisp_info.canv_x = 0;
        _gdisp_info.canv_y = 0;
        _gdisp_info.canv_width = _gdisp_info.win_width;
        _gdisp_info.canv_height = _gdisp_info.win_height;
    }

    return TRUE;
}

static BOOL _dvb_sub_disp_sig_dec(void)
{


    return TRUE;
}

static BOOL _dvb_sub_end_disp_set_dec(void)
{
    unsigned char *data;
    
    if(_gpf_draw_cb)
        _gpf_draw_cb(data, _gdisp_info.canv_color, _gdisp_info.canv_width, _gdisp_info.canv_height, _gdisp_info.canv_pitch);

    return TRUE;
}

/* decode segments */
static BOOL _dvb_sub_seg_dec(unsigned long long pts)
{
    unsigned char sync;
    BOOL ret;

    /* start segment loop */
    while(stream_peek_bits(&_gstream, &sync, 8))
    {
        if(sync != 0x0F)
        {
            if(sync != 0xFF) /* error condition */
            {
                _DEBUG(printf("segment sync byte error.\n"););
                return FALSE;
            }

            break;
        }

        if(!_sub_get_seg_hdr(&_gseg_hdr))
            break;

        /* decode segments */
        switch(_gseg_hdr.seg_type)
        {
            case SEG_PAGE_COMP: /* page composition segment */
                ret = _dvb_sub_page_dec();
                break;
            case SEG_REG_COMP:
                ret = _dvb_sub_reg_dec();
                break;
            case SEG_CLUT_DEF:
                ret = _dvb_sub_clut_dec();
                break;
            case SEG_OBJ_DATA:
                ret = _dvb_sub_obj_dec();
                break;
            case SEG_DISP_DEF:
                ret = _dvb_sub_disp_def_dec();
                break;
            case SEG_DISP_SIG:
                ret = _dvb_sub_disp_sig_dec();
                break;
            case SEG_END_DISP_SET:
                ret = _dvb_sub_end_disp_set_dec();
                break;
            case SEG_STUF:
                break;
            default:
                break;
        }

        if(ret == FALSE)
        {
            _DEBUG(printf("error occurred while decoding segment.\n"););
            return FALSE;
        }
    }

    return TRUE;
}

/* external function */

#ifndef SUB_DEC_PES_BUF_LEN
#define SUB_DEC_PES_BUF_LEN (65536)
#endif

unsigned char *_gpes_buf;

BOOL dvb_sub_deinit(void)
{
    do
    {
        /* clear up resources */
        if(_gpes_buf)
        {
            free(_gpes_buf);
            _gpes_buf = NULL;
        }
        return TRUE;
    }while(0);

    return FALSE;
}

BOOL dvb_sub_init(sub_draw_cb func)
{
    do
    {
        /* save callback interface */
        _gpf_draw_cb = func;

        /* allocate buffers */
        _gpes_buf = (unsigned char *)malloc(SUB_DEC_PES_BUF_LEN);
        if(_gpes_buf == NULL)
        {
            break;
        }
    
        return TRUE;
    }while(0);

    dvb_sub_deinit();
    return FALSE;
}

/*
 *  brief main decoder process of DVB Subtitle
 *  input:
 *      pes_buf: input buffer, contain PES data
 *      pes_len: input buffer length
 *  output:
 *      bufout: output buffer, corespond to a bitmap instance to a single PTS
 *      lenout: output buffer length
 *
 * */

int dvb_sub_dec(unsigned char *pes_buf, unsigned int pes_len)
{
    BOOL ret;
    unsigned char data_id;
    unsigned char sub_stream_id;
    unsigned char pes_hdr[4];
    unsigned long long pts;
    size_t size_to_cpy;

    /* decode PES */
    size_to_cpy = pes_len;
    memcpy((void *)_gpes_buf, (void *)pes_buf, size_to_cpy);

    /* stream function test */
    if(!pes_buf || !pes_len)
    {
        _DEBUG(printf("invalid input argument.\n"););
        return 0;
    }

    /* map PES data to stream object */
    if(!stream_create(&_gstream, STREAM_CREATE_SET, pes_buf, pes_len))
    {
        _DEBUG(printf("create stream fail.\n"););
        return 0;
    }

    /* check PES header */
    if(!stream_get_bytes(&_gstream, pes_hdr, 4))
    {
        _DEBUG(printf("get PES header fail.\n"););
        return 0;
    }

    if(!((pes_hdr[0] == 0x00) && (pes_hdr[1] == 0x00) && (pes_hdr[2] == 0x01) && (pes_hdr[3] == 0xBD)))
    {
        return 0;
    }

    /* get PTS */

    stream_get_bits(&_gstream, &data_id, 8);
    stream_get_bits(&_gstream, &sub_stream_id, 8);

    if(data_id != 0x20 || sub_stream_id != 0x00)
    {
        return 0;
    }

    /* start segment decode */
    ret = _dvb_sub_seg_dec(pts);

    /* return total processed data length */
    return stream_get_oft(&_gstream);
}


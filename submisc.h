
struct color
{
    union
    {
        struct
        {
            unsigned char a;
            unsigned char r;
            unsigned char g;
            unsigned char b;
        };

        struct
        {
            unsigned char y;
            unsigned char cb;
            unsigned char cr;
            unsigned char t;
        };
    };
};

unsigned int sub_2bit_code_str_dec(struct stream *strm, struct buf_info *buf);
unsigned int sub_4bit_code_str_dec(struct stream *strm, struct buf_info *buf);
unsigned int sub_8bit_code_str_dec(struct stream *strm, struct buf_info *buf);
struct color sub_YCbCrT_to_ARGB(struct color clr);



typedef enum
{
    STREAM_CREATE_COPY = 0,
    STREAM_CREATE_SET,

    STREAM_CREATE_ALL
}eSTREAM_CREATE_TYPE;

struct stream
{
    BOOL copy;
    unsigned int len;
    unsigned int oft;
    unsigned char bit_oft;
    unsigned char *buf;
};

void stream_reset(struct stream *strm);
void stream_destroy(struct stream *strm);
BOOL stream_create(struct stream *strm, eSTREAM_CREATE_TYPE type, unsigned char *buf, unsigned int len);
BOOL stream_get_bits(struct stream *strm, unsigned char *buf, unsigned char bit);
BOOL stream_get_bytes(struct stream *strm, unsigned char *buf, unsigned char byteA);
BOOL stream_get_short(struct stream *strm, unsigned short *buf);
BOOL stream_peek_bits(struct stream *strm, unsigned char *buf, unsigned char bit);
BOOL stream_drop_bits(struct stream *strm, unsigned char bit);
void stream_align_bytes(struct stream *strm, unsigned char byte);
int stream_get_oft(struct stream *strm);

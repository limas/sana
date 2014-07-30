#include <stdio.h>
#include <stdlib.h>

#include "subdec.h"

char _test_dummy(char *func_name, char *log)
{
    sprintf(func_name, "%s", __FUNCTION__);
    return 0;
}

char _test_stream_create(char *func_name, char *log)
{
    sprintf(func_name, "%s", __FUNCTION__);

    int i;
    unsigned char val;
    unsigned char data1[]={0x55, 0xAA};
    unsigned char len1[]={2, 4, 3, 1, 6};
    unsigned char res1[]={1, 5, 3, 0, 42};

    _stream_create(STREAM_CREATE_SET, data1, sizeof(data1));
    for(i=0; i<sizeof(len1); i++)
    {
        _stream_get_bits(&val, len1[i]);

        fprintf(log, "test %d: len=%d, expect=%d, result=%d\n", i, len1[i], res1[i], val);

        if(val != res1[i])
        {
            fprintf(log, "result error.\n");
            return -1;
        }
    }
    fprintf(log, "\n");
    
    unsigned char data2[]={0xA4, 0x3D, 0x22, 0x19};
    unsigned char len2[]={2, 6, 7, 8, 3, 2 ,1, 3};
    unsigned char res2[]={2, 36, 30, 145, 0, 1, 1, 1};

    _stream_create(STREAM_CREATE_SET, data2, sizeof(data2));
    for(i=0; i<sizeof(len2); i++)
    {
        _stream_get_bits(&val, len2[i]);

        fprintf(log, "test %d: len=%d, expect=%d, result=%d\n", i, len2[i], res2[i], val);

        if(val != res2[i])
        {
            fprintf(log, "result error.\n");
            return -1;
        }
    }
    fprintf(log, "\n");

    fprintf(log, "result correct.\n");

    return 0;
}

typedef char (*test_func)(char *func_name, char *log);

test_func _gpf_test_func_arr[]={
    _test_stream_create,


    _test_dummy
};

#define FUNC_NAME_LEN (512)
#define FUNC_LOG_LEN (2048)

int main(int argc, char **argv)
{
    char ret;
    char *func_name;
    char *func_log;
    int i;

    func_name = (char *)malloc(FUNC_NAME_LEN);
    func_log = (char *)malloc(FUNC_LOG_LEN);

    for(i=0; i<(sizeof(_gpf_test_func_arr)/sizeof(_gpf_test_func_arr[0]); i++))
    {
        ret = _gpf_test_func_arr[i](func_name, func_log);
        if(ret != 0)
        {
            printf("function [%s] log:\n%s\n", func_name, func_log);
            return -1;
        }
        else
        {
            printf("function [%s] test ok.\n", func_name);
        }
    }

    return 0;
}

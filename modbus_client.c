#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include "hash.h"
#include "converter.h"

#define DEFAULT_BACKLOG 128


const static unsigned char b1=0b00000001;
const static unsigned char b2=0b00000010;
const static unsigned char b3=0b00000100;
const static unsigned char b4=0b00001000;
const static unsigned char b5=0b00010000;
const static unsigned char b6=0b00100000;
const static unsigned char b7=0b01000000;
const static unsigned char b8=0b10000000;        

static char byteboolean[8]={b1,b2,b3,b4,b5,b6,b7,b8};


uv_tcp_t client;
uv_connect_t connect_req;
uv_loop_t *loop;
struct sockaddr_in addr;
uv_fs_t stdin_watcher;
char buffer[1024];

unsigned short data_num =0x01;

char *arg[10];

//k-���кţ�v-��ʼ��ַ
HashMap *map;
//k-���кţ�v-��ѯ����
HashMap *map1;

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

void free_write_req(uv_write_t *req) {
    write_req_t *wr = (write_req_t*) req;
    free(wr->buf.base); 
    free(wr);
}

//��ӡ����
void print_buffer(unsigned char *buf,ssize_t len){
    for(int i=0;i<len;i++){
        // if( *(buf+i) <16 ){
        //     printf("0%x ",buf[i]);
        // }else{
        //     printf("%x ",buf[i]);
        // }
        printf("%02x ",buf[i]);
    }
    printf("\n");
}

void on_write(uv_write_t *req, int status) {
    if (status) {
        fprintf(stderr, "Write error %s\n", uv_strerror(status));
    }
    printf("send areadly\n");
    free_write_req(req);
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    *buf = uv_buf_init((char*) malloc(suggested_size), suggested_size);
}

void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    if (nread < 0) {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t*) stream, NULL);
        free(buf->base);
        return;
    }

    // �����ﴦ����յ�������...
    printf("Received:\n");
    print_buffer((unsigned char*)(buf->base),nread);

    //��У�鳤��
    unsigned short len;
    len=read_uint16_big_endian(buf->base+4);
    //printf("len=%d\n",len);
    if(len != nread-6){
        printf("���α��ĳ��ȼ������");
        return;
    }

    //��ȡ������
    unsigned char work_code;
    work_code=*(buf->base+7);
    //���ݵĳ���
    unsigned char data_code;
    data_code=*(buf->base+8);
    //�������
    unsigned short num;
    num=read_uint16_big_endian(buf->base+0);
    //printf("num=%d\n",num);
    unsigned short status;
    unsigned short adress;
    adress=hashmap_get(map,num,&status);
    if( status != 0){
        hashmap_remove(map,num);
        unsigned short piont_num;
        piont_num=hashmap_get(map1,num,&status);
        //printf("adress=%d\n",adress);
        //printf("piont_num=%d\n",piont_num);
        hashmap_remove(map1,num);
        //printf("work_code=%d\n",work_code);
        if(work_code==0x01){
            printf("���չ��������ͣ��������Ȧ\n");
            int a;
            for(int i=0;i<piont_num;i++){
                unsigned char b= (*(buf->base+i/8+9) & byteboolean[i%8] );
                if(b==byteboolean[i%8])
                        a=1;
                    else
                        a=0;
                printf("��ַ��%d                 ��ֵ��%d \n",adress+i,a);
            }
        }else if (work_code==0x02)
        {
            printf("���չ��������ͣ���������Ȧ\n");
            int a;
            for(int i=0;i<piont_num;i++){
                unsigned char b= (*(buf->base+i/8+9) & byteboolean[i%8] );
                if(b==byteboolean[i%8])
                        a=1;
                    else
                        a=0;
                printf("��ַ��%d                 ��ֵ��%d \n",adress+i,a);
            }
        }else if (work_code==0x03){
            printf("���չ��������ͣ�������Ĵ���\n");
            for(int i=0;i<piont_num;i++){
                printf("��ַ��%d                 ��ֵ��%d \n",adress+i,read_uint16_big_endian(buf->base+9+i*2));
            }
        }else if (work_code==0x04){
            printf("���չ��������ͣ�������Ĵ���\n");
            for(int i=0;i<piont_num;i++){
                printf("��ַ��%d                 ��ֵ��%d \n",adress+i,read_uint16_big_endian(buf->base+9+i*2));
            }
        }else if(work_code==0x05){
            printf("���չ��������ͣ�д���������Ȧ\n");
        }else if(work_code==0x06){
            printf("���չ��������ͣ�д��������Ĵ���\n");
        }else if(work_code==0x0f){
            printf("���չ��������ͣ�д��������Ȧ\n");
        }else if(work_code==0x10){
            printf("���չ��������ͣ�д�������Ĵ���\n");
        }else{
            printf("�������������\n");
        }
        
    }else{
        fprintf(stderr,"�������Ĵ���:map��δ�����num��");
    }

    free(buf->base);
}

void on_type(uv_fs_t *req) {
    if (stdin_watcher.result > 0) {
        buffer[stdin_watcher.result] = '\0';
        //printf("Typed %s\n", buffer);
        splitString(buffer,",",arg);
        // for(int i=0;arg[i]!=NULL;i++){
        //     printf("%s\n",arg[i]);
        // }
        write_req_t *req1 = (write_req_t*) malloc(sizeof(write_req_t));
        char *buffer_data=(char *) malloc(sizeof(char)*256);
	    memset(buffer_data,0,256);

        write_uint16_big_endian(buffer_data,data_num);
        buffer_data[2]=0x00;
        buffer_data[3]=0x00;
        buffer_data[4]=0x00;
        buffer_data[5]=0x06;
        buffer_data[6]=0x01;
        hashmap_put(map,data_num,stringToShort(arg[2]));
        hashmap_put(map1,data_num,stringToShort(arg[3]));
        data_num++;
        int len=12;
        char *code=arg[1];
        short valve[255];
        //printf("code=%s\n",code);
        if(strcmp(arg[1],"0x01")==0){
            buffer_data[7]=0x01;
            write_uint16_big_endian(buffer_data+8,stringToShort(arg[2]));
            write_uint16_big_endian(buffer_data+10,stringToShort(arg[3]));
        }else if(strcmp(arg[1],"0x02")==0){
            buffer_data[7]=0x02;
            write_uint16_big_endian(buffer_data+8,stringToShort(arg[2]));
            write_uint16_big_endian(buffer_data+10,stringToShort(arg[3]));
        }else if(strcmp(arg[1],"0x03")==0){
            buffer_data[7]=0x03;
            write_uint16_big_endian(buffer_data+8,stringToShort(arg[2]));
            write_uint16_big_endian(buffer_data+10,stringToShort(arg[3]));
        }else if(strcmp(arg[1],"0x04")==0){
            buffer_data[7]=0x04;
            write_uint16_big_endian(buffer_data+8,stringToShort(arg[2]));
            write_uint16_big_endian(buffer_data+10,stringToShort(arg[3]));
        }else if(strcmp(arg[1],"0x05")==0){
            buffer_data[7]=0x05;
            write_uint16_big_endian(buffer_data+8,stringToShort(arg[2]));
            string_to_short_arr(arg[4],valve);
            if(valve[0]==1){
                buffer_data[10]=0xff;
                buffer_data[11]=0x00;
            }else{
                buffer_data[10]=0x00;
                buffer_data[11]=0x00;
            }
        }else if(strcmp(arg[1],"0x06")==0){
            buffer_data[7]=0x06;
            int arr_len;
            arr_len=string_to_short_arr(arg[4],valve);
            write_uint16_big_endian(buffer_data+8,stringToShort(arg[2]));
            write_uint16_big_endian(buffer_data+10,valve[0]);
        }else if(strcmp(arg[1],"0x0F")==0){
            buffer_data[7]=0x0F;
            int arr_len;
            arr_len=string_to_short_arr(arg[4],valve);
            //printf("arr_len=%d\n",arr_len);
            if(arr_len!=stringToShort(arg[3])){
                for(int i=0;i<10;i++){
                    printf("i=%d\n",valve[i]);
                }
                fprintf(stderr,"������޸��������ͣ������ݵĸ�����һ��");
                return;
            }
            // for(int i=0;i<arr_len;i++){
            //         printf("i=%d\n",valve[i]);
            // }
            write_uint16_big_endian(buffer_data+8,stringToShort(arg[2]));
            write_uint16_big_endian(buffer_data+10,stringToShort(arg[3]));
            unsigned char bytes=(unsigned char)(arr_len/8);
            if(arr_len%8>0){
                bytes++;
            }
            //printf("bytes=%d\n",bytes);
            buffer_data[12]=bytes;
            int temp=0;
            unsigned char bb=0;
            for (byte i = 0; i < bytes ; i++) {
                //��ÿһ��byte��ÿһ��bit����
                for (int j = 0; j < 8; j++) {
                    if(temp< arr_len){
                        if(valve[temp]==1){
                            bb=bb|byteboolean[j];
                        }
                        temp++;
                    }
                }
                buffer_data[++len]=bb;
                //ÿһ��byte�������˺�����
                bb=0b00000000;
            }
            write_uint16_big_endian(buffer_data+4,bytes+7);
            len++;
            //printf("len=%d\n",len);
        }else if(strcmp(arg[1],"0x10")==0){
            buffer_data[7]=0x10;
            write_uint16_big_endian(buffer_data+8,stringToShort(arg[2]));
            write_uint16_big_endian(buffer_data+10,stringToShort(arg[3]));
            int arr_len;
            arr_len=string_to_short_arr(arg[4],valve);
            if(arr_len!=stringToShort(arg[3])){
                fprintf(stderr,"������޸��������ͣ������ݵĸ�����һ��");
                return;
            }
            buffer_data[12]=2*arr_len;
            len++;//len=13
            for(int i=0;i<arr_len;i++){
                write_uint16_big_endian(buffer_data+len+2*i,valve[i]);
            }
            len=len+2*arr_len;
            write_uint16_big_endian(buffer_data+4,2*arr_len+7);
        }else{
            printf("�����������\n");
            return;
        }

        // ����һ���µĻ��������洢Ҫд�������
        // char *data = (char*)malloc(stdin_watcher.result);
        // if (data == NULL) {
        //     // �����ڴ����ʧ�ܵ����
        //     uv_close((uv_handle_t*)client, on_close);
        //     return;
        // }
        // memcpy(data, &buffer, stdin_watcher.result);

        printf("send data:\n");
        print_buffer(buffer_data,len);


        req1->buf = uv_buf_init(buffer_data, len);
        uv_write((uv_write_t*)req1, (uv_stream_t*)&client,&req1->buf, 1, on_write);


        //uv_fs_write(uv_default_loop(), &write_t, 1, &buf, 1, -1, on_write);

        
        uv_buf_t buf1 = uv_buf_init(buffer, 1024);
        uv_fs_read(loop, &stdin_watcher, 0, &buf1, 1, -1, on_type);
    }
    else if (stdin_watcher.result < 0) {
        fprintf(stderr, "error opening file: %s\n", uv_strerror(req->result));
    }
    
}


void on_connect(uv_connect_t *req, int status) {
    if (status < 0) {
        fprintf(stderr, "Connection error %s\n", uv_strerror(status));
        return;
    }

    uv_read_start((uv_stream_t*) &client, alloc_buffer, on_read);

    // �������ݵ�������
    //����ֱ����memcpy��Ϊ������С�ˣ�ֱ����Ҳ��С��д��ȥ��modbus TCP/IP�õ��Ǵ��
    //memcpy(buffer+0,&data_num, 2);
    // buffer[0]=0x00;
    // buffer[1]=0x01;
    write_uint16_big_endian(buffer,data_num);
    buffer[2]=0x00;
    buffer[3]=0x00;
    buffer[4]=0x00;
    buffer[5]=0x06;
    buffer[6]=0x01;
    buffer[7]=0x03;
    buffer[8]=0x00;
    buffer[9]=0x00;
    buffer[10]=0x00;
    buffer[11]=0x0a;

    hashmap_put(map,data_num,0);
    hashmap_put(map1,data_num,10);
    data_num++;
    uv_write_t write_req;
    uv_buf_t send_buf = uv_buf_init(buffer, 12);
    uv_write(&write_req, (uv_stream_t*)&client, &send_buf, 1, NULL);

    fprintf(stdout,"0x01������Ȧ\n0x05��д������Ȧ\n0x0F��д�����Ȧ\n0x02������ɢ������\n0x04��������Ĵ���\n0x03�������ּĴ���\n0x06��д�������ּĴ���\n0x10��д������ּĴ���\n");
    fprintf(stdout,"�����ʽ��[1/2](ѡ���ʹ�����1���Σ�2ѭ��),[������],[��ʼ��ַ],[��ѯ/д�������],[д�����ݣ�/������]\n");
}

int main() {
    loop = uv_default_loop();

    map = hashmap_create();
    map1 = hashmap_create();

    uv_tcp_init(loop, &client);

    struct sockaddr_in server_addr;
    uv_ip4_addr("127.0.0.1", 502, &server_addr); // ʹ����ķ�������ַ�Ͷ˿�

    uv_tcp_connect(&connect_req, &client, (const struct sockaddr*)&server_addr, on_connect);

    
    uv_buf_t buf = uv_buf_init(buffer, 1024);
    uv_fs_read(loop, &stdin_watcher, 0, &buf, 1, -1, on_type);

    uv_run(loop, UV_RUN_DEFAULT);
    hashmap_free(map);
    hashmap_free(map1);
    //uv_fs_req_cleanup(&stdin_watcher);
    return 0;
}
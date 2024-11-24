#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char buffer[1024];


void print_buffer(char *buf,int len){
    for(int i=0;i<len;i++){
        if( *(buf+i) <16 ){
            printf("0%x ",buf[i]);
        }else{
            printf("%x ",buf[i]);
        }
    }
}

int main(){
    buffer[0]=0x00;
    buffer[1]=0x01;
    buffer[2]=0x00;
    buffer[3]=0x00;
    buffer[4]=0x00;
    buffer[5]=0x06;
    buffer[6]=0x01;
    buffer[7]=0x01;
    buffer[8]=0x00;
    buffer[9]=0x02;
    buffer[10]=0x00;
    buffer[11]=0x04;

    print_buffer(buffer,12);
    return 0;
}


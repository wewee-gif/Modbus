#include <stdio.h>

int check_endian() {
    unsigned int x = 0x12345678;
    unsigned char *c = (unsigned char *)&x;
    printf("%x\n",*c);
    return (*c == 0x78); // 如果最低地址存储的是LSB，即0x78，则为小端；否则为大端
}

int main() {
    if (check_endian()) {
        printf("Little-Endian\n");
    } else {
        printf("Big-Endian\n");
    }
    return 0;
}
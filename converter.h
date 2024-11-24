
#ifndef CONVERTER_H
#define CONVERTER_H


void write_uint16_big_endian(unsigned char *buffer,unsigned short value);
short read_uint16_big_endian(unsigned char *buffer) ;
void trim_whitespace_simple(char *str);
void splitString(char *str, const char *delimiter,char **arg) ;
short stringToShort(const char *str);
int string_to_short_arr(char *str,short *arr);
#endif
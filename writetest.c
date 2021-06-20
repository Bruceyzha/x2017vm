#include <stdio.h>

int main(){
    unsigned char buffer[5] = {0x00,0x00,0x80,0x20,0x42};
    FILE *fp = fopen("wrong_type.x2017","wb");
    fwrite(buffer,5,1,fp);
    fclose(fp);
    fp = fopen("wrong_type.x2017","rb");
    unsigned char test_buffer[6];
    fread(test_buffer,6,1,fp);
    for(int i = 0; i < 6;i++){
        printf("0x%x\n",test_buffer[i]);
    }
    fclose(fp);
}
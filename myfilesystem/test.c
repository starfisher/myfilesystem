    #include<stdio.h>
    #include<string.h>
    #include<stdlib.h>
    int main(void)
    {
        int ch;
        int len;
        int i=0;
        FILE* fstream;
        /*w+，打开可读写文件，若文件存在则文件长度清为零，
	  即该文件内容会消失。若文件不存在则建立该文件*/
        fstream=fopen("testputw.txt","w+");
        if(fstream==NULL)
        {
            printf("read file test.txt failed!\n");
            exit(1);
        }
       
        len = 56;
        putw(len,fstream);
        putw(57,fstream);
        if(ferror(fstream))
        {
            printf("Error writing to file.\n");
        }
        else
        {
            printf("Success writing to file.\n");
        }
        fclose(fstream);
        fstream=fopen("testputw.txt","r");
	printf("%d\n", getw(fstream));
	fseek(fstream, -4, SEEK_CUR);
	printf("%d\n", getw(fstream));
       
        fclose(fstream);
        return 0;
    }

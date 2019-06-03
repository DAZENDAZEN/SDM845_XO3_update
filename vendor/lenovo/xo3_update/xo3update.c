#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BOX_XO3 "/system/bin/updateNEW/BOX_XO3.hex"

#define XO3R_OFFLINE "/sys/fpga_xo3r/xo3r_offline"
#define XO3R_RESETCFG "/sys/fpga_xo3r/xo3r_resetcfg"
#define XO3R_ERASE "/sys/fpga_xo3r/xo3r_erase"
#define XO3R_PROGRAM "/sys/fpga_xo3r/xo3r_program"
#define XO3R_PROGRAMDONE "/sys/fpga_xo3r/xo3r_programdone"
#define XO3R_PROGRAMEXIT "/sys/fpga_xo3r/xo3r_programexit"
#define XO3R_ISSUEREFRESH "/sys/fpga_xo3r/xo3r_IssueRefresh"

#define XO3L_OFFLINE "/sys/fpga_xo3l/xo3l_offline"
#define XO3L_RESETCFG "/sys/fpga_xo3l/xo3l_resetcfg"
#define XO3L_ERASE "/sys/fpga_xo3l/xo3l_erase"
#define XO3L_PROGRAM "/sys/fpga_xo3l/xo3l_program"
#define XO3L_PROGRAMDONE "/sys/fpga_xo3l/xo3l_programdone"
#define XO3L_PROGRAMEXIT "/sys/fpga_xo3l/xo3l_programexit"
#define XO3L_ISSUEREFRESH "/sys/fpga_xo3l/xo3l_IssueRefresh"

int HexToDec(char *src);
void TranHexR(char *formatcmd);
void TranHexL(char *formatcmd);
char *ExtraSubStr(const char *src,int startIndex,int len);
int endr = 0;
int page_numr = 0;
int endl = 0;
int page_numl = 0;

int main()
{
	FILE *fp = NULL;
	FILE *xo3r_offline_fd = NULL;
	FILE *xo3r_erase_fd = NULL;
	FILE *xo3r_resetcfg_fd = NULL;
	FILE *xo3r_programdone_fd = NULL;
	FILE *xo3r_programexit_fd = NULL;
	FILE *xo3r_IssuRefresh_fd = NULL;
	
	FILE *xo3l_offline_fd = NULL;
	FILE *xo3l_erase_fd = NULL;
	FILE *xo3l_resetcfg_fd = NULL;
	FILE *xo3l_programdone_fd = NULL;
	FILE *xo3l_programexit_fd = NULL;
	FILE *xo3l_IssuRefresh_fd = NULL;

	 char *buffer=malloc(sizeof(char)*100);

	if(buffer==NULL){
	printf("Apply for memory failed.!\n");
         return -1;
	}

	fp=fopen(BOX_XO3,"r"); 
	if(fp==NULL){
	printf("Open file error.\n");
	return -1;
	}

 	xo3r_offline_fd = fopen(XO3R_OFFLINE, "w");
	if(NULL == xo3r_offline_fd){
		 printf("xo3r_offline_fd open file fail: %s", XO3R_OFFLINE);
		
		return -1;
	}
	fprintf(xo3r_offline_fd,"%d", 1);
	 printf("xo3r_offline_fd ok\n");
	fclose(xo3r_offline_fd);
	usleep(10*1000*1000);//usleep(5);
	
 	xo3r_erase_fd = fopen(XO3R_ERASE, "w");
	if(NULL == xo3r_erase_fd){
		printf("xo3r_erase_fd open file fail: %s", XO3R_ERASE);
		
		return -1;
	}
	fprintf(xo3r_erase_fd,"%d", 1);
	printf("xo3r_erase_fd ok\n");
	fclose(xo3r_erase_fd);
	usleep(15*1000*1000);
	
 	xo3r_resetcfg_fd = fopen(XO3R_RESETCFG, "w");
	if(NULL == xo3r_resetcfg_fd){
		printf("xo3r_resetcfg_fd open file fail: %s", XO3R_RESETCFG);
		
		return -1;
	}
	fprintf(xo3r_resetcfg_fd,"%d", 1);
	printf("xo3r_resetcfg_fd ok\n");
	fclose(xo3r_resetcfg_fd);
	usleep(10*1000*1000);

	while((fscanf(fp,"%s",buffer)!=EOF)&&endr==0){
		printf("%s\n",buffer);
		TranHexR(buffer);
		usleep(10000);
	}
	printf("xo3r_program_fd ok\n");
	usleep(1000);
	
	 xo3r_programdone_fd = fopen(XO3R_PROGRAMDONE, "w");
	if(NULL == xo3r_programdone_fd){
		printf("xo3r_programdone_fd open file fail: %s", XO3R_PROGRAMDONE);
		
		return -1;
	}
	fprintf(xo3r_programdone_fd,"%d", 1);
	printf("xo3r_programdone_fd ok\n");
	fclose(xo3r_programdone_fd);
	usleep(1*1000*1000);
	
	 xo3r_programexit_fd = fopen(XO3R_PROGRAMEXIT, "w");
	if(NULL == xo3r_programexit_fd){
		printf("xo3r_programexit_fd open file fail: %s", XO3R_PROGRAMEXIT);
		
		return -1;
	}
	fprintf(xo3r_programexit_fd,"%d", 1);
	printf("xo3r_programexit_fd ok\n");
	fclose(xo3r_programexit_fd);
	usleep(1*1000*1000);
/*	
	xo3r_IssuRefresh_fd = fopen(XO3R_ISSUEREFRESH, "w");
	if(NULL == xo3r_IssuRefresh_fd){
		printf("xo3r_IssuRefresh_fd open file fail: %s", XO3R_ISSUEREFRESH);
		
		return -1;
	}
	fprintf(xo3r_IssuRefresh_fd,"%d", 1);
	printf("xo3r_IssuRefresh_fd ok\n");
	fclose(xo3r_IssuRefresh_fd);
	usleep(5*1000);*/
	printf("ZD xo3r_update finished");

	free(buffer);
	fclose(fp);
/*****************xo3l update*************************/

	char *buffer1=malloc(sizeof(char)*100);
	
	if(buffer1==NULL){
	   printf("Apply for memory failed.!\n");
			return -1;
	}
	
	 fp=fopen(BOX_XO3,"r"); 
	   if(fp==NULL){
	   printf("Open file error.\n");
	   return -1;
	}
	
 	xo3l_offline_fd = fopen(XO3L_OFFLINE, "w");
	if(NULL == xo3l_offline_fd){
		 printf("xo3l_offline_fd open file fail: %s", XO3L_OFFLINE);
		
		return -1;
	}
	fprintf(xo3l_offline_fd,"%d", 1);
	printf("xo3l_offline_fd ok\n");	
	fclose(xo3l_offline_fd);
	usleep(10*1000*1000);
	
 	xo3l_erase_fd = fopen(XO3L_ERASE, "w");
	if(NULL == xo3l_erase_fd){
		printf("xo3l_erase_fd open file fail: %s", XO3L_ERASE);
		
		return -1;
	}
	fprintf(xo3l_erase_fd,"%d", 1);
	printf("xo3l_erase_fd ok\n");
	fclose(xo3l_erase_fd);
	usleep(15*1000*1000);
	
 	xo3l_resetcfg_fd = fopen(XO3L_RESETCFG, "w");
	if(NULL == xo3l_resetcfg_fd){
		printf("xo3l_resetcfg_fd open file fail: %s", XO3L_RESETCFG);
		
		return -1;
	}
	fprintf(xo3l_resetcfg_fd,"%d", 1);
	printf("xo3l_resetcfg_fd ok\n");
	fclose(xo3l_resetcfg_fd);
	usleep(10*1000*1000);
	
 
	while((fscanf(fp,"%s",buffer1)!=EOF)&&endl==0){
		printf("%s\n",buffer1);
		TranHexL(buffer1);
		usleep(10000);
	}
	printf("xo3l_program_fd ok\n");
	usleep(1000);
	
	 xo3l_programdone_fd = fopen(XO3L_PROGRAMDONE, "w");
	if(NULL == xo3l_programdone_fd){
		printf("xo3l_programdone_fd open file fail: %s", XO3L_PROGRAMDONE);
		
		return -1;
	}
	fprintf(xo3l_programdone_fd,"%d", 1);
	printf("xo3l_programdone_fd ok\n");
	fclose(xo3l_programdone_fd);
	usleep(1*1000*1000);
	
	 xo3l_programexit_fd = fopen(XO3L_PROGRAMEXIT, "w");
	if(NULL == xo3l_programexit_fd){
		printf("xo3l_programexit_fd open file fail: %s", XO3L_PROGRAMEXIT);
		
		return -1;
	}
	fprintf(xo3l_programexit_fd,"%d", 1);
	printf("xo3l_programexit_fd ok\n");
	fclose(xo3l_programexit_fd);
	usleep(1*1000*1000);
	/*
	xo3l_IssuRefresh_fd = fopen(XO3L_ISSUEREFRESH, "w");
	if(NULL == xo3l_IssuRefresh_fd){
		printf("xo3l_IssuRefresh_fd open file fail: %s", XO3L_ISSUEREFRESH);
		
		return -1;
	}
	fprintf(xo3l_IssuRefresh_fd,"%d", 1);
	printf("xo3l_IssuRefresh_fd ok\n");
		fclose(xo3l_IssuRefresh_fd);
	usleep(5*1000);*/
	printf("ZD xo3l_update finished");
	free(buffer1);
	fclose(fp);

 	return 0;
}
 
void TranHexR(char *formatcmd){

	FILE *xo3r_program_fd = NULL;
	char *eofstring="FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";//end fllag
	int values[16];
 	int p;
	int q = 0;
	int size = 0;

	xo3r_program_fd = fopen(XO3R_PROGRAM, "w");
		if(NULL == xo3r_program_fd){
		printf("xo3r_program_fd open file fail: %s", XO3R_PROGRAM);
		
		//return -1;
	}
	if(strcmp(formatcmd,eofstring)==0){
	endr = 1;
	page_numr = 0;
	printf("End of file. page_numr:%d\n",page_numr);
	return;
	}

	for(p=0;p<16;p++){
		values[p]=HexToDec(ExtraSubStr(formatcmd,q,2));
		q+=2;
		//printf("value[%d]:%d\n",p,values[p]);	
	}
	
	page_numr++;	
	printf("ZD page_numr:%d\n",page_numr);
	
	size = fprintf(xo3r_program_fd,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", 
	 	values[0],values[1],values[2],values[3],values[4],values[5],values[6],values[7],
	 	values[8],values[9],values[10],values[11],values[12],values[13],values[14],values[15],page_numr);
	if(size < 0){
		printf("ZD fprint false\n");
	}else{
		printf("ZD fprint size:%d\n",size);
	}
	fclose(xo3r_program_fd);

}
 
 void TranHexL(char *formatcmd){

	FILE *xo3l_program_fd = NULL;
	char *eofstring="FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF";//end fllag
	int values[16];
	int p;
	int q = 0;
	int size = 0;

	xo3l_program_fd = fopen(XO3L_PROGRAM, "w");
		if(NULL == xo3l_program_fd){
		printf("xo3l_program_fd open file fail: %s", XO3L_PROGRAM);
		
		//return -1;
	}
	if(strcmp(formatcmd,eofstring)==0){
	endl = 1;
	page_numl = 0;
	printf("End of file. page_numl:%d\n",page_numl);
	return;
	}

	for(p=0;p<16;p++){
		values[p]=HexToDec(ExtraSubStr(formatcmd,q,2));
		q+=2;
		//printf("value[%d]:%d\n",p,values[p]);	
	}
	
	page_numl++;	
	printf("ZD page_numl:%d\n",page_numl);
	
	size = fprintf(xo3l_program_fd,"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", 
	 	values[0],values[1],values[2],values[3],values[4],values[5],values[6],values[7],
	 	values[8],values[9],values[10],values[11],values[12],values[13],values[14],values[15],page_numl);
	if(size < 0){
		printf("ZD fprint false\n");
	}else{
		printf("ZD fprint size:%d\n",size);
	}
	fclose(xo3l_program_fd);

}
 
int HexToDec(char *src){
    
	int value=0,srclen=strlen(src);
	int i;
	for(i=0;i<srclen;i++){
			if(src[i]<='9'&&src[i]>='0'){
			 value=value*16+(int)(src[i]-'0');
	        }else if(src[i]<='f'&&src[i]>='a'){
			value=value*16+(int)(src[i]-'a'+10);
	        }else{
			value=value*16+(int)(src[i]-'A'+10);
		}
	}
    return value;
}
 
char *ExtraSubStr(const char *src,int startIndex,int len){
   
	char *substr=malloc(sizeof(char)*(len+1));
	int i=0;
	if(substr==NULL){
		printf("Apply for memory failed.\n");
	return NULL;
	}

	if(strlen(src)<startIndex+len){
	printf("Index exceeds bounds.\n");
	return NULL;
	}
	for(i=0;i<len;i++){
		substr[i]=src[startIndex+i];
	}
	substr[len]='\0';
	return substr;
}
 

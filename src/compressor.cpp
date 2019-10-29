#include "compressor.h"
double average = 0;
double average_sub = 0;
double average_orig = 0;
double flip_average = 0;
double flip_average_sub = 0;
double flip_average_orig = 0;
int total = 0;
int max_size = 32768 * 64;
int compress_size = 0;
int compress_size_sub = 0;
int compress_size_orig = 0;
int flip_compress_size = 0;
int flip_compress_size_sub = 0;
int flip_compress_size_orig = 0;
int uncompress_size = 0;
FILE *f = fopen("file.txt", "w");
FILE *f1 = fopen("fileRaw.txt", "w");


void printCache(){

}

double calcRate(int alloc, int comp, int flip){
    double ret = (static_cast<double>(comp)/static_cast<double>(alloc));
    average = (average * total + ret)/static_cast<double>(total + 1);
printf("total: %d, rate: %d / %d = %.1f, average: %.1f\n", total, comp, alloc, ret, average);
    flip_average = (flip_average * total + flip)/static_cast<double>(total + 1);
    total ++; 
	//printf("%d / %d\n", comp, alloc);
    fprintf(f, "%1f, %1f, ", flip_average, average);
    return ret;
}
double calcRate_sub(int alloc, int comp, int flip){
    double ret = (static_cast<double>(comp)/static_cast<double>(alloc));
    average_sub = (average_sub * (total-1) + ret)/static_cast<double>(total);
    flip_average_sub = (flip_average_sub * (total-1) + flip)/static_cast<double>(total);
	//printf("%d / %d\n", comp, alloc);
    fprintf(f,"%1f, %1f, ", flip_average_sub, average_sub);
    return ret;
}
double calcRate_orig(int alloc, int comp, int flip){
    double ret = (static_cast<double>(comp)/static_cast<double>(alloc));
    average_orig = (average_orig * (total-1) + ret)/static_cast<double>(total);
    flip_average_orig = (flip_average_orig * (total-1) + flip)/static_cast<double>(total);
	//printf("%d / %d\n", comp, alloc);
    fprintf(f, "%1f, %1f\n", flip_average_orig, average_orig);
    return ret;
}

double calcRate_raw(int alloc, int comp, int flip){
    double ret = (static_cast<double>(comp)/static_cast<double>(alloc));
    average = (average * total + ret)/static_cast<double>(total + 1);
printf("total: %d, rate: %d / %d = %.1f, average: %.1f\n", total, comp, alloc, ret, average);
    flip_average = (flip_average * total + flip)/static_cast<double>(total + 1);
    total ++; 
	//printf("%d / %d\n", comp, alloc);
    fprintf(f1, "%1f, %1f, ", flip_average, average);
    return ret;
}
double calcRate_sub_raw(int alloc, int comp, int flip){
    double ret = (static_cast<double>(comp)/static_cast<double>(alloc));
    average_sub = (average_sub * (total-1) + ret)/static_cast<double>(total);
    flip_average_sub = (flip_average_sub * (total-1) + flip)/static_cast<double>(total);
	//printf("%d / %d\n", comp, alloc);
    fprintf(f1,"%1f, %1f, ", flip_average_sub, average_sub);
    return ret;
}
double calcRate_orig_raw(int alloc, int comp, int flip){
    double ret = (static_cast<double>(comp)/static_cast<double>(alloc));
    average_orig = (average_orig * (total-1) + ret)/static_cast<double>(total);
    flip_average_orig = (flip_average_orig * (total-1) + flip)/static_cast<double>(total);
	//printf("%d / %d\n", comp, alloc);
    fprintf(f1, "%1f, %1f\n", flip_average_orig, average_orig);
    return ret;
}
Line compress(Line line){
    Line num = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    Line first = {line.Byte[3],line.Byte[2],line.Byte[1],line.Byte[0],0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    for(int i = 1 ; i < 16; i++){
        num =  (Line){(line.Byte[i*4]),(line.Byte[i*4 + 1]),(line.Byte[i*4 + 2]),(line.Byte[i*4 + 3])};
        Line s = sub(num , first);
        int ss = countWidth(s.Byte[0]) + countWidth(s.Byte[1]) + countWidth(s.Byte[2]) + countWidth(s.Byte[3]);
        int sn = countWidth(num.Byte[0]) + countWidth(num.Byte[1]) + countWidth(num.Byte[2]) + countWidth(num.Byte[3]);
        if(ss > sn){
            first.Byte[i*4] = num.Byte[0];
            first.Byte[i*4 + 1] = num.Byte[1];
            first.Byte[i*4 + 2] = num.Byte[2];
            first.Byte[i*4 + 3] = num.Byte[3];
        }
        else{
            first.Byte[i*4] = s.Byte[0];
            first.Byte[i*4 + 1] = s.Byte[1];
            first.Byte[i*4 + 2] = s.Byte[2];
            first.Byte[i*4 + 3] = s.Byte[3];
        }
    }
	return first;
}



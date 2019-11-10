#include "compressor.h"

int orig_size;
int xor_size;
int xor_size_perf;
int orig_size_bdi;
int xor_size_bdi;
int xor_size_bdi_perf;
int flip_orig_size;
int flip_xor_size;
int flip_xor_size_perf;
int flip_orig_size_bdi;
int flip_xor_size_bdi;
int flip_xor_size_bdi_perf;
FILE *f = fopen("file.txt", "w");
FILE *f1 = fopen("fileRaw.txt", "w");


void printCache(){

}

double calcRate(int alloc, int comp, int orig_flip, int flip){
    orig_size_bdi += alloc;
    xor_size_bdi += comp;
    flip_orig_size_bdi += orig_flip; 
    flip_xor_size_bdi += flip;
	//printf("%d / %d\n", comp, alloc);
    fprintf(f, "%1f, %1f\n", (static_cast<double>(flip_orig_size_bdi)/static_cast<double>(flip_xor_size_bdi)), (static_cast<double>(orig_size_bdi)/static_cast<double>(xor_size_bdi)));
    return 0;
}
double calcRatePerf(int alloc, int comp, int orig_flip, int flip){
    xor_size_bdi_perf += comp;
    flip_xor_size_bdi_perf += flip;
	//printf("%d / %d\n", comp, alloc);
    fprintf(f, "%1f, %1f\n", (static_cast<double>(flip_orig_size_bdi)/static_cast<double>(flip_xor_size_bdi_perf)), (static_cast<double>(orig_size_bdi)/static_cast<double>(xor_size_bdi_perf)));
    return 0;
}
/*
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
*/
double calcRate_raw(int alloc, int comp, int orig_flip, int flip){
    orig_size += alloc;
    xor_size += comp;
    flip_orig_size += orig_flip; 
    flip_xor_size += flip;
	//printf("%d / %d\n", comp, alloc);
    fprintf(f1, "%1f, %1f\n", (static_cast<double>(flip_orig_size)/static_cast<double>(flip_xor_size)), (static_cast<double>(orig_size)/static_cast<double>(xor_size)));
    return 0;
}
double calcRatePerf_raw(int alloc, int comp, int orig_flip, int flip){
    xor_size_perf += comp;
    flip_xor_size_perf += flip;
	//printf("%d / %d\n", comp, alloc);
    fprintf(f1, "%1f, %1f\n", (static_cast<double>(flip_orig_size)/static_cast<double>(flip_xor_size_perf)), (static_cast<double>(orig_size)/static_cast<double>(xor_size_perf)));
    return 0;
}
/*
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
}*/
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



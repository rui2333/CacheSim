#include "compressor.h"
#include "visualize.h"
//
extern char* data_array; 
extern char* data_array_sub; 
extern char* data_array_orig;
extern int max_size;
extern int compress_size;
extern int compress_size_sub;
extern int compress_size_orig;
extern int uncompress_size;
extern int flip_compress_size;
extern int flip_compress_size_sub;
extern int flip_compress_size_orig;
extern int l1;
extern int index_ref[32768];
extern int index_ref_sub[32768];
extern int sizes[32768][3];
Line* var[32768];
void print_arr(){
	for(int i = 0; i < 32768; i++){
		//printf("%d: %u\n", i, *((char*)arr[i]));	
	}
}
void visualize(int start, int end){
	for(int i = start; i < end+1; i++){
		TestPrint(i);
		printf("\n");
	}
}
void TestPrint(int index){
	printf("ORIG: ");
	for(int i = 0 ; i < 64; i++){
		printf("|%d", (int)data_array_orig[index * 64 +i]);	
	}
	printf("|\n");
	printf("XOR: ");
	for(int i = 0 ; i < 64; i++){
		printf("|%d", (int)data_array[index * 64 +i]);	
	}
	printf("|\n");
}


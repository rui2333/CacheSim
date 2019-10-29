#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <iostream>
#include <limits.h>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <math.h>
#include <iostream>     // std::cout
#include <algorithm>    // std::min
#include "visualize.h"
using namespace std;
extern char* data_array;// = (char*)malloc(32768 * 64);
typedef unsigned char BYTE;
typedef struct Line{
	char Byte[64];
}Line;
Line add(Line l1, Line l2);
Line xxor(Line l1, Line l2);
Line sub(Line l1, Line l2);
int countWidth(unsigned short int num);
int getTag(long long line);
int countLine(Line line);
double calcRate(int alloc, int comp, int flip);
double calcRate_sub(int alloc, int comp, int flip);
double calcRate_orig(int alloc, int comp, int flip);
double calcRate_raw(int alloc, int comp, int flip);
double calcRate_sub_raw(int alloc, int comp, int flip);
double calcRate_orig_raw(int alloc, int comp, int flip);
Line compress(Line line);
int countFlip(Line line);
Line compare(Line line, uint32_t tag);
Line compare_sub(Line line, uint32_t tag);
Line compare_perfect(Line line, uint32_t tag);
Line cTol(int index);
Line cTol_sub(int index);
Line cTol_orig(int index);
Line cTol_perf(int index);
void LongToLine (Line line, uint32_t tag);
int recover(int tag);
int recover_sub(int tag);
Line replace(Line line, int tag);
Line replace_sub(Line line, int tag);
void printline(Line line);
int Empty(Line line);
double best_compression_rate();
double best_possible_rate(char* data_array_perf);
Line compare_perfect(Line line, uint32_t tag);
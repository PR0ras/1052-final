#ifndef __BWLABEL_HH_
#define __BWLABEL_HH_

#include "fsl_common.h"

uint8_t bwlabel(uint8_t *img, uint8_t n, uint8_t* labels);
void wallner(uint8_t *src, uint8_t *dst);
void wallner_new(uint8_t *input, uint8_t *bin);
void edge_dect(uint8_t *byteimage);
void edge_bw(uint8_t *byteimage,uint8_t *bin,uint8_t *bin1);
void edge_thr(uint8_t *byteimage,uint8_t *bin,uint8_t *bin1);
#endif

extern uint8_t mid_NUM;
extern uint8_t temp_img;
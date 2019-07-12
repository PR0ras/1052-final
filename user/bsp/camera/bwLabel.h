#ifndef __BWLABEL_HH_
#define __BWLABEL_HH_

#include "fsl_common.h"

uint8_t bwlabel(uint8_t *img, uint8_t n, uint8_t* labels);
void wallner(uint8_t *src, uint8_t *dst);
void wallner_new(uint8_t *input, uint8_t *bin);
void edge_dect(uint8_t *byteimage);
void edge_bw(uint8_t *byteimage,uint8_t *bin,uint8_t *bin1);
void edge_thr(uint8_t *byteimage,uint8_t *bin,uint8_t *bin1,uint8_t *bin2);
#endif

extern uint8_t mid_NUM,mid_NUM1,mid_NUM2,mid_NUM3,L_CNT_,R_CNT_;
extern uint8_t temp_img,temp_img1,temp_img2,temp_img3,temp_img4;
extern uint8_t tmp_BP[4][2][4];
extern int8_t Effective_line;
extern uint8_t HD_first_flag;
extern uint8_t midline[];

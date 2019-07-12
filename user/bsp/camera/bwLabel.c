#include "bwLabel.h"
#include "LQ_MT9V034M.h"

#define NO_OBJECT 0
//#define     MIN(x, y)       (((x) < (y)) ? (x) : (y))
//#define     ELEM(img, r, c) (CV_IMAGE_ELEM(img, unsigned char, r, c))
#define ONETWO(L, r, c, col) (L[(r) * (col) + c])
#define total 22560
#define col 188
#define row 120
#define thole 40
const uint8_t nr = 120;
const uint8_t nc = 188;
uint16_t prev_gn[188] = {0};
uint8_t graydata[188 * 120] = {0};
uint8_t midline[240]={0};
// AT_NONCACHEABLE_SECTION_ALIGN(uint8_t midline[240],64); 
uint8_t lset[800] = {0};
uint8_t cnt_gn[188] = {0};
uint8_t correct_par[120] = {16, 17, 18, 19, 20, 21, 22, 23, 25, 26, 27, 28, 28, 29, 30, 30, 30, 31, 32, 33, 34, 35, 36, 36, 36,
							37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50, 50, 51, 52, 53, 53, 59, 60, 61, 62, 63, 64, 64, 65, 66, 67, 68, 68, 69, 70,
							71, 72, 72, 73, 74, 74, 75, 76, 77, 77, 78, 79, 79, 80, 80, 81, 81, 82, 82, 83, 84, 84, 85, 85, 86, 86, 86, 87, 87, 87, 88, 88, 89, 89,
							89, 89, 89, 90, 90, 90, 91 , 92, 93, 93, 94, 94, 95, 95, 96, 96, 97, 97, 98, 98, 99, 99, 99, 100, 100};
// uint8_t correct_par[120] = {16, 17, 18, 19, 20, 21, 22, 23, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
// 							42, 43, 44, 45, 45, 46, 47, 48, 49, 50, 51, 51, 52, 53, 54, 55, 56, 57, 58, 58, 59, 60, 61, 62, 63, 64, 64, 65, 66, 67, 68, 68, 69, 70,
// 							71, 72, 72, 73, 74, 74, 75, 76, 77, 77, 78, 79, 79, 80, 81, 81, 82, 83, 84, 84, 85, 86, 87, 88, 88, 89, 89, 90, 90, 91, 91, 91, 92, 92,
// 							92, 92, 93, 93, 93, 93, 94, 94, 94, 94, 95, 95, 95, 95, 96, 96, 96, 96, 97, 97, 97, 97, 98, 98, 98, 98};
uint8_t mid_NUM = 0,mid_NUM1 = 0,mid_NUM2 = 0,mid_NUM3 = 0,L_CNT_=0,R_CNT_=0;
uint8_t temp_img = 0, temp_img1 = 0, temp_img2 = 0, temp_img3 = 0,temp_img4 = 0;
uint8_t tmp_BP[4][2][4]={0};
int8_t Effective_line=0;
//环岛
uint8_t HD_first_flag=0;
uint16_t labels[22560] = {0};
int find(uint8_t set[], uint8_t x)
{
	int r = x;
	while (set[r] != r)
		r = set[r];
	return r;
}
//边界追踪(输入图像,输出数组,左边界或右边界)
void edge_track(uint8_t *image,uint8_t *Line_arrayX,uint8_t *Line_arrayY,uint8_t flag);
/*
labeling scheme
+-+-+-+
|D|C|E|
+-+-+-+
|B|A| |
+-+-+-+
| | | |
+-+-+-+
A is the center pixel of a neighborhood.  In the 3 versions of
connectedness:
4:  A connects to B and C
6:  A connects to B, C, and D
8:  A connects to B, C, D, and E
*/
/////////////////////////////
//by yysdsyl
//
//input:img     --  gray image
//      n       --  n connectedness
//      labels  --  label of each pixel, labels[row * col]
//output:  number of connected regions
//
//
//email:yysdsyl@qq.com
uint8_t bwlabel(uint8_t *img, uint8_t n, uint8_t *labels)
{
	if (n != 4 && n != 8)
		n = 4;
	//	uint16_t nr = IMAGEH*2;
	//	uint16_t nc = IMAGEW/2;
	//uint16_t total = nr * nc;
	// results
	memset(labels, 0, total * sizeof(int));
	uint8_t nobj = 0; // number of objects found in image
	// other variables
	//int *lset = new int[total]; // label table
	memset(lset, 0, total);
	uint8_t ntable = 0;
	for (uint16_t r = 0; r < nr; r++)
	{
		for (uint16_t c = 0; c < nc; c++)
		{
			if (img[r * nc + c]) // if A is an object
			{
				// get the neighboring pixels B, C, D, and E
				uint8_t B, C, D, E;
				if (c == 0)
					B = 0;
				else
					B = find(lset, ONETWO(labels, r, c - 1, nc));
				if (r == 0)
					C = 0;
				else
					C = find(lset, ONETWO(labels, r - 1, c, nc));
				if (r == 0 || c == 0)
					D = 0;
				else
					D = find(lset, ONETWO(labels, r - 1, c - 1, nc));
				if (r == 0 || c == nc - 1)
					E = 0;
				else
					E = find(lset, ONETWO(labels, r - 1, c + 1, nc));
				if (n == 4)
				{
					// apply 4 connectedness
					if (B && C)
					{ // B and C are labeled
						if (B == C)
							ONETWO(labels, r, c, nc) = B;
						else
						{
							lset[C] = B;
							ONETWO(labels, r, c, nc) = B;
						}
					}
					else if (B) // B is object but C is not
						ONETWO(labels, r, c, nc) = B;
					else if (C) // C is object but B is not
						ONETWO(labels, r, c, nc) = C;
					else
					{ // B, C, D not object - new object
						//   label and put into table
						ntable++;
						ONETWO(labels, r, c, nc) = lset[ntable] = ntable;
					}
				}
				else if (n == 6)
				{
					// apply 6 connected ness
					if (D) // D object, copy label and move on
						ONETWO(labels, r, c, nc) = D;
					else if (B && C)
					{ // B and C are labeled
						if (B == C)
							ONETWO(labels, r, c, nc) = B;
						else
						{
							int tlabel = MIN(B, C);
							lset[B] = tlabel;
							lset[C] = tlabel;
							ONETWO(labels, r, c, nc) = tlabel;
						}
					}
					else if (B) // B is object but C is not
						ONETWO(labels, r, c, nc) = B;
					else if (C) // C is object but B is not
						ONETWO(labels, r, c, nc) = C;
					else
					{ // B, C, D not object - new object
						//   label and put into table
						ntable++;
						ONETWO(labels, r, c, nc) = lset[ntable] = ntable;
					}
				}
				else if (n == 8)
				{
					// apply 8 connectedness
					if (B || C || D || E)
					{
						int tlabel = B;
						if (B)
							tlabel = B;
						else if (C)
							tlabel = C;
						else if (D)
							tlabel = D;
						else if (E)
							tlabel = E;
						ONETWO(labels, r, c, nc) = tlabel;
						if (B && B != tlabel)
							lset[B] = tlabel;
						if (C && C != tlabel)
							lset[C] = tlabel;
						if (D && D != tlabel)
							lset[D] = tlabel;
						if (E && E != tlabel)
							lset[E] = tlabel;
					}
					else
					{
						//   label and put into table
						ntable++;
						ONETWO(labels, r, c, nc) = lset[ntable] = ntable;
					}
				}
			}
			else
			{
				ONETWO(labels, r, c, nc) = NO_OBJECT; // A is not an object so leave it
			}
		}
	}
	// consolidate component table
	for (uint16_t i = 0; i <= ntable; i++)
		lset[i] = find(lset, i);
	// run image through the look-up table
	for (uint16_t r = 0; r < nr; r++)
		for (uint16_t c = 0; c < nc; c++)
			ONETWO(labels, r, c, nc) = lset[ONETWO(labels, r, c, nc)];
	// count up the objects in the image
	for (uint16_t i = 0; i <= ntable; i++)
		lset[i] = 0;
	for (uint16_t r = 0; r < nr; r++)
		for (uint16_t c = 0; c < nc; c++)
			lset[ONETWO(labels, r, c, nc)]++;
	// number the objects from 1 through n objects
	nobj = 0;
	lset[0] = 0;
	for (uint16_t i = 1; i <= ntable; i++)
		if (lset[i] > 0)
			lset[i] = ++nobj;
	// run through the look-up table again
	for (uint16_t r = 0; r < nr; r++)
		for (uint16_t c = 0; c < nc; c++)
			ONETWO(labels, r, c, nc) = lset[ONETWO(labels, r, c, nc)];
	//
	//delete[] lset;
	return nobj;
}

void wallner(uint8_t *src, uint8_t *dst)
{
	/*
	* pn = 当前点的灰度值
	* s = 图片宽度/n （n = 8时效果最好）
	* t = 比例阈值+
	* 公式：g(n) = g(n-1) * (1-1/s) + p(n)
	*/

	static uint16_t t = 15;
	//	uint16_t nr = 120;
	//	uint16_t nc = 188;
	uint16_t s = nc >> 3;
	const uint8_t S = 9;
	const uint16_t power2S = 1 << S; //加速因子
	uint32_t factor = power2S * (100 - t) / (100 * s);
	/*使用初始值127*s *s是因为 原先算法采用均值也就是fn 是前n个像素之和 
	这次算法优化为与当前点越相邻对其影响越大的思路*/
	uint32_t gn = 127 * s;
	uint32_t q = power2S - power2S / s;
	uint8_t pn = 0;
	uint32_t hn = 0;
	//	uint8_t prev_gn[188] = {0}; //前一行各点像素值
	//Mat dst = Mat::zeros(src.size(), CV_8UC1);
	for (uint32_t i = 0; i < nc; i++)
		prev_gn[i] = gn;
	for (uint32_t i = 0; i < nr; i++)
	{
		for (uint32_t j = 0; j < nc; j++) //从左向右遍历
		{
			pn = src[i * nc + j];
			gn = ((gn * q) >> S) + pn;
			hn = (gn + prev_gn[j]) >> 1;
			prev_gn[j] = gn;
			if (pn < ((hn * factor) >> S))
				dst[i * nc + j] = 0;
			else
				dst[i * nc + j] = 0xff;
		}
		i++;
		if (i == nr)
			break;
		//scanline = src.ptr<uint8_t>(i);
		for (int32_t j = nc - 1; j >= 0; j--) //从右向左遍历
		{
			pn = src[i * nc + j];
			gn = ((gn * q) >> S) + pn;
			hn = (gn + prev_gn[j]) >> 1;
			prev_gn[j] = gn;
			if (pn < ((hn * factor) >> S))
				dst[i * nc + j] = 0;
			else
				dst[i * nc + j] = 0xff;
		}
	}
}

void SecondBW(uint8_t *input, uint8_t *bin)
{
	uint8_t i = 0, j = 0;
}
uint8_t Max(uint8_t A, uint8_t B);
void edge_dect(uint8_t *byteimage) //quint8 *byteimage//QImage *image1
{
	//int r=0,c=0;
	//int  bytePerLine = ( 188 * 8 + 31)/32 * 4;
	//bitimage=new QImage(graydata,188,120,bytePerLine,QImage::Format_Grayscale8);
	//return image;
	uint16_t NUM = 0;
	uint8_t A, B, C, D, E, max;
	for (uint8_t r = 118; r >= 1; r -= 1)
	{
		for (uint8_t c = 186; c >= 1; c -= 1)
		{
			NUM = col * r + c;
			A = byteimage[NUM];
			B = byteimage[NUM - col - 1];
			C = byteimage[NUM - col];
			D = byteimage[NUM - col + 1];
			E = byteimage[NUM - 1];

			max = Max(A, B);
			if (max < Max(A, C))
				max = Max(A, C);
			if (max < Max(A, D))
				max = Max(A, D);
			if (max < Max(A, E))
				max = Max(A, E);
			if (max > thole)
			{
				graydata[NUM] = 255;
			}
		}
	}
}

uint8_t Min(uint8_t A, uint8_t B);
uint8_t MAx(uint8_t A, uint8_t B);
void edge_bw(uint8_t *byteimage, uint8_t *bin, uint8_t *bin1) //quint8 *byteimage//QImage *image1
{
	uint16_t NUM = 0;
	uint8_t A, B, C, D, E, max, L_minY = 0, L_minX = 0, R_minY = 0, R_minX = 0;
	uint8_t find_err = 0;
	;
	// uint8_t labels[22560]={0};
	memset(bin, 0, 188 * 120);
	memset(graydata, 0, 188 * 120);
	memset(labels, 0, 188 * 120);
	memset(bin1, 0, 188 * 120);
	memset(midline,0,120);

	uint8_t LFX[800] = {0}, LFY[800] = {0}, RTX[800] = {0}, RTY[800] = {0}, tlabel = 0;
	// memset(labels, 0, 188*120);
	// uint32_t avrb=0,avrw=0,avrg=0,ntable=0,cnt=0;
	// for (int r=118;r>=1;r-=1)//二值化
	// {
	// 	for (int c=186;c>=1;c-=1)
	//     {
	//         //max=0;
	//         NUM=col*r+c;
	//         A=byteimage[NUM];
	//         B=byteimage[NUM-col-1];
	//         C=byteimage[NUM-col];
	//         D=byteimage[NUM-col+1];
	//         E=byteimage[NUM-1];
	//         max=Max(A,B);
	//         if(A>B){
	//             bP[cnt]=B;
	//             wP[cnt]=A;
	//         }
	//         else{
	//             bP[cnt]=A;
	//             wP[cnt]=B;
	//         }
	//         if(max<Max(A,C)){
	//             max=Max(A,C);
	//             if(A>C){
	//                 bP[cnt]=C;
	//                 wP[cnt]=A;
	//             }
	//             else{
	//                 bP[cnt]=A;
	//                 wP[cnt]=C;
	//             }
	//         }
	//         if(max<Max(A,D)){
	//             max=Max(A,D);
	//             if(A>D){
	//                 bP[cnt]=D;
	//                 wP[cnt]=A;
	//             }
	//             else{
	//                 bP[cnt]=A;
	//                 wP[cnt]=D;
	//             }
	//         }
	//         if(max<Max(A,E)){
	//             max=Max(A,E);
	//             if(A>E){
	//                 bP[cnt]=E;
	//                 wP[cnt]=A;
	//             }
	//             else{
	//                 bP[cnt]=A;
	//                 wP[cnt]=E;
	//             }
	//         }
	//        if(max>thole)
	//         {
	//             cnt++;
	//            graydata[NUM]=255;
	//         }
	//     }
	// }
	// for (int i=0;i<cnt+1;i++) {
	//     avrb+=bP[i];
	//     avrw+=wP[i];
	// }
	// avrb=(avrb/(cnt+1));
	// avrw=(avrw/(cnt+1));
	// avrg=(avrw+avrb)/2;

	// // for (int r = 0; r < 120; r++)
	// // {
	// // 	for (int c = 0; c < 188; c++)
	// // 	{
	// // 		if (byteimage[col * r + c]>avrg)
	// // 			bin[col * r + c] = 230;
	// // 	}
	// // }

	// for (uint8_t r=0;r<119;r++)
	// {
	//     for (uint8_t c=0;c<187;c++)
	//     {
	//         //max=0;
	//         NUM=col*r+c;
	//         if(byteimage[col*r+c]>avrg)
	//         {
	//             B=find(lset,labels[NUM-col-1]);
	//             C=find(lset,labels[NUM-col]);
	//             D=find(lset,labels[NUM-col-1]);
	//             E=find(lset,labels[NUM-1]);
	//             if (B || C || D || E)
	//             {
	//                 tlabel = B;
	//                 if (B)
	//                     tlabel = B;
	//                 else if (C)
	//                     tlabel = C;
	//                 else if (D)
	//                     tlabel = D;
	//                 else if (E)
	//                     tlabel = E;
	//                 labels[NUM]= tlabel;
	//                 if (B && B != tlabel)
	//                     lset[B] = tlabel;
	//                 if (C && C != tlabel)
	//                     lset[C] = tlabel;
	//                 if (D && D != tlabel)
	//                     lset[D] = tlabel;
	//                 if (E && E != tlabel)
	//                     lset[E] = tlabel;
	//             }
	//             else
	//             {
	//                 //   label and put into table
	//                 ntable++;
	//                 labels[c+r*col] = lset[ntable] = ntable;
	//             }
	//         }
	//     }
	// }
	// for (uint16_t i = 0; i <= ntable; i++)
	//         lset[i] = find(lset, i);
	// for (uint16_t r = 0; r < row; r++)
	//     for (uint16_t c = 0; c < col; c++)
	//         labels[c+r*col] = lset[labels[c+r*col]];
	// // count up the objects in the image
	// for (uint16_t i = 0; i <= ntable; i++)
	//     lset[i] = 0;
	// for (uint8_t r = 0; r < row; r++)
	//     for (uint8_t c = 0; c < col; c++)
	//         lset[labels[c+r*col]]++;
	// // number the objects from 1 through n objects
	// int nobj = 0;
	// lset[0] = 0;
	// for (uint16_t i = 1; i <= ntable; i++)
	// {
	//     if (lset[i] > 0)
	//         lset[i] = ++nobj;
	//     else {
	//         lset[i] = 0;
	//     }
	// }
	// // run through the look-up table again
	// for (uint16_t r = 0; r < row; r++)
	//     for (uint16_t c = 0; c < col; c++)
	//         labels[c+r*col] = lset[labels[c+r*col]];

	// uint8_t yu = labels[95 + 100 * col];
	// for (int r = 0; r < 120; r++)
	// {
	// 	for (int c = 0; c < 188; c++)
	// 	{
	// 		if (labels[c + r * col] == yu)
	// 			bin[col * r + c] = 230;
	// 		else
	// 			bin[col * r + c] = 0;
	// 	}
	// }

	//边缘检测
	for (uint8_t r = 1; r < 119; r++)
	{
		for (uint8_t c = 1; c < 187; c++)
		{
			NUM = col * r + c;
			A = byteimage[NUM];
			B = byteimage[NUM - col - 1];
			C = byteimage[NUM - col];
			D = byteimage[NUM - col + 1];
			E = byteimage[NUM - 1];
			max = Max(A, B);
			if (max < Max(A, C))
				max = Max(A, C);
			if (max < Max(A, D))
				max = Max(A, D);
			if (max < Max(A, E))
				max = Max(A, E);
			if (max > thole)
			{
				graydata[NUM] = 255;
			}
		}
	}

	for (uint8_t r = 0; r < 120; r++)
	{
		for (uint8_t c = 0; c < 188; c++)
		{
			if (graydata[col * r + c])
			{
				bin1[col * r + c] = 255;
				// byteimage[col*r+c]=255;
			}
		}
	}

	uint8_t O_Lxtmp = 0, O_Lytmp = 0, O_Rxtmp = 0, O_Rytmp = 0, L_first = 0, R_first = 0;
	uint16_t L_cnt = 0, R_cnt = 0;

	//左边界标号
	for (uint8_t r = 118; r >= 65; r -= 1)
	{
		for (uint8_t c = 60; c > 1; c--)
		{
			NUM = col * r + c;
			if (graydata[NUM])
			{
				if ((Max(O_Lxtmp, c) + Max(O_Lytmp, r)) > 7)
				{
					L_first = 0;
					L_cnt = 0;
				}
				L_cnt++;
				if (!L_first)
				{
					L_minY = r;
					L_minX = c;
					L_first = 1;
				}
				O_Lxtmp = c;
				O_Lytmp = r;
				// left=labels[NUM];
				break;
			}
		}
		if (L_cnt > 20)
			break;
	}
	//右边界标号
	for (uint8_t r = 118; r >= 65; r -= 1)
	{
		for (uint8_t c = 130; c < 187; c++)
		{
			NUM = col * r + c;
			if (graydata[NUM])
			{
				if ((Max(O_Rxtmp, c) + Max(O_Rytmp, r)) > 7)
				{
					R_first = 0;
					R_cnt = 0;
				}
				// right=labels[NUM];
				R_cnt++;
				if (!R_first)
				{
					R_minY = r;
					R_minX = c;
					R_first = 1;
				}
				O_Rxtmp = c;
				O_Rytmp = r;
				break;
			}
		}
		if (R_cnt > 20)
			break;
	}

	uint8_t L_Y = L_minY, L_X = L_minX, oLX = 0, oLY = 0, dir = 0;
	uint8_t R_Y = R_minY, R_X = R_minX, oRX = 0, oRY = 0;
	uint16_t CNT_L = 0, CNT_R = 0;
	//边界追踪
	// if(left)
	while (true)
	{

		switch (dir)
		{
		case 0:
			if (graydata[(L_Y + 1) * col + L_X + 1])
			{
				LFX[CNT_L] = L_X;
				LFY[CNT_L++] = L_Y;
				L_Y += 1;
				L_X += 1;
				dir = 0;
				break;
			}
			dir++;
		case 1:
			if (graydata[(L_Y)*col + L_X + 1])
			{
				LFX[CNT_L] = L_X;
				LFY[CNT_L++] = L_Y;
				L_X += 1;
				dir = 0;
				break;
			}
			dir++;
		case 2:
			if (graydata[(L_Y - 1) * col + L_X + 1])
			{
				LFX[CNT_L] = L_X;
				LFY[CNT_L++] = L_Y;
				L_Y -= 1;
				L_X += 1;
				dir -= 2;
				break;
			}
			dir++;
		case 3:
			if (graydata[(L_Y - 1) * col + L_X])
			{
				LFX[CNT_L] = L_X;
				LFY[CNT_L++] = L_Y;
				L_Y -= 1;
				dir -= 2;
				break;
			}
			dir++;
		case 4:
			if (graydata[(L_Y - 1) * col + L_X - 1])
			{
				LFX[CNT_L] = L_X;
				LFY[CNT_L++] = L_Y;
				L_Y -= 1;
				dir -= 2;
				break;
			}
			dir++;
		case 5:
			if (graydata[(L_Y)*col + L_X - 1])
			{
				LFX[CNT_L] = L_X;
				LFY[CNT_L++] = L_Y;
				L_X -= 1;
				dir -= 2;
				break;
			}
			dir++;
		case 6:
			if (graydata[(L_Y + 1) * col + L_X - 1])
			{
				LFX[CNT_L] = L_X;
				LFY[CNT_L++] = L_Y;
				L_Y += 1;
				L_X -= 1;
				dir -= 2;
				break;
			}
			dir++;
		case 7:
			if (graydata[(L_Y + 1) * col + L_X])
			{
				LFX[CNT_L] = L_X;
				LFY[CNT_L++] = L_Y;
				L_Y += 1;
				dir -= 2;
				break;
			}
			dir = 0;
		default:
			dir = 0;
			break;
		}

		if (oLX == L_X && oLY == L_Y)
		{
			if (tlabel++ > 2)
				break;
		}

		oLX = L_X;
		oLY = L_Y;
		if (L_Y < 3 || CNT_L > 700)
			break;
		if (L_X > 185)
			break;
	}

	while (true)
	{
		switch (dir)
		{
		case 0:
			if (graydata[(R_Y + 1) * col + R_X - 1])
			{
				RTX[CNT_R] = R_X;
				RTY[CNT_R++] = R_Y;
				R_Y += 1;
				R_X -= 1;
				dir = 0;
				break;
			}
			dir++;
		case 1:
			if (graydata[(R_Y)*col + R_X - 1])
			{
				RTX[CNT_R] = R_X;
				RTY[CNT_R++] = R_Y;
				R_X -= 1;
				dir = 0;
				break;
			}
			dir++;
		case 2:
			if (graydata[(R_Y - 1) * col + R_X - 1])
			{
				RTX[CNT_R] = R_X;
				RTY[CNT_R++] = R_Y;
				R_Y -= 1;
				dir = 0;
				break;
			}
			dir++;
		case 3:
			if (graydata[(R_Y - 1) * col + R_X])
			{
				RTX[CNT_R] = R_X;
				RTY[CNT_R++] = R_Y;
				R_Y -= 1;
				dir -= 2;
				break;
			}
			dir++;
		case 4:
			if (graydata[(R_Y - 1) * col + R_X + 1])
			{
				RTX[CNT_R] = R_X;
				RTY[CNT_R++] = R_Y;
				R_Y -= 1;
				R_X += 1;
				dir -= 2;
				break;
			}
			dir++;
		case 5:
			if (graydata[(R_Y)*col + R_X + 1])
			{
				RTX[CNT_R] = R_X;
				RTY[CNT_R++] = R_Y;
				R_X += 1;
				dir -= 2;
				break;
			}
			dir++;
		case 6:

			if (graydata[(R_Y + 1) * col + R_X + 1])
			{
				RTX[CNT_R] = R_X;
				RTY[CNT_R++] = R_Y;
				R_Y += 1;
				R_X += 1;
				dir -= 2;
				break;
			}
			dir++;
		case 7:
			if (graydata[(R_Y + 1) * col + R_X])
			{
				RTX[CNT_R] = R_X;
				RTY[CNT_R++] = R_Y;
				R_Y += 1;
				dir -= 2;
				break;
			}
			dir = 0;
		default:
			//dir=0;
			break;
		}
		if (oRX == R_X && oRY == R_Y)
			break;
		oRX = R_X;
		oRY = R_Y;
		if (R_Y < 3 || CNT_R > 700)
			break;
	}

	// for(uint16_t r=0;r<CNT_L;r++)
	// {
	// 	bin[LFX[r]+LFY[r]*188]=255;
	// }
	// for(uint16_t r=0;r<CNT_R;r++)
	// {
	// 	bin[RTX[r]+RTY[r]*188]=255;
	// }

	// for(uint16_t r=0;r<CNT_L;r++)
	// {
	// 	labels[LFX[r]+LFY[r]*188]=1;
	// }
	// for(uint16_t r=0;r<CNT_R;r++)
	// {
	// 	labels[RTX[r]+RTY[r]*188]=2;
	// }

	// for(uint8_t r=0;r<120;r++)
	// {
	// 	for(uint8_t c=0;c<188;c++)
	// 	{
	// 		if(labels[r*col+c])
	// 		bin[r*col+c]=255;
	// 	}
	// }

	temp_img = R_Y;
	temp_img3 = R_minY;
	//temp_img1=L_Y;
	temp_img2 = L_minY;
	// mid_NUM=L_X;

	for (uint16_t r = 0; r < CNT_L; r++)
	{
		labels[LFX[r] + LFY[r] * 188] = 1;
	}
	for (uint16_t r = 0; r < CNT_R; r++)
	{
		if (labels[RTX[r] + RTY[r] * 188] == 1)
		{
			find_err = 1;
			break;
		}
		labels[RTX[r] + RTY[r] * 188] = 2;
	}

	if (find_err)
	{
		memset(labels, 0, 188 * 120);
		if (L_minY > R_minY)
		{
			for (uint16_t r = 0; r < CNT_L; r++)
			{
				labels[LFX[r] + LFY[r] * 188] = 1;
			}
		}
		else
		{
			for (uint16_t r = 0; r < CNT_R; r++)
			{

				labels[RTX[r] + RTY[r] * 188] = 2;
			}
		}
	}
	temp_img1 = find_err;

	NUM = 0;
	uint8_t TESTLFX[240] = {0}, TESTLFY[240] = {0}, TESTRTX[240] = {0}, TESTRTY[240] = {0};
	uint8_t cntL = 0;
	uint8_t cntR = 0;
	L_first = 0;
	R_first = 0;
	uint8_t L_maxY = 120, R_maxY = 120;

	for (uint8_t r = 119; r > 0; r--)
	{
		for (uint8_t c = 0; c < 188; c++)
		{
			if (labels[r * col + c] == 1)
			{
				TESTLFX[r] = c;
				L_maxY = r;
				if (!L_first)
				{
					L_minY = r;
					L_first = 1;
				}
				break;
			}
		}
	}
	for (uint8_t r = 119; r > 0; r--)
	{
		for (uint8_t c = 187; c > 0; c--)
		{
			if (labels[r * col + c] == 2)
			{
				TESTRTX[r] = c;
				R_maxY = r;
				if (!R_first)
				{
					R_minY = r;
					R_first = 1;
				}
				break;
			}
		}
	}

	for (uint16_t r = L_minY; r > L_maxY; r--)
	{
		bin[TESTLFX[r] + r * 188] = 255;
	}
	for (uint16_t r = R_minY; r > R_maxY; r--)
	{
		bin[TESTRTX[r] + r * 188] = 255;
	}

	// //细化边缘像北科一样
	// for(uint16_t r=1;r<CNT_L-1;r++)
	// {
	//     if(LFY[r]>LFY[r-1])
	//     {
	//         TESTLFX[cntL]=LFX[r];
	//         TESTLFY[cntL++]=LFY[r];
	//     }
	//     if(LFY[r+1]<LFY[r])
	//     {
	//         TESTLFX[cntL]=LFX[r];
	//         TESTLFY[cntL++]=LFY[r];
	//     }
	// }

	// for(uint16_t r=1;r<CNT_R-1;r++)
	// {
	//     if(RTY[r]>RTY[r-1])
	//     {
	//         TESTRTX[cntR]=RTX[r];
	//         TESTRTY[cntR++]=RTY[r];
	//     }
	//     if(RTY[r+1]<RTY[r])
	//     {
	//         TESTRTX[cntR]=RTX[r];
	//         TESTRTY[cntR++]=RTY[r];
	//     }
	// }

	// for(uint16_t r=0;r<cntL;r++)
	// {
	// 	bin[TESTLFX[r]+TESTLFY[r]*188]=255;
	// }
	// for(uint16_t r=0;r<cntR;r++)
	// {
	// 	bin[TESTRTX[r]+TESTRTY[r]*188]=255;
	// }

	uint8_t mid_l[120] = {0}, mid_r[120] = {0}, mid_m[120] = {0}, cnt = 0;

	// for(uint16_t r=119;r>0;r--)
	// {
	// 	mid_m[r]=(TESTRTX[r]+TESTLFX[r])/2;
	// }

	for (uint16_t r = MAX(L_minY, R_minY); r > Min(L_maxY, R_maxY); r--)
	{
		if (TESTLFX[r])
		{
			if (TESTRTX[r])
			{
				mid_m[r] = (TESTRTX[r] + TESTLFX[r]) / 2;
			}
			else
			{
				mid_m[r] = TESTLFX[r] + correct_par[r];
			}
		}
		else
		{
			if (TESTRTX[r])
			{
				mid_m[r] = TESTRTX[r] - correct_par[r];
			}
			else
			{
				mid_m[r] = 0;
			}
		}
	}

	for (uint16_t r = Min(L_minY, R_minY); r > MAX(L_maxY, R_maxY); r--)
	{
		bin[mid_m[r] + r * 188] = 255;
	}

	// for(uint8_t r=0;r<120;r++)
	// {
	// 	mid_m[r]=(mid_l[r]+mid_r[r])/2;
	// }

	// //找出最高的Y R_minY
	// for(uint16_t r=0;r<cntL;r++)
	// {
	//    if(TESTLFY[r]<L_maxY)
	//        L_maxY=TESTLFY[r];
	// }
	// for(uint16_t r=0;r<cntR;r++)
	// {
	//     if(TESTRTY[r]<R_maxY)
	//         R_maxY=TESTRTY[r];
	// }
	// L_minY=0;
	// R_minY=0;
	// //找出最低的Y
	// for(uint16_t r=0;r<cntL;r++)
	// {
	//    if(TESTLFY[r]>L_minY)
	//        L_minY=TESTLFY[r];
	// }
	// for(uint16_t r=0;r<cntR;r++)
	// {
	//     if(TESTRTY[r]>R_minY)
	//         R_minY=TESTRTY[r];
	// }

	// for(uint8_t r=0;r<cntL;r++)
	// {
	// 	mid_l[TESTLFY[r]]=TESTLFX[r];
	// }
	// for(uint8_t r=0;r<cntR;r++)
	// {
	// 	mid_r[TESTRTY[r]]=TESTRTX[r];
	// }

	// for(uint16_t r=0;r<cntL;r++)
	// {
	// 	bin[mid_l[TESTLFY[r]]+TESTLFY[r]*188]=255;
	// }
	// for(uint16_t r=0;r<cntR;r++)
	// {
	// 	bin[mid_r[TESTRTY[r]]+TESTRTY[r]*188]=255;
	// }
	// temp_img=MAX(L_maxY,R_maxY);
	// for (uint16_t r = 119; r > MAX(L_maxY,R_maxY); r--)
	// {
	// 	if(mid_l[r])
	// 	{
	// 		if(mid_r[r])
	// 		{
	// 			mid_m[r]=(mid_l[r]+mid_r[r])/2;
	// 		}
	// 		else
	// 		{
	// 			mid_m[r]=mid_l[r]+correct_par[r];
	// 		}
	// 	}
	// 	else
	// 	{
	// 		if(mid_r[r])
	// 		{
	// 			mid_m[r]=mid_r[r]-correct_par[r];
	// 		}
	// 		else
	// 		{
	// 			mid_m[r]=0;
	// 		}

	// 	}
	// }

	//第一段找线
	// for (uint16_t r = 119; r > MAX(L_minY,R_minY); r++)
	// {
	// 	if(mid_l[r])
	// 	{
	// 		if(mid_r[r])
	// 		{
	// 			mid_m[r]=(mid_l[r]+mid_r[r])/2;
	// 		}
	// 		else
	// 		{
	// 			mid_m[r]=mid_l[r]+correct_par[r];
	// 		}

	// 	}
	// 	else
	// 	{
	// 		if(mid_r[r])
	// 		{
	// 			mid_m[r]=mid_r[r]-correct_par[r];
	// 		}
	// 		else
	// 		{
	// 			mid_m[r]=0;
	// 		}

	// 	}
	// }

	// for (uint16_t r = 0; r < 120; r++)
	// {
	// 	if(mid_r[r])
	// }

	uint16_t temp_sum = 0, cnttt = 0;
	// for(uint16_t r=0;r<120;r++)
	// {
	//     bin[mid_m[r]+r*188]=255;
	// }

	for (uint16_t r = 65; r < 85; r++)
	{
		if (mid_m[r])
		{
			temp_sum += mid_m[r];
			cnttt++;
		}
	}
	if (cnttt)
		mid_NUM = temp_sum / cnttt;

	// //if(L_first)
	// if(R_first)
	// {
	// 	for(uint8_t r = 119; r >= L_maxY; r++)
	// 	{
	// 		mid_m[r]=mid_l[r]+mid_r[r];
	// 	}
	// }
	// else
	// {
	// 	for(uint8_t r = 119; r >= L_maxY; r++)
	// 	{
	// 		mid_m[r]=mid_l[r]+correct_par[r];
	// 	}
	// }

	// }
	// if(L_maxY<R_maxY-4)
	// {
	//     for (uint16_t r=0;r<cntL;r++) {
	//         if(TESTLFY[r]<R_maxY)
	//             break;
	//         mid_l[r]=(TESTLFX[r]+TESTRTX[r])/2;
	//         cnt++;
	//     }
	// }
}

//计算连通标号
void tag_label(uint8_t *byteimage, uint16_t *labels)
{
	/*labeling scheme
	+-+-+-+
	|D|C|E|
	+-+-+-+
	|B|A| |
	+-+-+-+
	| | | |
	+-+-+-+*/
	memset(labels, 0, 22560 * 2);
	uint16_t B = 0, C = 0, D = 0, E = 0, F = 0, min_1 = 0, tlabel = 0, tmp_1 = 0, tmp_2 = 0;
	uint16_t lset[1000] = {0}, ntable = 0;
	uint16_t NUM = 0;
	for (uint8_t r = 1; r < 119; r++)
	{
		for (uint8_t c = 1; c < 187; c++)
		{
			NUM = r * col + c;
			if (byteimage[NUM])
			{
				B = labels[NUM - 1];
				C = labels[NUM - 188];
				D = labels[NUM - 189];
				E = labels[NUM - 187];
				if (B | C | D | E)
				{
					if (B)
						tlabel = B;
					else if (C)
						tlabel = C;
					else if (D)
						tlabel = D;
					else if (E)
						tlabel = E;
					min_1 = tlabel;
					if (B)
						min_1 = Min(B, min_1);
					if (C)
						min_1 = Min(C, min_1);
					if (D)
						min_1 = Min(D, min_1);
					if (E)
						min_1 = Min(E, min_1);
					labels[NUM] = min_1;
					if (B && (B != min_1))
					{
						tmp_1 = min_1;
						while (lset[tmp_1] != tmp_1)
							tmp_1 = lset[tmp_1];
						tmp_2 = B;
						while (lset[tmp_2] != tmp_2)
							tmp_2 = lset[tmp_2];
						if (lset[tmp_2] < lset[tmp_1])
							lset[tmp_1] = lset[tmp_2];
						else
							lset[tmp_2] = lset[tmp_1];
					}
					if (C && (C != min_1))
					{
						tmp_1 = min_1;
						while (lset[tmp_1] != tmp_1)
							tmp_1 = lset[tmp_1];
						tmp_2 = C;
						while (lset[tmp_2] != tmp_2)
							tmp_2 = lset[tmp_2];
						if (lset[tmp_2] < lset[tmp_1])
							lset[tmp_1] = lset[tmp_2];
						else
							lset[tmp_2] = lset[tmp_1];
					}
					if (D && (D != min_1))
					{
						tmp_1 = min_1;
						while (lset[tmp_1] != tmp_1)
							tmp_1 = lset[tmp_1];
						tmp_2 = D;
						while (lset[tmp_2] != tmp_2)
							tmp_2 = lset[tmp_2];
						if (lset[tmp_2] < lset[tmp_1])
							lset[tmp_1] = lset[tmp_2];
						else
							lset[tmp_2] = lset[tmp_1];
					}
					if (E && (E != min_1))
					{
						tmp_1 = min_1;
						while (lset[tmp_1] != tmp_1)
							tmp_1 = lset[tmp_1];
						tmp_2 = E;
						while (lset[tmp_2] != tmp_2)
							tmp_2 = lset[tmp_2];
						if (lset[tmp_2] < lset[tmp_1])
							lset[tmp_1] = lset[tmp_2];
						else
							lset[tmp_2] = lset[tmp_1];
					}
				}
				else
				{
					ntable++;
					labels[NUM] = ntable;
					lset[ntable] = ntable;
				}
			}
		}
	}
	//查表更新所有标号
	for (uint16_t c = ntable; c > 0; c--)
	{
		uint16_t j = c, minLabel = 0;
		while (lset[j] != j)
		{
			minLabel = lset[j];
			j = minLabel;
		}
		j = c;
		while (lset[j] != j)
		{
			lset[j] = minLabel;
			j = lset[j];
		}
	}
	for (uint8_t r = 0; r < 120; r++)
		for (uint8_t c = 0; c < 188; c++)
		{
			labels[r * col + c] = lset[labels[r * col + c]];
		}
}

void edge_thr(uint8_t *byteimage, uint8_t *bin, uint8_t *bin1,uint8_t *bin2)
{
	uint16_t NUM = 0;
	uint8_t A, B, C, D, E, max, L_minY = 0, L_minX = 0, R_minY = 0, R_minX = 0;
	uint8_t find_err = 0;
	uint8_t LF[2][800] = {{0}},RT[2][800] = {{0}};
	// uint8_t labels[22560]={0};
	memset(bin, 0, 188 * 120);
	memset(graydata, 0, 188 * 120);
	memset(bin1, 0, 188 * 120);
	memset(midline,0,120);
	//边缘检测
	for (uint8_t r = 19; r < 119; r++)
	{
		for (uint8_t c = 1; c < 187; c++)
		{
			NUM = col * r + c;
			A = byteimage[NUM];
			B = byteimage[NUM - col - 1];
			C = byteimage[NUM - col];
			D = byteimage[NUM - col + 1];
			E = byteimage[NUM - 1];
			max = Max(A, B);
			if (max < Max(A, C))
				max = Max(A, C);
			if (max < Max(A, D))
				max = Max(A, D);
			if (max < Max(A, E))
				max = Max(A, E);
			if (max > thole)
			{
				graydata[NUM] = 255;
			}
		}
	}
	tag_label(graydata, labels);
#if defined(USE_LCD) && (USE_LCD == 1U)
	for (uint8_t r = 0; r < 120; r++)
	{
		for (uint8_t c = 0; c < 188; c++)
		{
			if (graydata[col * r + c])
			{
				bin[col * r + c] = 255;
				// byteimage[col*r+c]=255;
			}
		}
	}
	// for (uint8_t r = 0; r < 120; r++)
	// {
	// 	for (uint8_t c = 0; c < 188; c++)
	// 	{
	// 		if (graydata[col * r + c])
	// 		{
	// 			bin2[col * r + c] = labels[col * r + c];
	// 			// byteimage[col*r+c]=255;
	// 		}
	// 	}
	// }
#endif

	uint8_t O_Lxtmp = 0, O_Lytmp = 0, O_Rxtmp = 0, O_Rytmp = 0, L_first = 0, R_first = 0;
	uint16_t L_cnt = 0, R_cnt = 0, left = 0, right = 0, old_tagget = 0;
	uint8_t L_cross = 0, R_cross = 0, left_up = 0, right_up = 0;
	//图像下部左边界标号
	for (uint8_t r = 118; r >= 65; r -= 1)
	{
		for (uint8_t c = 60; c > 1; c--)
		{
			NUM = col * r + c;
			if (graydata[NUM])
			{
				if (!L_first)
				{
					L_minY = r;
					L_minX = c;
					old_tagget = labels[NUM];
					L_first = 1;
				}
				if (old_tagget != labels[NUM])
					if ((Max(O_Lxtmp, c) + Max(O_Lytmp, r)) > 7)
					{
						L_first = 0;
						L_cnt = 0;
					}
				L_cnt++;

				O_Lxtmp = c;
				O_Lytmp = r;
				left = labels[NUM];
				break;
			}
		}
		if (L_cnt > 20)
			break;
	}
	//图像下部右边界标号
	for (uint8_t r = 118; r >= 65; r -= 1)
	{
		for (uint8_t c = 130; c < 187; c++)
		{
			NUM = col * r + c;
			if (graydata[NUM])
			{
				if (!R_first)
				{
					R_minY = r;
					R_minX = c;
					old_tagget = labels[NUM];
					R_first = 1;
				}
				if (old_tagget != labels[NUM])
					if ((Max(O_Rxtmp, c) + Max(O_Rytmp, r)) > 7)
					{
						R_first = 0;
						R_cnt = 0;
					}
				right = labels[NUM];
				R_cnt++;
				O_Rxtmp = c;
				O_Rytmp = r;
				break;
			}
		}
		if (R_cnt > 20)
			break;
	}
	uint8_t LBP[2][4] = {{0}}, RBP[2][4] = {{0}},LBP_UP[2][4] = {{0}}, RBP_UP[2][4] = {{0}}; //一个线段的最左最右最上最下四个点 X Y
	R_first = 0;
	L_first = 0;
	//左边界的最值点
	if (left)
		for (uint8_t r = 119; r > 10; r--)
		{
			for (uint8_t c = 187; c > 0; c--)
			{
				if (labels[c + r * col] == left)
				{
					if (!L_first)
					{
						LBP[0][0] = c;
						LBP[0][1] = c;
						LBP[0][2] = c;
						LBP[0][3] = c;
						LBP[1][0] = r;
						LBP[1][1] = r;
						LBP[1][2] = r;
						LBP[1][3] = r;
						L_first = 1;
					}
					//最左
					if (LBP[0][0] >= c)
					{
						LBP[0][0] = c;
						LBP[1][0] = r;
					}
					//最右
					if (LBP[0][1] <= c)
					{
						LBP[0][1] = c;
						LBP[1][1] = r;
					}
					//最上
					if (LBP[1][2] >= r)
					{
						LBP[0][2] = c;
						LBP[1][2] = r;
					}
					//最下
					if (LBP[1][3] <= r)
					{
						LBP[0][3] = c;
						LBP[1][3] = r;
					}
				}
			}
		}

	//右边界的最值点
	if (right)
		for (uint8_t r = 119; r > 10; r--)
		{
			for (uint8_t c = 0; c < 187; c++)
			{
				if (labels[c + r * col] == right)
				{
					if (!R_first)
					{
						RBP[0][0] = c;
						RBP[0][1] = c;
						RBP[0][2] = c;
						RBP[0][3] = c;
						RBP[1][0] = r;
						RBP[1][1] = r;
						RBP[1][2] = r;
						RBP[1][3] = r;
						R_first = 1;
					}
					//最左
					if (RBP[0][0] >= c)
					{
						RBP[0][0] = c;
						RBP[1][0] = r;
					}
					//最右
					if (RBP[0][1] <= c)
					{
						RBP[0][1] = c;
						RBP[1][1] = r;
					}
					//最上
					if (RBP[1][2] >= r)
					{
						RBP[0][2] = c;
						RBP[1][2] = r;
					}
					//最下
					if (RBP[1][3] <= r)
					{
						RBP[0][3] = c;
						RBP[1][3] = r;
					}
				}
			}
		}
	if (left == right)
	{
		//判断是右边界还是左边界
		//先判断是否为右边界
		if ((RBP[0][3] < 60) || (LBP[0][1] > (LBP[0][3] + 5)))
		{
			right = 0;
			for (uint8_t c = 0; c < 4; c++)
			{
				RBP[0][c] = 0;
				RBP[1][c] = 0;
			}
		}
		if (LBP[0][3] > 150)
		{
			left = 0;
			for (uint8_t c = 0; c < 4; c++)
			{
				LBP[0][c] = 0;
				LBP[1][c] = 0;
			}
		}
	}

	//左边界是否闭合到右边
	if (LBP[0][1] > 140) //左边界最左大于140
	{
		if (RBP[1][2]) //右边界存在
		{
			if (LBP[1][2] < RBP[1][2])
				L_cross = 1;
		}
		else
			L_cross = 1;
	}

	//右边界是否闭合到左边
	if ((RBP[0][0] < 50) && (RBP[0][0] != 0))
	{
		if (LBP[1][2])
		{
			if (RBP[1][2] < LBP[1][2])
				R_cross = 1;
		}
		else
			R_cross = 1;
	}

	//继续向上搜索右边界
	if (L_cross)
	{
	}
	else if(!R_cross)
	{
		if (RBP[1][2] > 30)
		{
			for (uint8_t r = RBP[1][2] - 2; r >= 20; r -= 1)
			{
				NUM = col * r + RBP[0][0];
				if (graydata[NUM])
				{
					right_up = labels[NUM];
					break;
				}
			}
		}
		else if (RBP[1][2] == 0) //如果下边界未搜索到则继续向上搜索
		{
			R_cnt = 0;
			R_first = 0;
			for (uint8_t r = 65; r >= 30; r -= 1)
			{
				for (uint8_t c = 120; c < 187; c++)
				{
					NUM = col * r + c;
					if (graydata[NUM])
					{
						if (!R_first)
						{
							R_minY = r;
							R_minX = c;
							old_tagget = labels[NUM];
							R_first = 1;
						}
						if (old_tagget != labels[NUM])
							if ((Max(O_Rxtmp, c) + Max(O_Rytmp, r)) > 7)
							{
								R_first = 0;
								R_cnt = 0;
							}
						old_tagget = labels[NUM];
						R_cnt++;
						O_Rxtmp = c;
						O_Rytmp = r;
					}
				}
				if (R_cnt > 20)
					break;
			}
			right_up = old_tagget;
		}
	}

	//继续向上搜索左边界
	if (R_cross)
	{
	}
	else if(!L_cross)
	{
		if (LBP[1][2] > 30)
		{
			for (uint8_t r = LBP[1][2] - 2; r >= 20; r -= 1)
			{
				NUM = col * r + LBP[0][1];
				if (graydata[NUM])
				{
					left_up = labels[NUM];
					break;
				}
			}
		}
		else if (LBP[1][2] == 0) //如果下边界未搜索到则继续向上搜索
		{
			L_cnt = 0;
			L_first = 0;
			for (uint8_t r = 65; r >= 30; r -= 1)
			{
				for (uint8_t c = 60; c > 1; c--)
				{
					NUM = col * r + c;
					if (graydata[NUM])
					{
						if (!L_first)
						{
							L_minY = r;
							L_minX = c;
							old_tagget = labels[NUM];
							L_first = 1;
						}
						if (old_tagget != labels[NUM])
							if ((Max(O_Lxtmp, c) + Max(O_Lytmp, r)) > 7)
							{
								L_first = 0;
								L_cnt = 0;
							}
						L_cnt++;
						O_Lxtmp = c;
						O_Lytmp = r;
						left_up = labels[NUM];
						break;
					}
				}
				if (L_cnt > 20)
					break;
			}
		}
	}
	R_first = 0;
	L_first = 0;
	//左上边界的最值点
	if (left_up)
		for (uint8_t r = 119; r > 10; r--)
		{
			for (uint8_t c = 187; c > 0; c--)
			{
				if (labels[c + r * col] == left_up)
				{
					if (!L_first)
					{
						LBP_UP[0][0] = c;
						LBP_UP[0][1] = c;
						LBP_UP[0][2] = c;
						LBP_UP[0][3] = c;
						LBP_UP[1][0] = r;
						LBP_UP[1][1] = r;
						LBP_UP[1][2] = r;
						LBP_UP[1][3] = r;
						L_first = 1;
					}
					//最左
					if (LBP_UP[0][0] >= c)
					{
						LBP_UP[0][0] = c;
						LBP_UP[1][0] = r;
					}
					//最右
					if (LBP_UP[0][1] <= c)
					{
						LBP_UP[0][1] = c;
						LBP_UP[1][1] = r;
					}
					//最上
					if (LBP_UP[1][2] >= r)
					{
						LBP_UP[0][2] = c;
						LBP_UP[1][2] = r;
					}
					//最下
					if (LBP_UP[1][3] <= r)
					{
						LBP_UP[0][3] = c;
						LBP_UP[1][3] = r;
					}
				}
			}
		}

	//右上边界的最值点
	if (right_up)
		for (uint8_t r = 119; r > 10; r--)
		{
			for (uint8_t c = 0; c < 187; c++)
			{
				if (labels[c + r * col] == right_up)
				{
					if (!R_first)
					{
						RBP_UP[0][0] = c;
						RBP_UP[0][2] = c;
						RBP_UP[0][1] = c;
						RBP_UP[0][3] = c;
						RBP_UP[1][0] = r;
						RBP_UP[1][1] = r;
						RBP_UP[1][2] = r;
						RBP_UP[1][3] = r;
						R_first = 1;
					}
					//最左
					if (RBP_UP[0][0] >= c)
					{
						RBP_UP[0][0] = c;
						RBP_UP[1][0] = r;
					}
					//最右
					if (RBP_UP[0][1] <= c)
					{
						RBP_UP[0][1] = c;
						RBP_UP[1][1] = r;
					}
					//最上
					if (RBP_UP[1][2] >= r)
					{
						RBP_UP[0][2] = c;
						RBP_UP[1][2] = r;
					}
					//最下
					if (RBP_UP[1][3] <= r)
					{
						RBP_UP[0][3] = c;
						RBP_UP[1][3] = r;
					}
				}
			}
		}
	//环岛
	HD_first_flag=0;
	if (!L_cross && !R_cross)
		if (RBP[0][3] > 180)
		{
			if ((RBP[1][2] - LBP[1][2]) > 10)
			{
				//最右上点在图像边界
				if (RBP[0][1] > 180 && (RBP[1][3] - RBP[1][1]) > 50)
					HD_first_flag = 1;
				//最上点在图像边界
				else if (RBP[0][2] > 180 && (RBP[1][2] - RBP[1][1]) < 5)
				{
					HD_first_flag = 1;
				}
			}
		}

	uint8_t TESTLFX[240] = {0}, TESTLFY[240] = {0}, TESTRTX[240] = {0}, TESTRTY[240] = {0};
	L_first = 0;
	R_first = 0;
	NUM = 0;
	uint8_t L_maxY = 120, R_maxY = 120;
	//将左边界装入数组
	if (left)
		for (uint8_t r = 119; r > 0; r--)
		{
			L_first = 0;
			for (uint8_t c = 187; c > 0; c--)
			{
				if (labels[c + r * col] == left)
				{
					if (!L_first)
					{
						TESTLFX[r] = c;
						L_first = 1;
					}
					else if((c-TESTLFX[r])==1)
					{
						TESTLFX[r] = c;
					}
					else if((c-TESTLFX[r])>5)
					{
						TESTLFX[r+120] = c;
						break;
					}
				}
			}
		}
	//将右边界装入数组
	if (right)
		for (uint8_t r = 119; r > 0; r--)
		{
			R_first = 0;
			for (uint8_t c = 0; c < 188; c++)
			{
				if (labels[c + r * col] == right)
				{
					if (!R_first)
					{
						TESTRTX[r] = c;
						R_first = 1;
					}
					else if((c-TESTRTX[r])==1)
					{
						TESTRTX[r] = c;
					}
					else if((c-TESTRTX[r])>5)
					{
						TESTRTX[r+120] = c;
						break;
					}
				}
			}
		}

	//将左上边界装入数组
	if (left_up)
		for (uint8_t r = 119; r > 0; r--)
		{
			L_first = 0;
			for (uint8_t c = 187; c > 0; c--)
			{
				if (labels[c + r * col] == left_up)
				{
					if (!L_first)
					{
						TESTLFX[r] = c;
						L_first = 1;
					}
					else if ((c - TESTLFX[r]) == 1)
					{
						TESTLFX[r] = c;
					}
					else if ((c - TESTLFX[r]) > 5)
					{
						TESTLFX[r + 120] = c;
						break;
					}
				}
			}
		}
	//将右上边界装入数组
	if (right_up)
		for (uint8_t r = 119; r > 0; r--)
		{
			R_first = 0;
			for (uint8_t c = 0; c < 188; c++)
			{
				if (labels[c + r * col] == right_up)
				{
					if (!R_first)
					{
						TESTRTX[r] = c;
						R_first = 1;
					}
					else if((c-TESTRTX[r])==1)
					{
						TESTRTX[r] = c;
					}
					else if((c-TESTRTX[r])>5)
					{
						TESTRTX[r+120] = c;
						break;
					}
				}
			}
		}

	for (uint8_t r = 119; r > 0; r--)
	{
		if(R_cross||L_cross)
		{
			if (r > Max(RBP[1][2], LBP[1][2]))
			{
				if (TESTLFX[r + 120] && TESTRTX[r + 120])
				{
					midline[r] = (TESTLFX[r + 120] + TESTRTX[r + 120]) / 2;
				}
				else if (TESTLFX[r + 120])
				{
					if (TESTRTX[r])
						midline[r] = TESTRTX[r] - correct_par[r];
					// midline[r] = (TESTLFX[r + 120] + TESTRTX[r]) / 2;
				}
				else if (TESTRTX[r + 120])
				{
					if (TESTLFX[r])
						midline[r] = TESTLFX[r] + correct_par[r];
					// midline[r] = (TESTLFX[r] + TESTRTX[r + 120]) / 2;
				}
				else if (TESTLFX[r] && TESTRTX[r])
				{
					midline[r] = (TESTLFX[r] + TESTRTX[r]) / 2;
				}
			}
			else if(r>((LBP[1][2]+RBP[1][2])/2))
			{
				if (TESTLFX[r])
				{
					midline[r] = TESTLFX[r] + correct_par[r];
				}
				else if (TESTRTX[r])
				{
					midline[r] = TESTRTX[r] - correct_par[r];
				}
			}
		}
	}
	//有效行
	// Effective_line=Min(LBP[1][3],RBP[1][3])-MAx(LBP[1][2],RBP[1][2]);//有效行计算，用于判断赛道元素
	// if(Effective_line<0||(!right&&!right_up)||(!left&&!left_up))
	// 	Effective_line=0;

	temp_img = left;
	// mid_NUM = right;
	temp_img1 =L_cross;
	temp_img2 =R_cross;
	temp_img3 =left_up;
	temp_img4 =right_up;
	memcpy(tmp_BP[0],LBP,8);
	memcpy(tmp_BP[1],RBP,8);
	memcpy(tmp_BP[2],LBP_UP,8);
	memcpy(tmp_BP[3],RBP_UP,8);
	#if defined(USE_LCD) && (USE_LCD == 1U)
	memcpy(tmp_BP[0],LBP,8);
	memcpy(tmp_BP[1],RBP,8);
	memcpy(tmp_BP[2],LBP_UP,8);
	memcpy(tmp_BP[3],RBP_UP,8);
	for (uint8_t r = 1; r < 119; r++)
	{
		if (TESTLFX[r])
			bin1[r * col + TESTLFX[r]] = 255;
		if (TESTRTX[r])
			bin1[r * col + TESTRTX[r]] = 255;
		if (TESTLFX[r+120])
			bin1[r * col + TESTLFX[r+120]] = 255;
		if (TESTRTX[r+120])
			bin1[r * col + TESTRTX[r+120]] = 255;
		if(midline[r])
			bin1[r * col + midline[r]] = 150;
	}
	#endif
		memset(bin2, 0, 188 * 120);
	// for (uint8_t r = 0; r < 120; r++)
	// {
	// 	for (uint8_t c = 0; c < 188; c++)
	// 	{
	// 		if (graydata[col * r + c]||r==30)
	// 		{
	// 			// bin2[col * r + c] = labels[col * r + c];
	// 			bin2[col * r + c]=255;
	// 			// byteimage[col*r+c]=255;
	// 		}
	// 	}
	// }

	for (uint8_t r = 1; r < 119; r++)
	{
		for (uint8_t c = 1; c <187; c++)
		{
			if(labels[c+r*188]==left||r==90)
				bin2[c+r*188]=255;
			if(labels[c+r*188]==right)
				bin2[c+r*188]=255;
		}
	}
	for (uint8_t r = 1; r < 119; r++)
	{
		bin2[midline[r]+r*188]=100;
	}
}

void edge_track(uint8_t *image, uint8_t *Line_arrayX, uint8_t *Line_arrayY, uint8_t flag)
{
	
	uint8_t L_Y = 0, L_X = 0, oLX = 0, oLY = 0, dir = 0;
	uint8_t R_Y = 0, R_X = 0, oRX = 0, oRY = 0;
	uint16_t CNT_L = 0, CNT_R = 0,tlabel=0;
	//边界追踪
	if (flag)
	{
		while (true)
		{
			switch (dir)
			{
			case 0:
				if (graydata[(L_Y + 1) * col + L_X + 1])
				{
					Line_arrayX[CNT_L] = L_X;
					Line_arrayY[CNT_L++] = L_Y;
					L_Y += 1;
					L_X += 1;
					dir = 0;
					break;
				}
				dir++;
			case 1:
				if (graydata[(L_Y)*col + L_X + 1])
				{
					Line_arrayX[CNT_L] = L_X;
					Line_arrayY[CNT_L++] = L_Y;
					L_X += 1;
					dir = 0;
					break;
				}
				dir++;
			case 2:
				if (graydata[(L_Y - 1) * col + L_X + 1])
				{
					Line_arrayX[CNT_L] = L_X;
					Line_arrayY[CNT_L++] = L_Y;
					L_Y -= 1;
					L_X += 1;
					dir -= 2;
					break;
				}
				dir++;
			case 3:
				if (graydata[(L_Y - 1) * col + L_X])
				{
					Line_arrayX[CNT_L] = L_X;
					Line_arrayY[CNT_L++] = L_Y;
					L_Y -= 1;
					dir -= 2;
					break;
				}
				dir++;
			case 4:
				if (graydata[(L_Y - 1) * col + L_X - 1])
				{
					Line_arrayX[CNT_L] = L_X;
					Line_arrayY[CNT_L++] = L_Y;
					L_Y -= 1;
					dir -= 2;
					break;
				}
				dir++;
			case 5:
				if (graydata[(L_Y)*col + L_X - 1])
				{
					Line_arrayX[CNT_L] = L_X;
					Line_arrayY[CNT_L++] = L_Y;
					L_X -= 1;
					dir -= 2;
					break;
				}
				dir++;
			case 6:
				if (graydata[(L_Y + 1) * col + L_X - 1])
				{
					Line_arrayX[CNT_L] = L_X;
					Line_arrayY[CNT_L++] = L_Y;
					L_Y += 1;
					L_X -= 1;
					dir -= 2;
					break;
				}
				dir++;
			case 7:
				if (graydata[(L_Y + 1) * col + L_X])
				{
					Line_arrayX[CNT_L] = L_X;
					Line_arrayY[CNT_L++] = L_Y;
					L_Y += 1;
					dir -= 2;
					break;
				}
				dir = 0;
			default:
				dir = 0;
				break;
			}

			if (oLX == L_X && oLY == L_Y)
			{
				if (tlabel++ > 2)
					break;
			}

			oLX = L_X;
			oLY = L_Y;
			if (L_Y < 3 || CNT_L > 700)
				break;
			if (L_X > 185)
				break;
		}
	}
	else
	{
		while (true)
		{
			switch (dir)
			{
			case 0:
				if (graydata[(R_Y + 1) * col + R_X - 1])
				{
					Line_arrayX[CNT_R] = R_X;
					Line_arrayY[CNT_R++] = R_Y;
					R_Y += 1;
					R_X -= 1;
					dir = 0;
					break;
				}
				dir++;
			case 1:
				if (graydata[(R_Y)*col + R_X - 1])
				{
					Line_arrayX[CNT_R] = R_X;
					Line_arrayY[CNT_R++] = R_Y;
					R_X -= 1;
					dir = 0;
					break;
				}
				dir++;
			case 2:
				if (graydata[(R_Y - 1) * col + R_X - 1])
				{
					Line_arrayX[CNT_R] = R_X;
					Line_arrayY[CNT_R++] = R_Y;
					R_Y -= 1;
					dir = 0;
					break;
				}
				dir++;
			case 3:
				if (graydata[(R_Y - 1) * col + R_X])
				{
					Line_arrayX[CNT_R] = R_X;
					Line_arrayY[CNT_R++] = R_Y;
					R_Y -= 1;
					dir -= 2;
					break;
				}
				dir++;
			case 4:
				if (graydata[(R_Y - 1) * col + R_X + 1])
				{
					Line_arrayX[CNT_R] = R_X;
					Line_arrayY[CNT_R++] = R_Y;
					R_Y -= 1;
					R_X += 1;
					dir -= 2;
					break;
				}
				dir++;
			case 5:
				if (graydata[(R_Y)*col + R_X + 1])
				{
					Line_arrayX[CNT_R] = R_X;
					Line_arrayY[CNT_R++] = R_Y;
					R_X += 1;
					dir -= 2;
					break;
				}
				dir++;
			case 6:

				if (graydata[(R_Y + 1) * col + R_X + 1])
				{
					Line_arrayX[CNT_R] = R_X;
					Line_arrayY[CNT_R++] = R_Y;
					R_Y += 1;
					R_X += 1;
					dir -= 2;
					break;
				}
				dir++;
			case 7:
				if (graydata[(R_Y + 1) * col + R_X])
				{
					Line_arrayX[CNT_R] = R_X;
					Line_arrayY[CNT_R++] = R_Y;
					R_Y += 1;
					dir -= 2;
					break;
				}
				dir = 0;
			default:
				//dir=0;
				break;
			}
			if (oRX == R_X && oRY == R_Y)
				break;
			oRX = R_X;
			oRY = R_Y;
			if (R_Y < 3 || CNT_R > 700)
				break;
		}
	}
}

uint8_t Max(uint8_t A, uint8_t B)
{

	if (A > B)
	{
		return A - B;
	}
	return B - A;
}

uint8_t Min(uint8_t A, uint8_t B)
{
	if (A > B)
	{
		return B;
	}
	return A;
}

uint8_t MAx(uint8_t A, uint8_t B)
{
	if (A < B)
	{
		return B;
	}
	return A;
}
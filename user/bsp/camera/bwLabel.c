#include "bwLabel.h"
#include "LQ_MT9V034M.h"

#define NO_OBJECT 0
//#define     MIN(x, y)       (((x) < (y)) ? (x) : (y))
//#define     ELEM(img, r, c) (CV_IMAGE_ELEM(img, unsigned char, r, c))
#define ONETWO(L, r, c, col) (L[(r) * (col) + c])
#define total IMAGEH*IMAGEW
const	uint8_t nr = 120;
const	uint8_t nc = 188;
uint16_t prev_gn[188] = {0};
uint32_t integralImg[22560]={0}; 
int find(uint8_t set[], uint8_t x)
{
	int r = x;
	while (set[r] != r)
		r = set[r];
	return r;
}
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
	static uint8_t lset[total] = {0};
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
	* t = 比例阈值
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
	uint8_t pn=0;
	uint32_t hn=0;
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
			if (pn<((hn * factor)>> S))
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
			if (pn<((hn * factor)>> S))
				dst[i * nc + j] = 0;
			else
				dst[i * nc + j] = 0xff;
		}
	}
}

void wallner_new(uint8_t *input, uint8_t *bin)
{
	int16_t S = nc >> 3;
	int16_t T = 15;

	// unsigned long *integralImg = 0;
	uint8_t i, j;
	int32_t sum = 0;
	int32_t count = 0;
	int16_t x1, y1, x2, y2;
	int16_t s2 = S / 2;

	//bin = new uint8_t[nc * nr];
	// create the integral image
	for (i = 0; i < nc; i++)
	{
		// reset this column sum
		sum = 0;
		for (j = 0; j < nr; j++)
		{
			sum += input[j * nc + i];
			if (i == 0)
				integralImg[j * nc + i] = sum;
			else
				integralImg[j * nc + i] = integralImg[j * nc + i - 1] + sum;
		}
	}
	// perform thresholding
	for (i = 0; i < nc; i++)
	{
		for (j = 0; j < nr; j++)
		{
			// set the SxS region
			x1 = i - s2;
			x2 = i + s2;
			y1 = j - s2;
			y2 = j + s2;
			// check the border
			if (x1 < 0)
				x1 = 0;
			if (x2 >= nc)
				x2 = nc - 1;
			if (y1 < 0)
				y1 = 0;
			if (y2 >= nr)
				y2 = nr - 1;
			count = (x2 - x1) * (y2 - y1);
			// I(x,y)=s(x2,y2)-s(x1,y2)-s(x2,y1)+s(x1,x1)
			sum = integralImg[y2 * nc + x2] -
				  integralImg[y1 * nc + x2] -
				  integralImg[y2 * nc + x1] +
				  integralImg[y1 * nc + x1];
			if ((int32_t)(input[j * nc + i] * count) < (int32_t)((sum * (100 - T)) / 100))
				bin[j * nc + i] = 0;
			else
				bin[j * nc + i] = 255;
		}
	}
}

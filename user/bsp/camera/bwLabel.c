#include "bwLabel.h"
#include "LQ_MT9V034M.h"

#define NO_OBJECT 0
//#define     MIN(x, y)       (((x) < (y)) ? (x) : (y))
//#define     ELEM(img, r, c) (CV_IMAGE_ELEM(img, unsigned char, r, c))
#define ONETWO(L, r, c, col) (L[(r) * (col) + c])
#define total 22560
#define col 188
#define row 120
#define thole 30
const	uint8_t nr = 120;
const	uint8_t nc = 188;
uint16_t prev_gn[188] = {0};
uint8_t graydata[188*120]={0},bP[800]={0},wP[800]={0},labels[188*120]={0};
uint8_t lset[800] = {0};
uint8_t correct_par[120]={16,17,18,19,20,21,22,23,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,
42,43,44,45,45,46,47,48,49,50,51,51,52,53,54,55,56,57,58,58,59,60,61,62,63,64,64,65,66,67,68,68,69,70,
71,72,72,73,74,74,75,76,77,77,78,79,79,80,81,81,82,83,84,84,85,86,87,88,88,89,89,90,90,91,91,91,92,92,
92,92,93,93,93,93,94,94,94,94,95,95,95,95,96,96,96,96,97,97,97,97,98,98,98,98};
uint8_t mid_NUM=0;
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

void SecondBW(uint8_t *input, uint8_t *bin)
{
	uint8_t  i=0,j=0;
}
uint8_t Max(uint8_t A,uint8_t B);
void edge_dect(uint8_t *byteimage)//quint8 *byteimage//QImage *image1
{
    //int r=0,c=0;
    //int  bytePerLine = ( 188 * 8 + 31)/32 * 4;
    //bitimage=new QImage(graydata,188,120,bytePerLine,QImage::Format_Grayscale8);
    //return image;
    uint16_t NUM=0;
    uint8_t A,B,C,D,E,max;
    for (uint8_t r=118;r>=1;r-=1)
    {
        for (uint8_t c=186;c>=1;c-=1)
        {
            NUM=col*r+c;
            A=byteimage[NUM];
            B=byteimage[NUM-col-1];
            C=byteimage[NUM-col];
            D=byteimage[NUM-col+1];
            E=byteimage[NUM-1];

			max=Max(A,B);
            if(max<Max(A,C))
                max=Max(A,C);
            if(max<Max(A,D))
                max=Max(A,D);
            if(max<Max(A,E))
                max=Max(A,E);
           if(max>thole)
            {
               graydata[NUM]=255;
            }
        }
    }
}

void edge_bw(uint8_t *byteimage,uint8_t *bin) //quint8 *byteimage//QImage *image1
{
	uint16_t NUM = 0;
	uint8_t A, B, C, D, E, max,L_minY=0,L_minX=0,R_minY=0,R_minX=0;
	memset(bin, 0, 188*120);
	memset(graydata, 0, 188*120);

	uint8_t LFX[800]={0},LFY[800]={0},RTX[800]={0},RTY[800]={0},tlabel=0;
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
		for (uint8_t c = 1; c <187; c++)
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
	
	// for (uint8_t r=0;r<120;r++)
    // {
    //     for (uint8_t c=0;c<188;c++)
    //     {
    //         if(graydata[col*r+c])
	// 		{
	// 			// bin[col*r+c]=255;
	// 			byteimage[col*r+c]=255;
	// 		}
    //     }
    // }

	uint8_t O_Lxtmp=0,O_Lytmp=0,O_Rxtmp=0,O_Rytmp=0,L_first=0,R_first=0;
	uint16_t L_cnt=0,R_cnt=0;

    //左边界标号
	for (uint8_t r = 118; r >= 65; r -= 1)
	{
		for (uint8_t c = 60; c > 1; c--)
		{
			NUM = col * r + c;
			if (graydata[NUM])
			{
				if((Max(O_Lxtmp,c)+Max(O_Lytmp,r))>4)
                {
					L_first = 0;
					L_cnt = 0;
                }
                L_cnt++;
                if(!L_first)
                {
					L_minY = r;
					L_minX = c;
					L_first = 1;
                }
				O_Lxtmp=c;
				O_Lytmp=r;
                // left=labels[NUM];
                break;
			}
		}
		if (L_cnt > 10)
			break;
	}
	//右边界标号
    for (uint8_t r=118;r>=65;r-=1)
    {
        for (uint8_t c=130;c<187;c++)
        {
            NUM=col*r+c;
            if(graydata[NUM])
            {
               if((Max(O_Rxtmp,c)+Max(O_Rytmp,r))>4)
                {
                    R_first=0;
                    R_cnt=0;
                }
                // right=labels[NUM];
                R_cnt++;
                if(!R_first)
                {
                    R_minY=r;
                    R_minX=c;
                    R_first=1;
                }
				O_Rxtmp=c;
				O_Rytmp=r;
                break;
            }
        }
        if(R_cnt>10)
            break;
    }

	uint8_t L_Y=L_minY,L_X=L_minX,oLX=0,oLY=0,dir=0;
	uint8_t R_Y=R_minY,R_X=R_minX,oRX=0,oRY=0;
	uint16_t CNT_L=0,CNT_R=0;
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
			break;
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

	NUM=0;
    uint8_t TESTLFX[240]={0},TESTLFY[240]={0},TESTRTX[240]={0},TESTRTY[240]={0};
    uint8_t cntL=0;
    uint8_t cntR=0;
	//细化边缘像北科一样
    for(uint16_t r=1;r<CNT_L-1;r++)
    {
        if(LFY[r]>LFY[r-1])
        {
            TESTLFX[cntL]=LFX[r];
            TESTLFY[cntL++]=LFY[r];
        }
        if(LFY[r+1]<LFY[r])
        {
            TESTLFX[cntL]=LFX[r];
            TESTLFY[cntL++]=LFY[r];
        }
    }

    for(uint16_t r=1;r<CNT_R-1;r++)
    {
        if(RTY[r]>RTY[r-1])
        {
            TESTRTX[cntR]=RTX[r];
            TESTRTY[cntR++]=RTY[r];
        }
        if(RTY[r+1]<RTY[r])
        {
            TESTRTX[cntR]=RTX[r];
            TESTRTY[cntR++]=RTY[r];
        }
    }


	for(uint16_t r=0;r<cntL;r++)
	{
		bin[TESTLFX[r]+TESTLFY[r]*188]=255;
	}
	for(uint16_t r=0;r<cntR;r++)
	{
		bin[TESTRTX[r]+TESTRTY[r]*188]=255;
	}



    // for(uint16_t r=0;r<cntL;r++)
    // {
    //     test1[TESTLFX[r]+TESTLFY[r]*188]=1;
    // }

    // for(uint16_t r=0;r<cntR;r++)
    // {
    //     test1[TESTRTX[r]+TESTRTY[r]*188]=2;
    // }

	uint8_t L_maxY=120,R_maxY=120;
    uint8_t mid_l[120]={0},mid_r[120]={0},mid_m[120]={0},cnt=0;

	for(uint8_t r=0;r<cntL;r++)
	{
		mid_l[TESTLFY[r]]=TESTLFX[r];
	}
	for(uint8_t r=0;r<cntR;r++)
	{
		mid_r[TESTRTY[r]]=TESTRTX[r];		
	}

	// for(uint8_t r=0;r<120;r++)
	// {
	// 	mid_m[r]=(mid_l[r]+mid_r[r])/2;		
	// }

	//找出最高的Y
    for(uint16_t r=0;r<cntL;r++)
    {
       if(TESTLFY[r]<L_maxY)
           L_maxY=TESTLFY[r];
    }
    for(uint16_t r=0;r<cntR;r++)
    {
        if(TESTRTY[r]<R_maxY)
            R_maxY=TESTRTY[r];
    }

	// for(uint16_t r=0;r<cntL;r++)
	// {
	// 	bin[mid_l[TESTLFY[r]]+TESTLFY[r]*188]=255;
	// }
	// for(uint16_t r=0;r<cntR;r++)
	// {
	// 	bin[mid_r[TESTRTY[r]]+TESTRTY[r]*188]=255;
	// }
	for (uint16_t r = 0; r < 120; r++)
	{
		if(mid_l[r])
		{
			if(mid_r[r])
			{
				mid_m[r]=(mid_l[r]+mid_r[r])/2;	
			}
			else
			{
				mid_m[r]=mid_l[r]+correct_par[r];
			}
			
		}
		else
		{
			if(mid_r[r])
			{
				mid_m[r]=mid_r[r]-correct_par[r];
			}
			else
			{
				mid_m[r]=0;
			}
			
		}
	}
	// for (uint16_t r = 0; r < 120; r++)
	// {
	// 	if(mid_r[r])
	// }
	uint16_t temp_sum=0,cnttt=0;
	for(uint16_t r=0;r<120;r++)
    {
        bin[mid_m[r]+r*188]=255;
    }
	for(uint16_t r=50;r<70;r++)
	{
		if(mid_m[r])
		{
			temp_sum+=mid_m[r];
			cnttt++;
		}
	}
	if(cnttt)
		mid_NUM=temp_sum/cnttt;
	

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


uint8_t Max(uint8_t A,uint8_t B)
{

    if(A>B)
    {
        return A-B;
    }
    return B-A;
}
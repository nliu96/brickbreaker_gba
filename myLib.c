#include "myLib.h"

unsigned short *videoBuffer = (unsigned short *)0x6000000;
int score = 0;

void setPixel(int row, int col, u16 color)
{
	videoBuffer[OFFSET(row, col, 240)] = color;
}

void drawRect(int row, int col, int height, int width, u16 color)
{
	int r;
	for(r=0; r<height; r++)
	{
		DMA[3].src = &color;
		DMA[3].dst = videoBuffer +OFFSET(row+r, col, 240);
		DMA[3].cnt = width | DMA_ON | DMA_SOURCE_FIXED;
	}
}

void drawImage3(int col, int row, int width, int height, const u16* image)
{
	for (int r = 0; r < height; r++) {
		DMA[3].src = &image[OFFSET(r, 0, width)];
		DMA[3].dst = &videoBuffer[OFFSET(row + r, col, 240)];
		DMA[3].cnt = width | DMA_ON;
	}
}

int boundsCheck(int *var, int bound, int *delta, int size)
{
	if(*var < 0)
	{
		*var = 0;
		*delta = -*delta;
		return 1;
	}
	if(*var > bound-size+1)
	{
		*var = bound-size+1;
		*delta = -*delta;
	}
	return 0;
}

int paddleCheck(int *var, int bound, int size) {
	if(*var < 0) {
		*var = 0;
		return 1;
	}
	if(*var > bound-size) {
		*var = bound-size;
	}
	return 0;
}

int hitBall(int ballRow, int ballCol, int objRow, int objCol, int objWidth, int objHeight) {
	if((ballRow + 5) >=  objRow && (objRow + objHeight - 1) >= ballRow) {
		if((ballCol + 5) >= objCol && (objCol + objWidth - 1) >= ballCol) {
			return 1;
		}
	}
	return 0;
}

int hitSide(int ballRow, int ballCol, int objRow, int objCol, int objWidth, int ballHeight) {
	if((ballCol + 3) < objCol || ballCol > (objCol + objWidth - 2)) {
		if(ballRow >= (objRow - 1) || (objRow + ballHeight + 1) >= ballRow) {
			return 1;
		}
	}
	return 0;
}

void WaitForVblank()
{
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}
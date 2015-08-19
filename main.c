#include <stdio.h>
#include <stdlib.h>
#include "myLib.h"
#include "text.h"
#include "images.h"



void title();
int game();
void win();
void lose();

int main() {
	REG_DISPCNT = MODE_3 | BG2_EN;

	enum GBAState state = TITLE;
	
	while(1) {
		WaitForVblank();
		switch(state) {
			case TITLE:
				title();
				state = LEVEL1;
				break;
			case LEVEL1:
				state = game(1);
				if (state == WIN) {
					state = LEVEL2;
				}
				break;
			case LEVEL2:
				state = game(2);
				if (state == WIN) {
					state = LEVEL3;
				}
				break;
			case LEVEL3:
				state = game(3);
				break;
			case WIN:
				win();
				state = TITLE;
				break;
			case LOSE:
				lose();
				state = TITLE;
				break;
		}
	}
	return 0;
}

void title() {
	drawImage3(0, 0, TITLE_WIDTH, TITLE_HEIGHT, title_data);
	drawString(20, 20, "BRICK BREAKER", BLACK);
	while(KEY_DOWN_NOW(BUTTON_START));
	while(!KEY_DOWN_NOW(BUTTON_START));
}


int game(int level) {

	u16 bgcolor = YELLOW;
	DMA[3].src = &bgcolor;
	DMA[3].dst = videoBuffer;
	DMA[3].cnt = 38400 | DMA_ON | DMA_SOURCE_FIXED;

	int numBricks = level + 3;
	int bricksHit = 0;
	char buffer[50];

	BALL ball;
	BRICK bricks[numBricks];
	BRICK *cur;
	PADDLE paddle;

	ball.row = 120;
	ball.col = 120;
	ball.rd = 2;
	ball.cd = 2;
	ball.size = 5;
	ball.color = BLACK;
	BALL oldBall = ball;
	int i = 0;
	int c = 0;
	int r = 40;
	while(i < numBricks) {
		if ((rand()%2) && (50+30*c)<210 && r < 81) {
			bricks[i].row = r;
			bricks[i].col = 50 + 30 * c;
			bricks[i].width = 20;
			bricks[i].height = 5;
			bricks[i].hit = 0;
			drawRect(bricks[i].row, bricks[i].col, bricks[i].height, bricks[i].width, CYAN);
			i++;
		} else if((50 + 30 * c) < 210) {
			r += 10;
			c = 0;
		}
		if (r > 80) {
			numBricks = i;
		}
		c++;

	}
	if (numBricks == 0) {
		numBricks = 3;
		for(int j = 0; j < numBricks; j++) {
			bricks[j].row = 50;
			bricks[j].col = 80 + 30 * j;
			bricks[j].width = 20;
			bricks[j].height = 5;
			bricks[j].hit = 0;
			drawRect(bricks[j].row, bricks[j].col, bricks[j].height, bricks[j].width, CYAN);
		}
	}
	drawImage3(ball.col, ball.row, 5, 5, ball_data);
	paddle.row = 140;
	paddle.col = 110;
	paddle.width = 40;
	paddle.height = 1;
	paddle.cd = 2;
	PADDLE oldPaddle = paddle;

	char levelBuff[5];
	drawRect(paddle.row, paddle.col, 5, paddle.width, BLACK);
	sprintf(levelBuff, "Level %d", level);
	drawString(20, 70, levelBuff, BLACK);
	while(!KEY_DOWN_NOW(BUTTON_A) && !KEY_DOWN_NOW(BUTTON_SELECT));
	while(KEY_DOWN_NOW(BUTTON_A) || KEY_DOWN_NOW(BUTTON_SELECT)) {
		if(KEY_DOWN_NOW(BUTTON_SELECT)) {
			return TITLE;
		} 
	}
	drawRect(20, 70, 8, 100, YELLOW);
	while(1) {
		if(KEY_DOWN_NOW(BUTTON_LEFT)) {
			paddle.col -= paddle.cd;
			paddleCheck(&paddle.col, 240, paddle.width);
		}
		if(KEY_DOWN_NOW(BUTTON_RIGHT)) {
			paddle.col += paddle.cd;
			paddleCheck(&paddle.col, 240, paddle.width);
		}
		if(hitBall(ball.row, ball.col, paddle.row, paddle.col, paddle.width, paddle.height)) {
			if(hitSide(ball.row, ball.col, paddle.row, paddle.col, paddle.width, 5)) {
				if(ball.col < paddle.col + 2) {
					ball.col = paddle.col - 6;
				} else {
					ball.col = paddle.col + paddle.width + 1;
				}
				ball.cd = -ball.cd;
			} else {
				if(ball.row < paddle.row) {
					ball.row = paddle.row - 6;
				} else {
					ball.row = paddle.row + paddle.height + 1;
				}
				ball.rd = -ball.rd;
			}
		}
		ball.row += ball.rd;
		ball.col += ball.cd;
		for(int j = 0; j < numBricks; j++) {
			cur = bricks + j;
			if(!cur->hit) {
				if(hitBall(ball.row, ball.col, cur->row, cur->col, cur->width, cur->height)) {
					cur->hit = 1;
					score++;
					bricksHit++;
					if(hitSide(ball.row, ball.col, cur->row, cur->col, cur->width, 5)) {
						if(ball.col < cur->col + 2) {
							ball.col = cur->col - 6;
						} else {
							ball.col = cur->col + cur->width + 1;
						}
						ball.cd = -ball.cd;
					} else {
						if(ball.row < cur->row) {
							ball.row = cur->row - 6;
						} else {
							ball.row = cur->row + cur->height + 1;
						}
						ball.rd = -ball.rd;
					}
					drawRect(cur->row, cur->col, cur->height, cur->width, YELLOW);
					cur->width = 0;
					cur->height = 0;
					cur->row = 0;
					cur->col = 0;
				}
			}
		}
		sprintf(buffer, "Score: %d", score);
		if(ball.row >= 159 - ball.size) {
			return LOSE;
		}
		if(bricksHit == numBricks) {
			return WIN;
		}
		boundsCheck(&ball.row, 159, &ball.rd, ball.size);
		boundsCheck(&ball.col, 239, &ball.cd, ball.size);
		if(KEY_DOWN_NOW(BUTTON_SELECT)) {
			return TITLE;
		}
		WaitForVblank();
		drawRect(oldBall.row, oldBall.col, 5, 5, YELLOW);
		drawImage3(ball.col, ball.row, 5, 5, ball_data);
		drawRect(oldPaddle.row, oldPaddle.col, 5, oldPaddle.width, YELLOW);
		drawRect(paddle.row, paddle.col, 5, paddle.width, BLACK);
		drawRect(150, 41, 10, 36, YELLOW);
		drawString(150, 5, buffer, BLACK);
		oldPaddle = paddle;
		oldBall = ball;
	}
}

void win() {
	drawImage3(0, 0, VICTORY_WIDTH, VICTORY_HEIGHT, victory_data);
	drawString(40, 80, "YOU WIN", BLACK);
	while(KEY_DOWN_NOW(BUTTON_START)||KEY_DOWN_NOW(BUTTON_SELECT));
	while(!KEY_DOWN_NOW(BUTTON_START) && !KEY_DOWN_NOW(BUTTON_SELECT));
}

void lose() {
	drawImage3(0, 0, DEFEAT_WIDTH, DEFEAT_HEIGHT, defeat_data);
	char buffer[10];
	// drawString(40, 80, "YOU LOSE", BLACK);
	sprintf(buffer, "Score: %d", score);
	drawString(50, 80, buffer ,WHITE);
	score = 0;
	while(KEY_DOWN_NOW(BUTTON_SELECT)||KEY_DOWN_NOW(BUTTON_START));
	while(!KEY_DOWN_NOW(BUTTON_START) && !KEY_DOWN_NOW(BUTTON_SELECT));
}
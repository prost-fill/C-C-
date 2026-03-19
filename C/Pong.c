#include <ncurses.h>
#include <stdio.h>

void drawField(int width, int height, int ballX, int ballY, int leftPaddleY, int rightPaddleY, int leftScore, int rightScore) {
    const int PADDLE_SIZE = 3;
    int i, j;
    
    attron(COLOR_PAIR(1));
    for (i = 1; i < height - 1; i++) {
        for (j = 1; j < width / 2; j++) {
            mvaddch(i, j, ' ');
        }
    }
    attroff(COLOR_PAIR(1));
    
    attron(COLOR_PAIR(2));
    for (i = 1; i < height - 1; i++) {
        for (j = width / 2; j < width - 1; j++) {
            mvaddch(i, j, ' ');
        }
    }
    attroff(COLOR_PAIR(2));
    
    attron(COLOR_PAIR(3));
    for (i = 0; i < width; i++) {
        mvaddch(0, i, '=');
    }
    
    for (i = 0; i < width; i++) {
        mvaddch(height - 1, i, '=');
    }
    
    for (i = 1; i < height - 1; i++) {
        mvaddch(i, 0, '|');
    }
    
    for (i = 1; i < height - 1; i++) {
        mvaddch(i, width - 1, '|');
    }
    
    mvaddch(0, 0, '+');
    mvaddch(0, width - 1, '+');
    mvaddch(height - 1, 0, '+');
    mvaddch(height - 1, width - 1, '+');
    
    for (i = 1; i < height - 1; i++) {
        if (i % 2 == 0) {
            mvaddch(i, width / 2, ':');
        }
    }
    
    for (i = 0; i < PADDLE_SIZE; i++) {
        mvaddch(leftPaddleY + i, 1, '|');
        mvaddch(leftPaddleY + i, 2, '|');
    }
    
    for (i = 0; i < PADDLE_SIZE; i++) {
        mvaddch(rightPaddleY + i, width - 3, '|');
        mvaddch(rightPaddleY + i, width - 2, '|');
    }
    attroff(COLOR_PAIR(3));
    
    attron(COLOR_PAIR(3));
    mvaddch(ballY, ballX, 'O');
    attroff(COLOR_PAIR(3));
    
    attron(COLOR_PAIR(3));
    mvprintw(0, width / 2 - 5, "%d", leftScore);
    mvprintw(0, width / 2 + 5, "%d", rightScore);
    attroff(COLOR_PAIR(3));
}

int main() {
    const int FIELD_WIDTH = 80;
    const int FIELD_HEIGHT = 25;
    const int PADDLE_SIZE = 3;
    const int WIN_SCORE = 21;
    
    int ballX, ballY;
    int ballDX, ballDY;
    
    int leftPaddleY, rightPaddleY;
    int leftScore, rightScore;
    
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    
    start_color();
    init_pair(1, COLOR_RED, COLOR_RED);
    init_pair(2, COLOR_BLUE, COLOR_BLUE);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    
    ballX = FIELD_WIDTH / 2;
    ballY = FIELD_HEIGHT / 2;
    
    static int seed = 1;
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    ballDX = (seed % 2) ? 1 : -1;
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    ballDY = (seed % 2) ? 1 : -1;
    
    leftPaddleY = FIELD_HEIGHT / 2 - PADDLE_SIZE / 2;
    leftScore = 0;
    
    rightPaddleY = FIELD_HEIGHT / 2 - PADDLE_SIZE / 2;
    rightScore = 0;
    
    int ch;
    int gameRunning = 1;
    
    while (gameRunning) {
        clear();
        
        drawField(FIELD_WIDTH, FIELD_HEIGHT, ballX, ballY, leftPaddleY, rightPaddleY, leftScore, rightScore);
        
        if (leftScore >= WIN_SCORE) {
            attron(COLOR_PAIR(3));
            mvprintw(FIELD_HEIGHT / 2, FIELD_WIDTH / 2 - 10, "LEFT PLAYER WINS!");
            attroff(COLOR_PAIR(3));
            refresh();
            napms(3000);
            break;
        }
        if (rightScore >= WIN_SCORE) {
            attron(COLOR_PAIR(3));
            mvprintw(FIELD_HEIGHT / 2, FIELD_WIDTH / 2 - 10, "RIGHT PLAYER WINS!");
            attroff(COLOR_PAIR(3));
            refresh();
            napms(3000);
            break;
        }
        
        refresh();
        
        ch = getch();
        switch(ch) {
            case 'a':
            case 'A':
                if (leftPaddleY > 1) {
                    leftPaddleY--;
                }
                break;
            case 'z':
            case 'Z':
                if (leftPaddleY + PADDLE_SIZE < FIELD_HEIGHT - 1) {
                    leftPaddleY++;
                }
                break;
            case 'k':
            case 'K':
                if (rightPaddleY > 1) {
                    rightPaddleY--;
                }
                break;
            case 'm':
            case 'M':
                if (rightPaddleY + PADDLE_SIZE < FIELD_HEIGHT - 1) {
                    rightPaddleY++;
                }
                break;
            case ' ':
                break;
            case 'q':
            case 'Q':
                gameRunning = 0;
                break;
        }
        
        ballX += ballDX;
        ballY += ballDY;
        
        if (ballY <= 1 || ballY >= FIELD_HEIGHT - 2) {
            ballDY = -ballDY;
        }
        
        if (ballX == 2) {
            if (ballY >= leftPaddleY && ballY < leftPaddleY + PADDLE_SIZE) {
                ballDX = -ballDX;
                ballDY += (ballY - (leftPaddleY + PADDLE_SIZE / 2)) / 2;
            }
        }
        
        if (ballX == FIELD_WIDTH - 3) {
            if (ballY >= rightPaddleY && ballY < rightPaddleY + PADDLE_SIZE) {
                ballDX = -ballDX;
                ballDY += (ballY - (rightPaddleY + PADDLE_SIZE / 2)) / 2;
            }
        }
        
        if (ballX <= 1) {
            rightScore++;
            ballX = FIELD_WIDTH / 2;
            ballY = FIELD_HEIGHT / 2;
            
            seed = (seed * 1103515245 + 12345) & 0x7fffffff;
            ballDX = 1;
            ballDY = (seed % 2) ? 1 : -1;
        }
        
        if (ballX >= FIELD_WIDTH - 2) {
            leftScore++;
            ballX = FIELD_WIDTH / 2;
            ballY = FIELD_HEIGHT / 2;
            
            seed = (seed * 1103515245 + 12345) & 0x7fffffff;
            ballDX = -1;
            ballDY = (seed % 2) ? 1 : -1;
        }
        
        napms(50);
    }
    
    endwin();
    
    return 0;
}
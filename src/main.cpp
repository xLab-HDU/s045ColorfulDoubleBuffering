#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <windows.h>
#include <conio.h>
#include <time.h>
using namespace std;

HANDLE hOutput1, hOutput2;
COORD coord = { 0, 0 };
DWORD bytes = 0;
bool BufferSwapFlag = false;

bool isGameOver;
const int mWidth = 20;
const int mHeight = 20;
char ScreenData[mWidth + 5][mHeight + 5];

int headX, headY, fruitX, fruitY, mScore;
int tailX[100], tailY[100];
int nTail;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

void Initial()
{
    hOutput1 = CreateConsoleScreenBuffer(
        GENERIC_WRITE,//定义进程可以往缓冲区写数据
        FILE_SHARE_WRITE,//定义缓冲区可共享写权限
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );
    hOutput2 = CreateConsoleScreenBuffer(
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL
    );
    //隐藏两个缓冲区的光标
    CONSOLE_CURSOR_INFO cci;
    cci.bVisible = 0;
    cci.dwSize = 1;
    SetConsoleCursorInfo(hOutput1, &cci);
    SetConsoleCursorInfo(hOutput2, &cci);

    isGameOver = false;
    dir = STOP;
    headX = mWidth / 2;
    headY = mHeight / 2;
    srand((unsigned)time(NULL));
    fruitX = rand() % (mWidth-2)+1;
    fruitY = rand() % (mHeight-2)+1;
    mScore = 0;
}
void Draw()
{
    system("cls");
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    int textColor = 0x06;
    SetConsoleTextAttribute(h, textColor);

    for (int i = 0; i < mWidth + 1; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < mHeight; i++)
    {
        for (int j = 0; j < mWidth; j++)
        {
            if (j == 0)
                cout << "#";
            else if (i == fruitY && j == fruitX)
            {
                textColor = 0x04;
                SetConsoleTextAttribute(h, textColor);
                cout << "F";
            }
            else if (i == headY && j == headX)
            {
                textColor = 0x0a;
                SetConsoleTextAttribute(h, textColor);
                cout << "O";
            }
            else
            {
                bool flagPrint = false;
                for (int k = 0; k < nTail; k++)
                {
                    if (tailX[k] == j && tailY[k] == i)
                    {
                        cout << "o";
                        flagPrint = true;
                    }
                }

                textColor = 0x06;
                SetConsoleTextAttribute(h, textColor);
                if (!flagPrint)
                    cout << " ";
            }

            if (j == mWidth - 1)
                cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < mWidth + 1; i++)
        cout << "#";
    cout << endl;
    cout << "游戏得分" << mScore << endl;
}
void Draw2()
{
    int i, j;
    int currentLine = 0;

    for (j = 0; j < mWidth + 2; j++)
        ScreenData[currentLine][j] = '#';
    currentLine++;

    for (i = 0; i < mHeight; i++)
    {
        for (j = 0; j < mWidth; j++)
        {
            if (j == 0)
                ScreenData[currentLine + i][j] = '#';
            else if (i == fruitY && j == fruitX)
            {
                ScreenData[currentLine + i][j] = 'F';
            }
            else if (i == headY && j == headX)
            {
                ScreenData[currentLine + i][j] = 'O';
            }
            else
            {
                bool flagPrint = false;
                for (int k = 0; k < nTail; k++)
                {
                    if (tailX[k] == j && tailY[k] == i)
                    {
                        ScreenData[currentLine + i][j] = 'o';
                        flagPrint = true;
                    }
                }

                if (!flagPrint)
                    ScreenData[currentLine + i][j] = ' ';;
            }

            if (j == mWidth - 1)
                ScreenData[currentLine + i][j] = '#';
        }
    }

    for (j = 0; j < mWidth + 2; j++)
        ScreenData[currentLine + i][j] = '#';
    currentLine++;

    sprintf(ScreenData[currentLine + i], "游戏得分: %d", mScore);
}
void Show_doubelBuffer()
{
    HANDLE hBuf;
    WORD textColor;
    int i, j;
    Draw2();//在缓冲区画当前游戏区	

    if (BufferSwapFlag == false)
    {
        BufferSwapFlag = true;
        hBuf = hOutput1;
    }
    else
    {
        BufferSwapFlag = false;
        hBuf = hOutput2;
    }
    //对ScreenData数组的内容进行上色，并将属性传到输出缓冲区hBuf
    for (i = 0; i < mHeight + 5; i++)
    {
        coord.Y = i;
        for (j = 0; j < mWidth + 5; j++)
        {
            coord.X = j;
            if (ScreenData[i][j] == 'O')
            {
                textColor = 0x03;
            }
            else if (ScreenData[i][j] == 'F')
            {
                textColor = 0x04;
            }
            else if (ScreenData[i][j] == 'o')
            {
                textColor = 0x0a;
            }
            else
            {
                textColor = 0x06;
            }

            WriteConsoleOutputAttribute(hBuf, &textColor, 1, coord, &bytes);
        }
        coord.X = 0;
        WriteConsoleOutputCharacterA(hBuf, ScreenData[i], mWidth, coord, &bytes);
    }
    SetConsoleActiveScreenBuffer(hBuf);
}
void Input()
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 'a':
            dir = LEFT;
            break;
        case 'd':
            dir = RIGHT;
            break;
        case 'w':
            dir = UP;
            break;
        case 's':
            dir = DOWN;
            break;
        case 'x':
            isGameOver = true;
            break;
        default:
            break;
        }
    }
}
void Logic()
{
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = headX;
    tailY[0] = headY;

    for (int i = 1; i < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir)
    {
    case LEFT:
        headX--;
        break;
    case RIGHT:
        headX++;
        break;
    case UP:
        headY--;
        break;
    case DOWN:
        headY++;
        break;
    default:
        break;
    }
    //if (x > mWidth || x < 0 || y > mHeight || y < 0)
    //	isGameOver = true;
    for (int i = 0; i < nTail; i++)
        if (tailX[i] == headX && tailY[i] == headY)
            isGameOver = true;

    if (headX >= mWidth - 1) headX = 1;	else if (headX < 1)	headX = mWidth - 2;
    if (headY >= mHeight) headY = 0; else if (headY < 0) headY = mHeight - 1;

    if (headX == fruitX && headY == fruitY)
    {
        mScore += 10;
        fruitX = rand() % (mWidth - 2) + 1;
        fruitY = rand() % (mHeight - 2) + 1;
        nTail++;
    }
}
int main()
{
    // 设置控制台为UTF-8
    SetConsoleOutputCP(CP_UTF8);
    Initial();
    while (!isGameOver)
    {
        //Draw();
        Show_doubelBuffer();
        Input();
        Logic();
        Sleep(100);
    }

    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleActiveScreenBuffer(h);
    system("pause");
    return 0;
}
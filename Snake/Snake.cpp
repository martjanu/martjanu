#include <iostream>
#include <windows.h>
#include <conio.h>
#include <chrono>
#include <thread>
#include <fstream>
#include <vector>
#include <algorithm> 

using namespace std;

// Globalus kintamieji
CONST char BORDER = '#';
CONST char HEAD = 'O';
CONST char BODY = 'o';
CONST char FRUIT = '@';

CONST int HEIGHT = 15;
CONST int WIDTH = 50;

//Kryptys
CONST int UP = 1;
CONST int RIGHT = 2;
CONST int DOWN = 3;
CONST int LEFT = 4;

int currentdir = 0, newDir;

//Pozicijos
char arena[WIDTH][HEIGHT];
int currentHeadPosX = WIDTH / 2,

currentHeadPosY = HEIGHT / 2;

int currentFruitX, currentFruitY;

int cBodyX[1000], cBodyY[1000];
int prevX[1000], prevY[1000];

bool gameOver = false;
bool isAlive = false;

int bodyLenght = 0;

//Taskai
int score = -5;
int pointsPerFruit = 5;

//Greitis ir laikas
int speedUpPerFruit = 5;
int baseSpeed = 150;
int timeToLoad = 1000;

struct Player
{
    string name;
    int score;
};

//Sarasai
vector<Player> playerList;
Player newPlayer;

// Metodai
void GotoXY(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Delay(int time)
{
    chrono::milliseconds duration(time);
    this_thread::sleep_for(duration);
}

void DrawArena()
{
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1)
            {
                arena[i][j] = BORDER;
                cout << arena[i][j];
            }
            else
            {
                arena[i][j] = ' ';
                cout << arena[i][j];

            }
            if (j == WIDTH - 1)
            {
                cout << endl;
            }
        }
    }
}

void ClearCell()
{
    GotoXY(currentHeadPosX, currentHeadPosY);
    cout << ' ';
    arena[currentHeadPosX][currentHeadPosY] = ' ';
}

void GameOver()
{
    gameOver = true;
    Delay(timeToLoad);
}

void ChooseDirection()
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 'w':
            newDir = UP;
            if (currentdir != DOWN)
            {
                currentdir = newDir;
            }
            break;
        case 's':
            newDir = DOWN;
            if (currentdir != UP)
            {
                currentdir = newDir;
            }
            break;
        case 'a':
            newDir = LEFT;
            if (currentdir != RIGHT)
            {
                currentdir = newDir;
            }
            break;
        case 'd':
            newDir = RIGHT;
            if (currentdir != LEFT)
            {
                currentdir = newDir;
            }
            break;
        case 'x':
            gameOver = true;
        }
    }
}

void MoveByDirection()
{
    switch (currentdir)
    {
    case UP:
        if (currentHeadPosY - 1 != 0 && arena[currentHeadPosX][currentHeadPosY - 1] != BODY)
        {
            ClearCell();
            currentHeadPosY--;
        }
        else
            GameOver();
        break;
    case DOWN:
        if (currentHeadPosY + 1 != HEIGHT - 1 && arena[currentHeadPosX][currentHeadPosY + 1] != BODY)
        {
            ClearCell();
            currentHeadPosY++;
        }
        else
            GameOver();
        break;
    case LEFT:
        if (currentHeadPosX - 1 != 0 && arena[currentHeadPosX - 1][currentHeadPosY] != BODY)
        {
            ClearCell();
            currentHeadPosX--;
        }
        else
            GameOver();
        break;
    case RIGHT:
        if (currentHeadPosX + 1 != WIDTH - 1 && arena[currentHeadPosX + 1][currentHeadPosY] != BODY)
        {
            ClearCell();
            currentHeadPosX++;
        }
        else
            GameOver();
        break;

    }

}

void MoveBody()
{
    if (bodyLenght > 0)
    {
        for (int i = 0; i < bodyLenght + 1; i++)
        {
            cBodyX[i + 1] = prevX[i];
            cBodyY[i + 1] = prevY[i];

            arena[cBodyX[i + 1]][cBodyY[i + 1]] = BODY;

            GotoXY(cBodyX[i + 1], cBodyY[i + 1]);
            cout << arena[cBodyX[i + 1]][cBodyY[i + 1]];

            prevX[i] = cBodyX[i];
            prevY[i] = cBodyY[i];

            if (i == bodyLenght)
            {
                arena[cBodyX[i + 1]][cBodyY[i + 1]] = ' ';

                GotoXY(cBodyX[i + 1], cBodyY[i + 1]);
                cout << arena[cBodyX[i + 1]][cBodyY[i + 1]];
            }
        }
    }
}

void MovementManagement()
{
    prevX[0] = currentHeadPosX;
    prevY[0] = currentHeadPosY;

    ChooseDirection();
    MoveByDirection();
    
    arena[currentHeadPosX][currentHeadPosY] = HEAD;

    GotoXY(currentHeadPosX, currentHeadPosY);
    cout << arena[currentHeadPosX][currentHeadPosY];

    MoveBody();
}

void FruitManagement()
{
    if (!isAlive)
    {
        currentFruitX = rand() % (WIDTH - 1);
        currentFruitY = rand() % (HEIGHT - 1);

        if (arena[currentFruitX][currentFruitY] != BORDER && arena[currentFruitX][currentFruitY] != BODY)
        {
            score += pointsPerFruit;
            baseSpeed -= speedUpPerFruit;
            isAlive = true;
            arena[currentFruitX][currentFruitY] = FRUIT;

            GotoXY(currentFruitX, currentFruitY);
            cout << arena[currentFruitX][currentFruitY];
        }
    }

    if (currentHeadPosX == currentFruitX && currentHeadPosY == currentFruitY)
    {
        isAlive = false;
        bodyLenght++;
    }
}

void ScoreManagement()
{
    GotoXY(WIDTH + 5, HEIGHT);
    cout << "SCORE: " << score;
}

void ReadFile()
{
    ifstream myFile;
    myFile.open("Leaderboard.txt");
    Player oldPlayer;

    while (myFile >> oldPlayer.name >> oldPlayer.score)
    {
        playerList.push_back(oldPlayer);
    }

    myFile.close();
}

bool CompareByScore(const Player& a, const Player& b)
{
    return a.score > b.score;
}

void EnterName()
{
    newPlayer.score = score;

    system("cls");
    cout << "Enter your name: ";
    cin >> newPlayer.name;
    system("cls");

    playerList.push_back(newPlayer);
}

void WriteFile()
{
    int place = 1;
    ofstream myFile;
    myFile.open("Leaderboard.txt");
    for (Player i : playerList)
    {
        cout << place << ". " << i.name << " " << i.score << endl;
        place++;

        myFile << i.name << " " << i.score << endl;
    }
    myFile.close();
}

void Leaderboard()
{
    EnterName();
    ReadFile();
    sort(playerList.begin(), playerList.end(), CompareByScore);
    WriteFile();
}

int main()
{
    DrawArena();
    while (!gameOver)
    {
        Delay(baseSpeed);
        MovementManagement();
        FruitManagement();
        ScoreManagement();
    }
    Leaderboard();

    // Nustumti baigiamaji teksta apacion
    GotoXY(0, 20);
    cout << " ";
    return 0;
}
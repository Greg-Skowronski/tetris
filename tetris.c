#include "primlib.h"
#include "pieces.h"
#include <stdlib.h>
#include <time.h>

//area width and height in squares
#define AREA_WIDTH 10
#define AREA_HEIGHT 20

//area data in pixels
//#define SIZE_OF_SINGLE_BLOCK 30
#define GAME_AREA_WIDTH SIZE_OF_SINGLE_BLOCK*AREA_WIDTH
#define GAME_AREA_HEIGHT SIZE_OF_SINGLE_BLOCK*AREA_HEIGHT

//coordinate x and y of left upper corner
#define LEFT_UPPER_CORNER 50

//others
#define NUMBER_OF_PIECES 7

//delay values
#define DELAY_DEFAULT 200
#define DELAY_FASTER 100
#define DELAY_DROP 10
#define DELAY_WAIT_FOR_DECISION 1500
#define DELAY_PATTERN 50

//game screen net
#define SMALLER_FRAME 5
#define BIGGER_FRAME 10

//draw piece info 
#define INFO_POSITION_X 500
#define INFO_LEVEL_Y 50
#define INFO_SCORE_Y 70
#define INFO_NEXT_PIECE_Y 110
#define INFO_DISPLAYED_PIECE_Y 150
#define LEVELTEXT_SIZE 20
#define SCORETEXT_SIZE 30

//instruction
#define INSTRUCTION_INFO_X 400
#define INSTRUCTION_INFO_Y 500
#define INSTRUCTION_INFO_EACH_LINE_Y 20

//game over
#define PATTERN_MAX 200
#define GAME_OVER_X 110
#define GAME_OVER_EACH_Y 40

//find best area size 
#define FIRST_CORRECT_AREA_SIZE 10
#define SECOND_CORRECT_AREA_SIZE 20
#define THIRD_CORRECT_AREA_SIZE 30


//GLOBAL VARIABLES
int SIZE_OF_SINGLE_BLOCK = 30;
char GameArea[AREA_WIDTH][AREA_HEIGHT+1];
char GameAreaStopLevel[AREA_WIDTH][AREA_HEIGHT+1];
int Level = 0;
int Score = 0;
int ScoreMultiplier = 1;

//FUNCTIONS
void SetFloor();
void SetGameAreaStopLevel();
void GameScreenNet();
void ZeroArray();
void DrawCells();
void CheckDoneLevel();
int CheckExceedRight(int, int);
void DrawNextPieceInfo(int);
void LoseScreen();
void PrintTableInPrintf();
int MoveLeft(int, int ,int ,int);
int MoveRight(int, int, int, int);
void ChooseCorrectSizeOfBlock();
void ColorBlockToCheckHim(int, int);
int CheckTopAndBottom(int, int, int, int);

int CheckWhereIs2X(char[4][4]);
int CheckWhereIs2Y(char[4][4]);
int CheckDifferenceInPositionOf2X(char[4][4], char[4][4]);
int CheckDifferenceInPositionOf2Y(char[4][4], char[4][4]);
int CheckPosibilityOfRotation(int, int);
int CorrectThePositionX=0;
int CorrectThePositionY=0;

int main()
{
  ChooseCorrectSizeOfBlock();
  
    srand(time(NULL));
    int CurrentX, CurrentY, CurrentPiece = rand()%NUMBER_OF_PIECES, CurrentRotation = 0;
    int AddHeight=0;
    int HitGround=0; 
    SDL_Event ButtonPressed;
    int MoveHorizontal;
    int DelayHit;
    int NextPiece;
    int Decision=1;
    int timecounter;
    
	if(initGraph())
	{
		exit(3);
	}
	/* clear screen */
	filledRect(0, 0, screenWidth() - 1, screenHeight() - 1, BLACK);
        
        SetFloor();
        ZeroArray();
        GameScreenNet();
        SetGameAreaStopLevel();
        
        CurrentPiece= rand()%NUMBER_OF_PIECES; 
        
        while(Decision==1){
            
            GameScreenNet();
            SetGameAreaStopLevel();
            AddHeight=0;
            NextPiece = rand()%NUMBER_OF_PIECES; 
            CurrentRotation=0;
            HitGround=0;
            MoveHorizontal=0;
            DelayHit=DELAY_DEFAULT;
            ScoreMultiplier=1;
            
        do{
            
            if(AddHeight>0)
        {
        for(CurrentX=0; CurrentX<4; CurrentX++)
        {
            for(CurrentY=0; CurrentY<4; CurrentY++)
            {
                if(pieces[CurrentPiece][CurrentRotation%4][CurrentX][CurrentY] != 0)
                {
                GameArea[(AREA_WIDTH/2-1)+CurrentX+MoveHorizontal][CurrentY+AddHeight-1] = 0;
                //printf("\nPainted: T[%d][%d] \n", (AREA_WIDTH/2-1)+CurrentX, CurrentY);
                }
            }
        }
        }
        
        DrawCells();
        DrawNextPieceInfo(NextPiece);
        updateScreen();
        
//        PrintTableInPrintf();
        
        timecounter=0;
        while(timecounter<DelayHit)
        {
            while(SDL_PollEvent(&ButtonPressed))
        {
            if(ButtonPressed.type == SDL_QUIT || ButtonPressed.key.keysym.sym == SDLK_ESCAPE)
                return 0;
            else if(ButtonPressed.type == SDL_KEYDOWN)
            {
                if(ButtonPressed.key.keysym.sym == SDLK_LEFT && MoveHorizontal>(-1)*AREA_WIDTH/2+1  && MoveLeft(CurrentPiece, CurrentRotation, AddHeight, MoveHorizontal) != 1)
                {
                    MoveHorizontal--;
                }
                else if(ButtonPressed.key.keysym.sym == SDLK_RIGHT && MoveHorizontal<AREA_WIDTH/2 - 3 + (4-CheckExceedRight(CurrentPiece, CurrentRotation)) && MoveRight(CurrentPiece, CurrentRotation, AddHeight, MoveHorizontal) != 1)
                {
                    MoveHorizontal++;
                }
                else if(ButtonPressed.key.keysym.sym == SDLK_UP && MoveHorizontal<AREA_WIDTH/2 - 3 + (4-CheckExceedRight(CurrentPiece, CurrentRotation+1) + 1) && MoveLeft(CurrentPiece, CurrentRotation+1, AddHeight, MoveHorizontal) != 1 && MoveRight(CurrentPiece, CurrentRotation+1, AddHeight, MoveHorizontal) != 1)
                {
                    
                    CorrectThePositionX = CheckDifferenceInPositionOf2X(pieces[CurrentPiece][CurrentRotation%4], pieces[CurrentPiece][(CurrentRotation+1)%4]);
                    CorrectThePositionY = CheckDifferenceInPositionOf2Y(pieces[CurrentPiece][CurrentRotation%4], pieces[CurrentPiece][(CurrentRotation+1)%4]);
                    
                    if(CheckPosibilityOfRotation(MoveHorizontal, AddHeight) != 1)
                    {
                    MoveHorizontal=MoveHorizontal+CorrectThePositionX;
                    AddHeight=AddHeight+CorrectThePositionY;
                    CurrentRotation++;
                    }
                    
                }
                else if(ButtonPressed.key.keysym.sym == SDLK_SPACE)
                {
                    DelayHit=DELAY_DROP;
                    ScoreMultiplier=4;
                }
                else if(isKeyDown(SDLK_DOWN)==1)
                {
                    DelayHit=DELAY_FASTER;
                    ScoreMultiplier=2;
                }
            }
        }
        if(isKeyDown(SDLK_DOWN)==0 && DelayHit!=DELAY_DROP)
                {
                    DelayHit=DELAY_DEFAULT;
                    ScoreMultiplier=1;
                }
        timecounter++;
        }
        //SDL_Delay(5);
            
        
        for(CurrentX=0; CurrentX<4; CurrentX++)
        {
            for(CurrentY=0; CurrentY<4; CurrentY++)
            {
                if(pieces[CurrentPiece][CurrentRotation%4][CurrentX][CurrentY] != 0)
                {
                GameArea[(AREA_WIDTH/2-1)+CurrentX+MoveHorizontal][CurrentY+AddHeight] = pieces[CurrentPiece][CurrentRotation%4][CurrentX][CurrentY];
                //printf("\nPainted: T[%d][%d] \n", (AREA_WIDTH/2-1)+CurrentX, CurrentY);
                }
            }
        }
        Score=Score+ScoreMultiplier;
        DrawCells();
        DrawNextPieceInfo(NextPiece);
        updateScreen();
//        PrintTableInPrintf();
        SDL_Delay(DelayHit);
        
        AddHeight++;
        
        for(CurrentX=0; CurrentX<AREA_WIDTH; CurrentX++)
    {
        for(CurrentY=0; CurrentY<AREA_HEIGHT; CurrentY++)
        {
            if(GameAreaStopLevel[CurrentX][CurrentY]==1)
            {
                if(GameArea[CurrentX][CurrentY]!=0) 
                {
                    HitGround=1;
                    SetGameAreaStopLevel();
                    if(HitGround==1)
                    {
                        CheckDoneLevel();
                    }
                    if(HitGround==1 && AddHeight<=1)
                    {
                        LoseScreen();
                        SDL_Delay(DELAY_WAIT_FOR_DECISION);
                        Decision=getkey();
                        if(Decision=='r' || Decision=='R')
                        {
                            SetFloor();
                            ZeroArray();
                            GameScreenNet();
                            SetGameAreaStopLevel();
                            DrawCells();
                            Level = 0;
                            Score = 0;
                            Decision=1;
                        }
                        else
                        {
                            //return 0;
                            Decision=0;
                        }
                    }
                }
            }
        }
    }
        
        //DrawCells();
        //updateScreen();
        //getkey();
        
        }while(HitGround==0);
        
        CurrentPiece = NextPiece;
        
        }
        
        
	return 0;
}

void SetFloor()
{
    int CurrentX;
    for(CurrentX=0; CurrentX<AREA_WIDTH; CurrentX++)
    {
        GameArea[CurrentX][AREA_HEIGHT]=1;
    }
}

void SetGameAreaStopLevel()
{
    int CurrentX, CurrentY;
    
    for(CurrentX=0; CurrentX<AREA_WIDTH; CurrentX++)
    {
        for(CurrentY=0; CurrentY<AREA_HEIGHT+1; CurrentY++)
        {
                GameAreaStopLevel[CurrentX][CurrentY]=0;
        }
    }
    
    for(CurrentX=0; CurrentX<AREA_WIDTH; CurrentX++)
    {
        for(CurrentY=1; CurrentY<AREA_HEIGHT+1; CurrentY++)
        {
            if(GameArea[CurrentX][CurrentY]!=0 && GameArea[CurrentX][CurrentY-1]==0)
            {
                GameAreaStopLevel[CurrentX][CurrentY-1]=1;
            }
        }
    }
}

void GameScreenNet()
{
    int CellX=0;
    int CellY=0;
    
    for(CellX=0;CellX<GAME_AREA_WIDTH;CellX=CellX+SIZE_OF_SINGLE_BLOCK)
    {
        for(CellY=0;CellY<GAME_AREA_HEIGHT;CellY=CellY+SIZE_OF_SINGLE_BLOCK)
        {
            rect(LEFT_UPPER_CORNER+CellX, LEFT_UPPER_CORNER+CellY, GAME_AREA_WIDTH+LEFT_UPPER_CORNER, GAME_AREA_HEIGHT+LEFT_UPPER_CORNER, WHITE);
        }
    }
    
    rect(LEFT_UPPER_CORNER, LEFT_UPPER_CORNER, GAME_AREA_WIDTH+LEFT_UPPER_CORNER, GAME_AREA_HEIGHT+LEFT_UPPER_CORNER, GREEN);
    
    rect(SMALLER_FRAME, SMALLER_FRAME, screenWidth()-SMALLER_FRAME-1, screenHeight()-SMALLER_FRAME-1, YELLOW);
    rect(BIGGER_FRAME, BIGGER_FRAME, screenWidth()-BIGGER_FRAME-1, screenHeight()-BIGGER_FRAME-1, CYAN);
}

void ZeroArray()
{
    int CurrentX, CurrentY;
    for(CurrentX=0; CurrentX<AREA_WIDTH; CurrentX++)
    {
        for(CurrentY=0; CurrentY<AREA_HEIGHT; CurrentY++)
        {
            GameArea[CurrentX][CurrentY]=0;
        }
    }
}

void DrawCells()
{
    int CurrentX, CurrentY;
    
    filledRect(0, 0, screenWidth() - 1, screenHeight() - 1, BLACK);
    GameScreenNet();
    
    for(CurrentX=0; CurrentX<AREA_WIDTH; CurrentX++)
    {
        for(CurrentY=0; CurrentY<AREA_HEIGHT; CurrentY++)
        {
            if(GameArea[CurrentX][CurrentY]==1)
            {
                filledRect(LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*CurrentX), LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*CurrentY), LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*(CurrentX+1)), LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*(CurrentY+1)), CYAN);
            }
            if(GameArea[CurrentX][CurrentY]==2)
            {
                filledRect(LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*CurrentX), LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*CurrentY), LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*(CurrentX+1)), LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*(CurrentY+1)), GREEN);
            }
        }
    }
}

void PrintTableInPrintf()
{
    int CurrentX, CurrentY;
    
    for(CurrentY=0; CurrentY<AREA_HEIGHT+1; CurrentY++)
    {
        for(CurrentX=0; CurrentX<AREA_WIDTH; CurrentX++)
        {
            printf("%d ",GameArea[CurrentX][CurrentY]);
        }
        printf("\n");
    }
    printf("--------------\n");
}

void CheckDoneLevel()
{
    int CurrentX, CurrentY, CurrentX2, CurrentY2;
    int Correct=0;
    for(CurrentY=0; CurrentY<AREA_HEIGHT; CurrentY++)
    {
        Correct=0;
        for(CurrentX=0; CurrentX<AREA_WIDTH; CurrentX++)
        {
            if(GameArea[CurrentX][CurrentY]!=0)
            {
                Correct++;
                if(Correct==AREA_WIDTH)
                {
                    for(CurrentY2=CurrentY; CurrentY2>0; CurrentY2--)
                    {
                        for(CurrentX2=0; CurrentX2<AREA_WIDTH; CurrentX2++)
                        {
                            filledRect(LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*CurrentX2), LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*CurrentY), LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*(CurrentX2+1)), LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*(CurrentY+1)), YELLOW);
                            updateScreen();
                            //GameArea[CurrentX2][CurrentY2]=GameArea[CurrentX2][CurrentY2-1];
                        }
                        SDL_Delay(DELAY_DROP);
                        for(CurrentX2=0; CurrentX2<AREA_WIDTH; CurrentX2++)
                        {
                            GameArea[CurrentX2][CurrentY2]=GameArea[CurrentX2][CurrentY2-1];
                        }
                    }
                    Level++;
                    DrawCells();
                    updateScreen();
                }
            }
        }
    }
}

int CheckExceedRight(int CurrentPiece, int CurrentRotation)
{
    int CurrentX, CurrentY;
    int MaxWidth=0;
    int CurrentWidth=0;
    for(CurrentX=0; CurrentX<4; CurrentX++)
        {
            for(CurrentY=0; CurrentY<4; CurrentY++)
            {
                if(pieces[CurrentPiece][CurrentRotation%4][CurrentX][CurrentY] != 0)
                {
                    CurrentWidth=CurrentX+1;
                    if(MaxWidth<CurrentWidth) MaxWidth=CurrentWidth;
                }
            }
        }
        return MaxWidth;
}

void DrawNextPieceInfo(int NextPiece)
{

    
    char LevelText[LEVELTEXT_SIZE];
    
    snprintf(LevelText, LEVELTEXT_SIZE, "Level: %d", Level);
    
    textout( INFO_POSITION_X, INFO_LEVEL_Y, LevelText, WHITE);
    
    char ScoreText[SCORETEXT_SIZE];
    
    snprintf(ScoreText, SCORETEXT_SIZE, "Score: %d", Score);
    
    textout( INFO_POSITION_X, INFO_SCORE_Y, ScoreText, WHITE);
    
    textout(INFO_POSITION_X, INFO_NEXT_PIECE_Y,"Next piece:",WHITE);
    
    int CurrentX, CurrentY;
    
    for(CurrentX=0; CurrentX<4; CurrentX++)
    {
        for(CurrentY=0; CurrentY<4; CurrentY++)
        {
            if(pieces[NextPiece][0][CurrentX][CurrentY] != 0)
                {
                    filledRect(INFO_POSITION_X+(SIZE_OF_SINGLE_BLOCK*CurrentX), INFO_DISPLAYED_PIECE_Y+(SIZE_OF_SINGLE_BLOCK*CurrentY), INFO_POSITION_X+(SIZE_OF_SINGLE_BLOCK*(CurrentX+1)), INFO_DISPLAYED_PIECE_Y+(SIZE_OF_SINGLE_BLOCK*(CurrentY+1)), CYAN);
                }
        }
    }
    

    textout(INSTRUCTION_INFO_X, INSTRUCTION_INFO_Y,"Game instruction:",GREEN);
    textout(INSTRUCTION_INFO_X, INSTRUCTION_INFO_Y+INSTRUCTION_INFO_EACH_LINE_Y,"UP - rotate", WHITE);
    textout(INSTRUCTION_INFO_X, INSTRUCTION_INFO_Y+2*INSTRUCTION_INFO_EACH_LINE_Y,"DOWN - speed up (score x2)",WHITE);
    textout(INSTRUCTION_INFO_X, INSTRUCTION_INFO_Y+3*INSTRUCTION_INFO_EACH_LINE_Y,"LEFT/RIGHT - move left/right",WHITE);
    textout(INSTRUCTION_INFO_X, INSTRUCTION_INFO_Y+4*INSTRUCTION_INFO_EACH_LINE_Y,"SPACE - drop block (score x3)",WHITE);
    
    updateScreen();
}

void LoseScreen()
{
    char LevelText[LEVELTEXT_SIZE];
    
    snprintf(LevelText, LEVELTEXT_SIZE, "Your level: %d", Level);
    
    char ScoreText[SCORETEXT_SIZE];
    
    snprintf(ScoreText, SCORETEXT_SIZE, "Score: %d", Score);
    
    filledRect(0, 0, screenWidth() - 1, screenHeight() - 1, BLACK);
    
    int CurrentX;
    for(CurrentX=0; CurrentX<PATTERN_MAX; CurrentX=CurrentX+BIGGER_FRAME)
    {
        rect(SMALLER_FRAME+CurrentX, SMALLER_FRAME+CurrentX, screenWidth()-SMALLER_FRAME-1-CurrentX, screenHeight()-SMALLER_FRAME-1-CurrentX, YELLOW);
        updateScreen();
        SDL_Delay(DELAY_PATTERN);
    
    }
    
    
    textout(screenWidth()/2-GAME_OVER_X, screenHeight()/2-GAME_OVER_EACH_Y*2, "YOU LOST!", WHITE);
    
    textout(screenWidth()/2-GAME_OVER_X, screenHeight()/2-GAME_OVER_EACH_Y, ScoreText, CYAN);
    
    textout(screenWidth()/2-GAME_OVER_X, screenHeight()/2, LevelText, GREEN);
    
    textout(screenWidth()/2-GAME_OVER_X, screenHeight()/2+GAME_OVER_EACH_Y, "Press R to restart.", WHITE);
    
    textout(screenWidth()/2-GAME_OVER_X, screenHeight()/2+GAME_OVER_EACH_Y*2, "Press any other key to exit.", WHITE);
    
    updateScreen();
    
}

int MoveLeft(int CurrentPiece, int CurrentRotation, int AddHeight, int MoveHorizontal)
{
    int CurrentX, CurrentY;
    int LeftSide[4] = {0, 0, 0, 0};
    
    for(CurrentY=0; CurrentY<4; CurrentY++)
    {
        for(CurrentX=0; CurrentX<4; CurrentX++)
        {
            if(pieces[CurrentPiece][CurrentRotation%4][CurrentX][CurrentY] != 0)
                {
                    LeftSide[CurrentY]=CurrentX+1;
                    //printf("\nLeftSide[%d]=%d",CurrentY,CurrentX+1);
                    break;
                }
        }
    }

    int CurrentLevel;
    
    
    for(CurrentLevel=0; CurrentLevel<4; CurrentLevel++)
    {
        if((AREA_WIDTH/2-1)+LeftSide[CurrentLevel]-2+MoveHorizontal >= 0 && LeftSide[CurrentLevel]!=0)
        {
            if(GameArea[(AREA_WIDTH/2-1)+LeftSide[CurrentLevel]-2+MoveHorizontal][CurrentLevel+AddHeight] != 0) 
            {
                return 1;
            }
        }
    }
    /*printf("Left side: \n");
    int i;
    for(i=0;i<4;i++) printf("%d \n", LeftSide[i]);*/
    
    return 0;
}

int MoveRight(int CurrentPiece, int CurrentRotation, int AddHeight, int MoveHorizontal)
{
    int CurrentX, CurrentY;
    int RightSide[4] = {0, 0, 0, 0};
    
    for(CurrentY=0; CurrentY<4; CurrentY++)
    {
        for(CurrentX=3; CurrentX>=0; CurrentX--)
        {
            if(pieces[CurrentPiece][CurrentRotation%4][CurrentX][CurrentY] != 0)
                {
                    RightSide[CurrentY]=CurrentX+1;
                    break;
                }
        }
    }
    
    int CurrentLevel;
    for(CurrentLevel=0; CurrentLevel<4; CurrentLevel++)
    {
        if(RightSide[CurrentLevel] !=0 && (AREA_WIDTH/2-1)+RightSide[CurrentLevel]+MoveHorizontal < AREA_WIDTH )
        {
            //ColorBlockToCheckHim((AREA_WIDTH/2-1)+RightSide[CurrentLevel]+MoveHorizontal, CurrentLevel+AddHeight);
            
                if(GameArea[(AREA_WIDTH/2-1)+RightSide[CurrentLevel]+MoveHorizontal][CurrentLevel+AddHeight] != 0) 
                {
                return 1;
                }
        }
    }
    /*printf("Right side: \n");
    int i;
    for(i=0;i<4;i++) printf("%d \n", RightSide[i]);*/
    
    return 0;
}

void ChooseCorrectSizeOfBlock()
{
    
  if(AREA_WIDTH>FIRST_CORRECT_AREA_SIZE || AREA_HEIGHT>SECOND_CORRECT_AREA_SIZE)
    SIZE_OF_SINGLE_BLOCK=SECOND_CORRECT_AREA_SIZE;
  if(AREA_WIDTH>SECOND_CORRECT_AREA_SIZE || AREA_HEIGHT>THIRD_CORRECT_AREA_SIZE)
    SIZE_OF_SINGLE_BLOCK=FIRST_CORRECT_AREA_SIZE;
}

void ColorBlockToCheckHim(int CurrentX, int CurrentY)
{
    filledRect(LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*CurrentX), LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*CurrentY), LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*(CurrentX+1)), LEFT_UPPER_CORNER+(SIZE_OF_SINGLE_BLOCK*(CurrentY+1)), WHITE);
    
    updateScreen();
    
    getkey();
}

int CheckTopAndBottom(int CurrentPiece, int CurrentRotation, int AddHeight, int MoveHorizontal)
{
    return 0;
}


int CheckWhereIs2X(char CurrentPiece[4][4])
{
    int X2;
    int CurrentX, CurrentY;
    
    for(CurrentY=0; CurrentY<4; CurrentY++)
    {
        for(CurrentX=0; CurrentX<4; CurrentX++)
        {
            if(CurrentPiece[CurrentX][CurrentY] == 2)
                {
                    X2 = CurrentX;
                    break;
                }
        }
    }
    
    return X2;
}

int CheckWhereIs2Y(char CurrentPiece[4][4])
{
    int Y2;
    int CurrentX, CurrentY;
    
    for(CurrentY=0; CurrentY<4; CurrentY++)
    {
        for(CurrentX=0; CurrentX<4; CurrentX++)
        {
            if(CurrentPiece[CurrentX][CurrentY] == 2)
                {
                    Y2 = CurrentY;
                    break;
                }
        }
    }
    
    return Y2;
}

int CheckDifferenceInPositionOf2X(char CurrentPiece[4][4], char NextPiece[4][4])
{
    int X2Current, X2Next;
    X2Current = CheckWhereIs2X(CurrentPiece);
    X2Next = CheckWhereIs2X(NextPiece);
    int Result = X2Current - X2Next;
    return Result;
}

int CheckDifferenceInPositionOf2Y(char CurrentPiece[4][4], char NextPiece[4][4])
{
    int Y2Current, Y2Next;
    Y2Current = CheckWhereIs2Y(CurrentPiece);
    Y2Next = CheckWhereIs2Y(NextPiece);
    int Result = Y2Current - Y2Next;
    return Result;
}

int CheckPosibilityOfRotation(int MoveHorizontal, int AddHeight)
{
    MoveHorizontal=MoveHorizontal+CorrectThePositionX;
    AddHeight=AddHeight+CorrectThePositionY;
    
    //GameArea[(AREA_WIDTH/2-1)+CurrentX+MoveHorizontal][CurrentY+AddHeight]
    
    if((AREA_WIDTH/2-1)+MoveHorizontal < 0 || (AREA_WIDTH/2-1)+3+MoveHorizontal > AREA_WIDTH
        || AddHeight < 0 || 3+AddHeight > AREA_HEIGHT)
        return 1;
    
    return 0;
}
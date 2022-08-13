#include "glut.h"
#include <time.h>
#include <vector>
#include <iostream>
#include "Cell.h"
#include <chrono>
#include <thread>

using namespace std;

const int MSZ = 100;  //maze size
const int WALL = 0;
const int SPACE = 1;
const int START = 2;
const int TARGET = 3;
const int BLACK = 4;
const int GRAY = 5;
const int PATH = 6;
const int VERY_DARK_GREY = 7;
const int DARK_GREY = 8;
const int FROM_TARGET = 100;
const int FROM_START = 101;
Cell* from_start_ptr;
Cell* from_target_ptr;
int maze[MSZ][MSZ] = { 0 };
int sss = 0, ttt = 0;
bool bfs_is_running = false;
bool dfs_is_running = false;
bool bidirectional_Search_running = false;
int tLocation;

//defines qeue of cells
vector <Cell*> graysT;
vector <Cell*> grays;
//Cell target;

void InitMaze() {
	int i, j;
	//the border is WALL by defalut
	for (i = 1; i < MSZ-1; i++)
	{
		for (j = 1; j < MSZ-1; j++)
		{
			if (i % 2 == 1) //more spaces
			{
				if (rand() % 100 < 85)
				{
					maze[i][j] = SPACE;
				}
				else
				{
					maze[i][j] = WALL;
				}
			}
			else //i is even more walls
			{
				if (rand() % 100 < 60)
				{
					maze[i][j] = WALL;
				}
				else
				{
					maze[i][j] = SPACE;
				}
			}
			
		}
	}
	maze[MSZ / 2][MSZ / 2] = START;
	//add start point to grays
	Cell* pc = new Cell(MSZ / 2, MSZ / 2);
	grays.push_back(pc);
	//add start point to graysT
	tLocation = rand() % MSZ;
	//int tLocation = MSZ / 2 + 10;
	Cell* pt = new Cell(tLocation, tLocation);
	graysT.push_back(pt);
	maze[tLocation][tLocation] = TARGET;

}

void DrawMaze() {
	int i, j;
	for (i = 0; i < MSZ; i++) 
	{
		for (j = 0; j < MSZ; j++) 
		{
			//set color
			switch (maze[i][j])
			{
			case WALL:
				glColor3d(0, 0, 0);// black
				break;
			case SPACE:
				glColor3d(1, 1, 1);// white
				break;
			case START:
				glColor3d(0.5, 0.7, 1);// blue light
				break;
			case TARGET:
				glColor3d(1, 0, 0);// red
				break;
			case BLACK:
				glColor3d(1, 0.5, 0);//drak green
				break;
			case GRAY:
				glColor3d(0, 0.5, 0);//light green
				break;
			case PATH:
				glColor3d(0.7, 0, 0.7);// purple
				break;
			case VERY_DARK_GREY:
				glColor3d(0.66, 0.66, 0.66);// dark grey
				break;
			case DARK_GREY:
				glColor3d(0.66, 1, 0.66);// dark grey
				break;
			}
			

			glBegin(GL_POLYGON);
			glVertex2d(j, i);
			glVertex2d(j, i+1);
			glVertex2d(j+1, i+1);
			glVertex2d(j+1, i);
			glEnd();
			
		}
	}
}

void init()
{
	srand(time(0)); //seed random numbers
	glClearColor(0, 0, 1, 0);// color of window background
	glOrtho(0, MSZ, 0, MSZ, -1, 1);//setup coordiante system
	InitMaze();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
	DrawMaze();
	/*
	
	*/
	glutSwapBuffers(); // show all
}

void RestorePath(Cell* pc) {
	
	while (pc->getParent() != nullptr)
	{
		maze[pc->getRow()][pc->getCol()] = PATH;
		pc = pc->getParent();
	}
}

void RestorePathBi()
{
	Cell* ps = from_start_ptr;
	Cell* pt = from_target_ptr;
	while (ps != nullptr || pt != nullptr)
	{
		if (ps != nullptr) {
			maze[ps->getRow()][ps->getCol()] = PATH;
			ps = ps->getParent();
		}
		if (pt != nullptr) {
			maze[pt->getRow()][pt->getCol()] = PATH;
			pt = pt->getParent();

		}
		maze[tLocation][tLocation] = TARGET;
		maze[MSZ /2][MSZ / 2] = START;

		display();
	}
}

Cell* search(int row, int col, vector<Cell*> cells) {
	for (Cell* c : cells) {
		if (c->getCol() == col && c->getRow() == row)
			return c;
	}
	return NULL;
}

void BiCheckNeighbor(Cell* pCurrent, int row, int col, vector<Cell*>* cells, int mode)
{
	switch (mode)
	{
	case FROM_TARGET:
		if (maze[row][col] == GRAY || maze[row][col] == BLACK) // The algorithm is over
		{
			cout << "the solution has been found/n";
			bfs_is_running = false;
			dfs_is_running = false;
			bidirectional_Search_running = false;
			from_start_ptr = search(row, col, grays);
			RestorePathBi();
		}
		else
		{
			maze[row][col] = DARK_GREY;
			Cell* pc = new Cell(row, col, pCurrent);
			cells->push_back(pc);
		}
		break;
	case FROM_START:
		if (maze[row][col] == DARK_GREY || maze[row][col] == VERY_DARK_GREY) // The algorithm is over
		{
			bfs_is_running = false;
			dfs_is_running = false;
			bidirectional_Search_running = false;
			cout << "The solution has been found" << endl;
			from_target_ptr = search(row, col, graysT);
			RestorePathBi();
		}
		else
		{
			maze[row][col] = GRAY;
			Cell* pc = new Cell(row, col, pCurrent);
			cells->push_back(pc);
		}
		break;
	default:
		break;
	}

}

void  checkNeighbor(int row, int col, Cell* pCurr,int sOrT)
{
	Cell* pneig;

	if (maze[row][col] == TARGET)
	{
		cout << "the solution has been found/n";
		bfs_is_running = false;
		dfs_is_running = false;
		bidirectional_Search_running = false;
		RestorePath(pCurr);
		maze[MSZ / 2][MSZ / 2] = START;
	}
	else //this is white 
	{
		pneig = new Cell(row, col, pCurr);//abd pcurrent as parent
		grays.push_back(pneig);
		maze[row][col] = GRAY;
	}
}

Cell* bidirectionalSearch(vector<Cell*>* cells, int mode)
{
	Cell* pCurrent;

	// 1. check if cells [queue] is not empty
	if (cells->empty())
	{
		cout << "there is no solution/n";
		bidirectional_Search_running = false;
		return nullptr;
	}
	else // cells is not empty
	{
		// 2. extract the FIRST element from cells and paint it
		pCurrent = *cells->begin();
		if (mode == FROM_START) {
			from_start_ptr = pCurrent;
		}
		else {
			from_target_ptr = pCurrent;
		}
		// 2.1 paint it BLACK
		int row, col;
		// 2.2 and remove it from cells
		cells->erase(cells->begin());
	
		row = pCurrent->getRow();
		col = pCurrent->getCol();
		int color = mode == FROM_START ? BLACK : VERY_DARK_GREY;
		maze[row][col] = color;
		// 3 check the neighbors of pCurrent and pick and add them to the end of cells
		// UP
		int target = mode == FROM_START ? DARK_GREY : GRAY;
	
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == target)
			BiCheckNeighbor(pCurrent, row + 1, col, cells, mode);
		// DOWN
		if (bidirectional_Search_running)
			if (maze[row - 1][col] == SPACE || maze[row - 1][col] == target)
				BiCheckNeighbor(pCurrent, row - 1, col, cells, mode);
		// right		
		if (bidirectional_Search_running)
			if (maze[row][col + 1] == SPACE || maze[row][col + 1] == target)
				BiCheckNeighbor(pCurrent, row, col + 1, cells, mode);
		// left		
		if (bidirectional_Search_running)
			if (maze[row][col - 1] == SPACE || maze[row][col - 1] == target)
				BiCheckNeighbor(pCurrent, row, col - 1, cells, mode);
	}
}

void DfsIteration() {
	Cell* pCurr;
	int r, c;
	if (grays.empty()) {
		cout << "there is no solution/n";
		dfs_is_running = false;
		return;
	}
	else //grays is not empty
	{ //pick the last elemnt from grays
		pCurr = grays.back();//get the last element
		grays.pop_back();//remove the last element
		r = pCurr->getRow();
		c = pCurr->getCol();
		maze[r][c] = BLACK;
		//choose random direction (in DFS random direcation is important)
		int direction[4] = { -1,-1,-1,-1 };
		int place;
		for(int i=0;i<4;i++)
		{	
			 //insert to direction vlaues from 0-3 (i)
			do 
			{
				place = rand() % 4; // 0\1\2\3
			} while (direction[place]!=-1);
			direction[place] = i;
		}

		//pick nighe in random order write in direction
		for (int i = 0; i < 4 && dfs_is_running ; i++) 
		{
			switch (direction[i])
			{
			case 0: //up
				if (maze[r + 1][c] == SPACE || maze[r + 1][c] == TARGET)
				{
					checkNeighbor(r + 1, c, pCurr,1);
				}
				break;
			case 1: //down
				if (maze[r -1 ][c] == SPACE || maze[r -1 ][c] == TARGET)
				{
					checkNeighbor(r -1 , c, pCurr,1);
				}
				break;
			case 2://left
				if (maze[r][c-1] == SPACE || maze[r][c-1] == TARGET)
				{
					checkNeighbor(r, c-1, pCurr,1);
				}
				break;
			case 3://right
				if (maze[r][c + 1] == SPACE || maze[r][c + 1] == TARGET)
				{
					checkNeighbor(r, c + 1, pCurr,1);
				}
				break;
			}
		}
	}
}

void BfsIteration()
{
	Cell* pCurrC;
	int r, c;
	if (grays.empty())
	{
		cout << "there is no solution/n";
		bfs_is_running = false;
		return;
	}
	else //grays is not empty
	{
		pCurrC = *grays.begin();
		// remove pCurr from grays and paint it black
		grays.erase(grays.begin());
		r = pCurrC->getRow();
		c = pCurrC->getCol();
		maze[r][c] = BLACK;
		//add non-visited neighbors
		// up
		if (maze[r + 1][c] == SPACE || maze[r + 1][c] == TARGET)
		{
			checkNeighbor(r+1, c, pCurrC,1);
		}
		// down
		 if (bfs_is_running && maze[r - 1][c] == SPACE || maze[r - 1][c] == TARGET)
		{
			checkNeighbor(r - 1, c, pCurrC,1);
		}
		// left 
		 if (bfs_is_running && maze[r][c-1] == SPACE || maze[r][c-1] == TARGET)
		 {
			 checkNeighbor(r , c-1, pCurrC,1);
		 }
		// right
		 if (bfs_is_running && maze[r][c + 1] == SPACE || maze[r][c + 1] == TARGET)
		 {
			 checkNeighbor(r, c + 1, pCurrC,1);
		 }
	}
}

void idle()
{
	//gx -= 0.0001;//change the gx value
	if (bfs_is_running)
		BfsIteration();
		
	if(dfs_is_running)
		DfsIteration();

	if (bidirectional_Search_running) {
		bidirectionalSearch(&grays, FROM_START);
		bidirectionalSearch(&graysT, FROM_TARGET);
	}
	glutPostRedisplay();//calls indirectly to display
}

void menu(int choice) { 
	switch (choice)
	{
	case 1:
		bfs_is_running = true;
		break;
	case 2:
		dfs_is_running = true;
		break;
	case 3:
		bidirectional_Search_running = true;
		break;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);// double means its uses double buffer we print in the buffer
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("BFS");

	glutDisplayFunc(display);//refresh function
	glutIdleFunc(idle); //runs all the time in the background

	glutCreateMenu(menu);
	glutAddMenuEntry("Run BFS", 1);
	glutAddMenuEntry("Run DFS", 2);
	glutAddMenuEntry("Run Bidirectional Search", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);


	init();

	glutMainLoop();
}
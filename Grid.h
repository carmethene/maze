//------------------------------------------------------------------------------
// File: Grid.h
// Desc: Game grid with A* solver
//
// Created: 05 December 2002 12:52:14
//
// (c)2002 Neil Wakefield
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_GRID_H
#define INCLUSIONGUARD_GRID_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Constants.h"

#include <vector>
#include <list>
#include <string>
using namespace std;


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Name: struct MazeSquare
// Desc: Representation of a single square in the maze
//------------------------------------------------------------------------------
struct MazeSquare
{
	MazeSquare() : x( 0 ), y( 0 ) {}
	MazeSquare( const int newX, const int newY ) : x( newX ), y( newY ) {}

	int x, y;
};

//------------------------------------------------------------------------------
// Name: struct StateNode
// Desc: Single node in the A* algorithm
//------------------------------------------------------------------------------
struct StateNode
{
	StateNode() : parent( NULL ), nodeCost( 0 ), x( 0 ), y( 0 ), totalCost( 0 ) {}
	StateNode( StateNode* newParent, const int newX, const int newY,
			   const int newNodeCost, const int newTotalCost )
	{
		parent = newParent;
		x = newX;
		y = newY;
		nodeCost = newNodeCost;
		totalCost = newTotalCost;
	}
		
	StateNode* parent;
	int nodeCost;
	int totalCost;
	int x;
	int y;
};

//------------------------------------------------------------------------------
// Name: class Grid
// Desc: The game grid with an A* solver
//------------------------------------------------------------------------------
class Grid
{

public:
	Grid();

	const static enum GridTile { TILE_PATH, TILE_WALL, TILE_START, TILE_GOAL };
	const static enum Direction { DIR_UP, DIR_RIGHT, DIR_DOWN, DIR_LEFT };
	const static int GRID_SIZE = CONST_GRID_SIZE;

	GridTile GetTile( const int x, const int y ) const { return m_tiles[ y ][ x ]; }
	void SetTile( const int x, const int y, const GridTile value )
	{
		m_tiles[ y ][ x ] = value;
	}

	int GetStartY() { return m_startY; }
	int GetGoalY() { return m_goalY; }

	void GenerateMaze();
	void GenerateSolution();

	bool PopNextSolution( MazeSquare& square );

private:
	GridTile m_tiles[ GRID_SIZE ][ GRID_SIZE ];

	int m_startY;
	int m_goalY;

	list< MazeSquare > m_solution;

	bool m_solved;

	bool IsPath( const int x, const int y );
	bool CanPlacePath( const int x, const int y );
	void MoveInDirection( int& x, int& y, const Direction d );
	int SearchHeuristic( const int x, const int y );

};


#endif //INCLUSIONGUARD_GRID_H

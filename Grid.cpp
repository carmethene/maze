//------------------------------------------------------------------------------
// File: Grid.cpp
// Desc: Game grid with A* solver
//
// Created: 05 December 2002 12:58:41
//
// (c)2002 Neil Wakefield
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Grid.h"

#include "time.h"


//------------------------------------------------------------------------------
// Globals:
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Name: Grid()
// Desc: Constructor for the game grid object
//------------------------------------------------------------------------------
Grid::Grid()
{
	m_solved = false;
	m_solution.clear();
}

//------------------------------------------------------------------------------
// Name: GenerateMaze()
// Desc: Generates a maze using a path-growing algorithm
//------------------------------------------------------------------------------
void Grid::GenerateMaze()
{
	m_solved = false;

	srand( unsigned int( time( NULL ) ) );

	vector< MazeSquare > path;	//list of all path squares so far

	bool foundExit = false; //have we found an exit

	//fill the entire grid with wall squares
	for( int y = 0; y < GRID_SIZE; ++y )
	{
		for( int x = 0; x < GRID_SIZE; ++x )
		{
			SetTile( x, y, TILE_WALL );
		}
	}

	//pick a random starting point in the left hand column
	m_startY = rand() % GRID_SIZE;
	SetTile( 0, m_startY, TILE_START );

	//record this square
	MazeSquare square;
	square.y = m_startY;
	square.x = 0;
	path.push_back( square );

	//now grow the paths from here
	while( 1 )
	{
		if( 0 == path.size() )
		{
			//no more elements left to expand from, this shouldn't actually happen
			break;
		}

		//take a random square from the path list
		vector< MazeSquare >::iterator currentSquare = path.begin();
		advance( currentSquare, rand() % int( path.size() ) );

		square = *currentSquare;
		int x = square.x;
		int y = square.y;

		//pick a random direction to move in from this square
		Direction d = Direction( rand() % 4 );
		int newD = int( d );

		//see if this square is valid
		MoveInDirection( x, y, d );
		if( ! CanPlacePath( x, y ) )
		{
			//try next direction
			x = square.x;
			y = square.y;
			int newd = int( d );
			++newd; newd %= 4; d = Direction( newd );
			MoveInDirection( x, y, d );
			
			if( ! CanPlacePath( x, y ) )
			{
				//try next direction
				x = square.x;
				y = square.y;
				++newd; newd %= 4; d = Direction( newd );
				MoveInDirection( x, y, d );
				
				if( ! CanPlacePath( x, y ) )
				{
					//try next direction
					x = square.x;
					y = square.y;
					++newd; newd %= 4; d = Direction( newd );
					MoveInDirection( x, y, d );
					if( ! CanPlacePath( x, y ) )
					{
						//all four directions tried, so this path is useless
						//remove it from the places-to-try list
						path.erase( currentSquare );
						continue;
					}
				}
			}
		}

		//if we get this far, x,y should contain the location of the next sqaure to
		// add to the path
		SetTile( x, y, TILE_PATH );
		square.x = x;
		square.y = y;
		path.push_back( square );

		//see if this path is a valid exit
		if( x == ( GRID_SIZE - 1 ) )
		{
			//if we haven't already found an exit, use this one
			if( ! foundExit )
			{
				//set this tile to the goal
				SetTile( x, y, TILE_GOAL );
				m_goalY = y;

				foundExit = true;

				//the maze is complete, but continue until all possible squares have been
				//used so we don't end up with large areas of wall
			}
		}
	}
}

//------------------------------------------------------------------------------
// Name: CanPlacePath()
// Desc: Checks to see if putting a path at a given point would break the maze
//------------------------------------------------------------------------------
bool Grid::CanPlacePath( const int x, const int y )
{
	//make sure the square is inside the bounds of the grid
	if( ( x < 0 ) || ( x >= GRID_SIZE ) ||
		( y < 0 ) || ( y >= GRID_SIZE ) )
		return false;

	//check to see if this square is already a path
	if( TILE_WALL != GetTile( x, y ) )
		return false;

	//check to see if this square would cause a loop
	// this would happen if the square was touching two or more path squares
	bool foundOneSquare = false;

	//track paths found - we need these to constrain diagonals
	bool foundLeft = false;
	bool foundRight = false;
	bool foundUp = false;
	bool foundDown = false;

	bool minX = ( x <= 0 );
	bool maxX = ( x >= ( GRID_SIZE - 1 ) );
	bool minY = ( y <= 0 );
	bool maxY = ( y >= ( GRID_SIZE - 1 ) );

	//up
	if( !minY )
		if( TILE_WALL != GetTile( x, y - 1 ) )
		{
			foundOneSquare = true;
			foundUp = true;
		}

	//right
	if( !maxX )
		if( TILE_WALL != GetTile( x + 1, y ) )
			if( foundOneSquare )
				return false;
			else
			{
				foundOneSquare = true;
				foundRight = true;
			}
		
	//down
	if( !maxY )
		if( TILE_WALL != GetTile( x, y + 1 ) )
			if( foundOneSquare )
				return false;
			else
			{
				foundOneSquare = true;
				foundDown = true;
			}

	//left
	if( !minX )
		if( TILE_WALL != GetTile( x - 1, y ) )
			if( foundOneSquare )
				return false;
			else
			{
				foundOneSquare = true;
				foundLeft = true;
			}

	//add some constraints to creating diagonals as this results in ugly mazes
	//upper-left
	if( !minY && !minX )
		if( TILE_WALL != GetTile( x - 1, y - 1 ) )
			if( ! foundLeft && ! foundUp )
				return false;
			else
				foundOneSquare = true;

	//upper-right
	if( !minY && !maxX )
		if( TILE_WALL != GetTile( x + 1, y - 1 ) )
			if( ! foundRight && ! foundUp )
				return false;
			else
				foundOneSquare = true;

	//lower-right
	if( !maxY && !maxX )
		if( TILE_WALL != GetTile( x + 1, y + 1 ) )
			if( ! foundRight && ! foundDown )
				return false;
			else
				foundOneSquare = true;

	//lower-left
	if( !minX && !maxY )
		if( TILE_WALL != GetTile( x - 1, y + 1 ) )
			if( ! foundLeft && ! foundDown )
				return false;
			else
				foundOneSquare = true;

	//must be a valid place to put a path
	return true;
}

//------------------------------------------------------------------------------
// Name: MoveInDirection()
// Desc: Moves along the grid one unit in the specified direction
//------------------------------------------------------------------------------
void Grid::MoveInDirection( int& x, int& y, const Direction d )
{
	if( d == DIR_UP )
		y++;
	else if( d == DIR_DOWN )
		y--;
	else if( d == DIR_LEFT )
		x--;
	else if( d == DIR_RIGHT )
		x++;
}

//------------------------------------------------------------------------------
// Name: GenerateSolution()
// Desc: Finds the path through the maze
//------------------------------------------------------------------------------
void Grid::GenerateSolution()
{
	m_solution.clear();

	vector< StateNode* > openList;
	vector< StateNode* > closedList;
	vector< StateNode* >::iterator iter;
	vector< StateNode* >::iterator currentNode;
	StateNode* pNode = NULL;

	StateNode* states[ GRID_SIZE ][ GRID_SIZE ];
	memset( states, 0, GRID_SIZE * GRID_SIZE * sizeof( StateNode* ) );

	//create the start node
	pNode = new StateNode( NULL, 0, m_startY, 0, 0 );
	states[ m_startY ][ 0 ] = pNode;
	openList.push_back( pNode );
	pNode = NULL;

	while( openList.size() != 0 )	//there are still nodes on the open list
	{
        //find the node on the open list with the lowest total cost
		currentNode = ( iter = openList.begin() );

		int tc = (*iter)->totalCost;
		while( iter != openList.end() )
		{
			if( (*iter)->totalCost < tc )
			{
				currentNode = iter;
				tc = (*iter)->totalCost;
			}
			++iter;
		}
		pNode = *currentNode;

		//if the current node is the same as the goal, we have finished
		if( (*currentNode)->x == ( GRID_SIZE - 1 ) &&
			(*currentNode)->y == m_goalY )
		{
			m_solved = true;
			break;
		}

		//move the node to the closed list
		openList.erase( currentNode );
		closedList.push_back( pNode );

		//for all valid neighbouring nodes...
		for( int i = 0; i < 4; ++i )
		{
			int x = pNode->x;
			int y = pNode->y;
			Direction d = Direction( i );
            MoveInDirection( x, y, d );

			if( IsPath( x, y ) )
			{
                //look through the open and closed lists to see if this point already exists
				// in a more efficient form, replacing less efficient ones
				bool skipThisNode = false;

				//open
				iter = openList.begin();
				while( iter != openList.end() )
				{
					if( (*iter)->x == x && (*iter)->y == y )
					{
						if( (*iter)->nodeCost <= ( (*currentNode)->nodeCost + 1 ) )
						{
                            //old node is better so skip this one
							skipThisNode = true;
						}
						break;
					}

					++iter;
				}
				if( skipThisNode )
					continue;

				//closed
				iter = closedList.begin();
				while( iter != closedList.end() )
				{
					if( (*iter)->x == x && (*iter)->y == y )
					{
						if( (*iter)->nodeCost <= ( pNode->nodeCost + 1 ) )
						{
							//old node is better so skip this one
							skipThisNode = true;
						}
						else
						{
							//newer node is better, remove this one from the closed list and
							// resubmit it for checking
							StateNode* pTemp = *iter;
							closedList.erase( iter );
							openList.push_back( pTemp );
						}
						break;
					}
					++iter;
				}
				if( skipThisNode )
					continue;

				//now we have a new node to add to the open list
				if( states[ y ][ x ] == NULL )
				{
					states[ y ][ x ] = new StateNode( pNode, x, y,
													  pNode->nodeCost + 1,
                                                      pNode->nodeCost + 1 + SearchHeuristic( x, y ) );
					openList.push_back( states[ y ][ x ] );
				}
				else
				{
					//node already exists, but must be more inefficient so update it
					states[ y ][ x ]->parent = pNode;
					states[ y ][ x ]->nodeCost = pNode->nodeCost + 1;
					states[ y ][ x ]->totalCost = pNode->nodeCost + 1 + SearchHeuristic( x, y );
					states[ y ][ x ]->x = x;
					states[ y ][ x ]->y = y;                    				
				}
			}
		}
	}

	if( ! m_solved )
	{
		//something has gone horribly horribly wrong
		return;
	}

	//store the solution
	m_solution.push_front( MazeSquare( GRID_SIZE, m_goalY ) );
	while( pNode != NULL )
	{
		m_solution.push_front( MazeSquare( pNode->x, pNode->y ) );       		
		pNode = pNode->parent;
	}
	m_solution.push_front( MazeSquare( -1, m_startY ) );

	//clear the nodes from memory
	for( int j = 0; j < GRID_SIZE; ++j )
	{
		for( int i = 0; i < GRID_SIZE; ++i )
		{
            pNode = states[ j ][ i ];
			delete pNode;
			states[ j ][ i ] = NULL;
		}
	}
	openList.clear();
	closedList.clear();

}

//------------------------------------------------------------------------------
// Name: IsPath()
// Desc: Checks to see if a path exists at a given point on the grid
//------------------------------------------------------------------------------
bool Grid::IsPath( const int x, const int y )
{
	//check if we're out of bounds
	if( x < 0 || x >= GRID_SIZE ||
		y < 0 || y >= GRID_SIZE )
		return false;

	//check to see if the space is a wall
	if( TILE_WALL == GetTile( x, y ) )
		return false;

	return true;
}

//------------------------------------------------------------------------------
// Name: SearchHeuristic()
// Desc: Heuristic function for the A* algorithm
//------------------------------------------------------------------------------
int Grid::SearchHeuristic( const int x, const int y )
{
	return abs( ( GRID_SIZE - 1 ) - x ) + abs( m_goalY - y );
}

//------------------------------------------------------------------------------
// Name: PopNextSolution()
// Desc: Returns the next point in the solution, or FALSE if there isn't one
//------------------------------------------------------------------------------
bool Grid::PopNextSolution( MazeSquare& square )
{
	//make sure we actually have a solution
	if( ! m_solved )
		return false;

	//make sure we have some points remaining in the path
	if( m_solution.size() == 0 )
		return false;

	//return the next point in the path
	list< MazeSquare >::iterator iter = m_solution.begin();
	square = (*iter);
	m_solution.pop_front();

	return true;
}
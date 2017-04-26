//------------------------------------------------------------------------------
// File: Player.h
// Desc: Object to represent a character moving through the grid
//
// Created: 05 December 2002 14:58:56
//
// (c)2002 Neil Wakefield
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_PLAYER_H
#define INCLUSIONGUARD_PLAYER_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Constants.h"

#include <d3dx8.h>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Name: class Player
// Desc: The player object
//------------------------------------------------------------------------------
class Player
{

public:
	Player();

	D3DXVECTOR3 GetPos() const { return m_vPosition; }

	void SetPosition( const float x, const float y )
	{
		m_vPosition = D3DXVECTOR3( x, y, 0.5f );
		m_hasReachedGoal = false;
	}

	void SetGoal( const float x, const float y )
	{
		m_vGoal = D3DXVECTOR3( x, y, 0.5 );
		m_hasReachedGoal = false;
	}

	bool HasReachedGoal() const { return m_hasReachedGoal; }
	void MovePlayer( const float timeInterval );

private:
	const static float PLAYER_SPEED;

	bool m_hasReachedGoal;

	D3DXVECTOR3 m_vPosition;
	D3DXVECTOR3 m_vGoal;
};

#endif //INCLUSIONGUARD_PLAYER_H

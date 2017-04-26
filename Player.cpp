//------------------------------------------------------------------------------
// File: Player.cpp
// Desc: Object to represent a character moving through the grid
//
// Created: 05 December 2002 15:00:04
//
// (c)2002 Neil Wakefield
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Player.h"
#include <d3dx8.h>


//------------------------------------------------------------------------------
// Globals:
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
const float Player::PLAYER_SPEED = CONST_PLAYER_SPEED;

//------------------------------------------------------------------------------
// Name: Player()
// Desc: Constructor for the player object
//------------------------------------------------------------------------------
Player::Player()
{
	m_hasReachedGoal = true;
	m_vGoal = D3DXVECTOR3( 0.0, 0.0, 0.0 );
	m_vPosition = D3DXVECTOR3( 0.0, 0.0, 0.0 );
}

//------------------------------------------------------------------------------
// Name: MovePlayer()
// Desc: Moves the player along a straight line towards the goal
//------------------------------------------------------------------------------
void Player::MovePlayer( const float timeInterval )
{
	//calculate the vector between the current point and the goal
	D3DXVECTOR3 direction;
	D3DXVec3Subtract( &direction, &m_vGoal, &m_vPosition );
	D3DXVECTOR3 movement;
	D3DXVec3Normalize( &movement, &direction );

    //scale it by the speed
	D3DXVec3Scale( &movement, &movement, PLAYER_SPEED * timeInterval );

	//if movement is greater than the distance to the point, we need the next goal
	if( D3DXVec3Length( &movement ) >= D3DXVec3Length( &direction ) )
	{
		m_hasReachedGoal = true;
		movement = direction;	//stop it overshooting the goal
	}

	//move the player along this vector
	D3DXVec3Add( &m_vPosition, &m_vPosition, &movement );
}
//------------------------------------------------------------------------------
// File: Pathfinder.h
// Desc: Program to generate a simple A* path solver
//
// Created: 05 December 2002 12:19:45
//
// (c)2002 Neil Wakefield
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_PATHFINDER_H
#define INCLUSIONGUARD_PATHFINDER_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include "Grid.h"
#include "Player.h"
#include "Constants.h"
#include "resource.h"

#include <windows.h>
#include <d3dx8.h>
#include <tchar.h>
#include "d3dapp.h"


//------------------------------------------------------------------------------
// Macros: 
//------------------------------------------------------------------------------
#define SAFE_RELEASE_VS(s) if(s!= 0xffffffff){m_pd3dDevice->DeleteVertexShader(s);s=0xffffffff;}
#define SAFE_RELEASE(x) if(x){x->Release();x=NULL;}


//-----------------------------------------------------------------------------
// Name: struct MESH_VERTEX
// Desc: A single vertex in a mesh
//-----------------------------------------------------------------------------
struct MESH_VERTEX
{
    D3DXVECTOR3 p; // Position
    D3DXVECTOR3 n; // Normal
};

//------------------------------------------------------------------------------
// Name: class App
// Desc: The main application class
//------------------------------------------------------------------------------
class App : public CD3DApplication
{
public:
	App();
	~App();

	HRESULT OneTimeSceneInit();
	HRESULT FinalCleanup();
	HRESULT InitDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT Render();
	HRESULT FrameMove();
	HRESULT ConfirmDevice( D3DCAPS8*, DWORD, D3DFORMAT );

private:
	void SetUpLights();

	Grid* m_pGrid;

	bool m_newMaze;

	bool m_chaseCam;

	Player* m_pPlayer;

	DWORD m_dwMeshFVF;

	LPD3DXMESH m_pPlayerMesh;
	LPDIRECT3DVERTEXBUFFER8	m_pPlayerVB;
	DWORD m_dwNumPlayerVertices;
	LPDIRECT3DINDEXBUFFER8	m_pPlayerIB;
	DWORD m_dwNumPlayerPrims;

	LPD3DXMESH m_pCubeMesh;
	LPDIRECT3DVERTEXBUFFER8	m_pCubeVB;
	DWORD m_dwNumCubeVertices;
	LPDIRECT3DINDEXBUFFER8	m_pCubeIB;
	DWORD m_dwNumCubePrims;

	D3DMATERIAL8 m_wallMaterial;
	D3DMATERIAL8 m_borderMaterial;
	D3DMATERIAL8 m_playerMaterial;
};

#endif //INCLUSIONGUARD_PATHFINDER_H

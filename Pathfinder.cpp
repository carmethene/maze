//------------------------------------------------------------------------------
// File: Pathfinder.cpp
// Desc: Program to generate a simple A* path solver
//
// Created: 05 December 2002 12:20:48
//
// (c)2002 Neil Wakefield
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files: 
//------------------------------------------------------------------------------
#include "Pathfinder.h"


//------------------------------------------------------------------------------
// Globals: 
//------------------------------------------------------------------------------
App* g_pApp = NULL;


//------------------------------------------------------------------------------
// Functions: 
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Name: App()
// Desc: Constructor for the application class
//------------------------------------------------------------------------------
App::App()
{
	//update the global pointer
	g_pApp = this;

	//enable z-buffering
	m_bUseDepthBuffer = TRUE;

	//start with chasecam disabled
	m_chaseCam = FALSE;

	//initialise buffer pointers
	m_pPlayerMesh	= NULL;
	m_pPlayerVB		= NULL;
	m_pPlayerIB		= NULL;
	m_pCubeMesh		= NULL;
	m_pCubeVB		= NULL;
	m_pCubeIB		= NULL;

	m_dwMeshFVF				= 0;
	m_dwNumPlayerVertices	= 0;
	m_dwNumPlayerPrims		= 0;
	m_dwNumCubeVertices		= 0;
	m_dwNumCubePrims		= 0;

	//create the game grid
	m_pGrid = new Grid();
	m_pGrid->GenerateMaze();
	m_pGrid->GenerateSolution();
	m_newMaze = true;

	//create the player
	m_pPlayer = new Player();
    MazeSquare square;
	float halfSize = Grid::GRID_SIZE / 2.0f;
    
	m_pGrid->PopNextSolution( square );
	m_pPlayer->SetPosition( square.x - halfSize, square.y - halfSize );
	m_pGrid->PopNextSolution( square );
	m_pPlayer->SetGoal( square.x - halfSize, square.y - halfSize );
}

//------------------------------------------------------------------------------
// Name: ~App()
// Desc: Destructor for the application class
//------------------------------------------------------------------------------
App::~App()
{
	//remove the game grid
	delete m_pGrid;
	m_pGrid = NULL;
	
	//update the global pointer
	g_pApp = NULL;
}

//------------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Checks the capabilities of the graphics card
//------------------------------------------------------------------------------
HRESULT App::ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehaviour, D3DFORMAT format )
{
	return S_OK;
}

//------------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Sets up application-specific data on first run
//------------------------------------------------------------------------------
HRESULT App::OneTimeSceneInit()
{
	return S_OK;
}

//------------------------------------------------------------------------------
// Name: InitDeviceObjects
// Desc: Sets up device-specific data on startup and device change
//------------------------------------------------------------------------------
HRESULT App::InitDeviceObjects()
{
	//create the grid cube mesh...
	D3DXCreateBox( m_pd3dDevice, 1.0f, 1.0f, 1.0f, &m_pCubeMesh, NULL );
	m_pCubeMesh->GetVertexBuffer( &m_pCubeVB );
	m_dwNumCubeVertices = m_pCubeMesh->GetNumVertices();
	m_pCubeMesh->GetIndexBuffer( &m_pCubeIB );
	m_dwNumCubePrims = m_pCubeMesh->GetNumFaces();

	//create the player model
	D3DXCreateSphere( m_pd3dDevice, CONST_PLAYER_SIZE, 20, 20, &m_pPlayerMesh, NULL );
	m_pPlayerMesh->GetVertexBuffer( &m_pPlayerVB );
	m_dwNumPlayerVertices = m_pPlayerMesh->GetNumVertices();
	m_pPlayerMesh->GetIndexBuffer( &m_pPlayerIB );
	m_dwNumPlayerPrims = m_pPlayerMesh->GetNumFaces();
	
	//get the FVF flags for the meshes
	m_dwMeshFVF = m_pPlayerMesh->GetFVF();

	//create the materials
	memset( &m_wallMaterial, 0, sizeof( D3DMATERIAL8 ) );
	m_wallMaterial.Diffuse.r = 1.0f;
	m_wallMaterial.Diffuse.g = 0.0f;
	m_wallMaterial.Diffuse.b = 0.0f;
	m_wallMaterial.Diffuse.a = 1.0f;

	memset( &m_borderMaterial, 0, sizeof( D3DMATERIAL8 ) );
	m_borderMaterial.Diffuse.r = 1.0f;
	m_borderMaterial.Diffuse.g = 1.0f;
	m_borderMaterial.Diffuse.b = 1.0f;
	m_borderMaterial.Diffuse.a = 1.0f;

	memset( &m_playerMaterial, 0, sizeof( D3DMATERIAL8 ) );
	m_playerMaterial.Diffuse.r = 1.0f;
	m_playerMaterial.Diffuse.g = 1.0f;
	m_playerMaterial.Diffuse.b = 0.0f;
	m_playerMaterial.Diffuse.a = 1.0f;
	
	return S_OK;
}

//------------------------------------------------------------------------------
// Name: RestoreDeviceObjects
// Desc: Sets up device-specific data on res change
//------------------------------------------------------------------------------
HRESULT App::RestoreDeviceObjects()
{
	//set up the camera
	D3DXMATRIX matView;
	D3DXVECTOR3 vEyePt		= D3DXVECTOR3( -0.5f, -0.5f, ( Grid::GRID_SIZE + 2 ) * -1.4f );
	D3DXVECTOR3 vLookAtPt	= D3DXVECTOR3( -0.5f, -0.5f, 0.0f );
	D3DXVECTOR3 vUp			= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookAtPt, &vUp );
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	D3DXMATRIX matProj;
	float fAspect = m_d3dsdBackBuffer.Width / float(m_d3dsdBackBuffer.Height);
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 500.0f );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );	

	//set render states
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );	//z-buffer on

	SetUpLights();

	return S_OK;
}

//------------------------------------------------------------------------------
// Name: SetUpLights()
// Desc: Sets up lighting for the scene
//------------------------------------------------------------------------------
void App::SetUpLights()
{
	//set up point lights
	D3DLIGHT8 d3dLight;
	memset( &d3dLight, 0, sizeof( D3DLIGHT8 ) );
	d3dLight.Type		= D3DLIGHT_POINT;
	d3dLight.Diffuse.r  = 0.9f;
	d3dLight.Diffuse.g  = 0.9f;
	d3dLight.Diffuse.b  = 0.9f;
	d3dLight.Position.x	= -0.5f;
	d3dLight.Position.y	= -0.5f;
	d3dLight.Position.z	= -1.0f * Grid::GRID_SIZE;
	d3dLight.Range		= 1000.0f;
	d3dLight.Attenuation0 = 1.0f;
	m_pd3dDevice->SetLight( 0, &d3dLight );
	m_pd3dDevice->LightEnable( 0, TRUE );

	//set render states
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );	//lighting on
	m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_XRGB( 0, 0, 0 ) );
}

//------------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the current frame
//------------------------------------------------------------------------------
HRESULT App::Render()
{
	//blank the screen
	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						 D3DCOLOR_ARGB(0xff,0,0,0), 1.0f, 0 );

	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
		//render the game grid
		m_pd3dDevice->SetVertexShader( m_dwMeshFVF );
		m_pd3dDevice->SetStreamSource( 0, m_pCubeVB, sizeof( MESH_VERTEX ) );
		m_pd3dDevice->SetIndices( m_pCubeIB, 0 );
		m_pd3dDevice->SetMaterial( &m_wallMaterial );

		//calculate half the grid size - this is needed for translating cubes
		float halfSize = Grid::GRID_SIZE / 2.0f;

		for( int y = 0; y < Grid::GRID_SIZE; ++y )
		{
			for( int x = 0; x < Grid::GRID_SIZE; ++x )
			{
				if( Grid::TILE_WALL == m_pGrid->GetTile( x, y ) )
				{
					//transform the cube to this point
					D3DXMATRIX matWorld;
					D3DXMatrixTranslation( &matWorld, x - halfSize, y - halfSize, 0.0f );
					m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

					//now render it
					m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0,
														m_dwNumCubeVertices, 0, 
														m_dwNumCubePrims );
				}
			}
		}

		//draw the grid border
		m_pd3dDevice->SetMaterial( &m_borderMaterial );		

		for( int x = -1; x <= Grid::GRID_SIZE; ++x )
		{
			//draw top and bottom borders
			D3DXMATRIX matWorld;
			D3DXMatrixTranslation( &matWorld, x - halfSize, -1 - halfSize, 0.0f );
			m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
			m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumCubeVertices, 0, 
												m_dwNumCubePrims );

			D3DXMatrixTranslation( &matWorld, x - halfSize, halfSize, 0.0f );
			m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
			m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumCubeVertices, 0, 
												m_dwNumCubePrims );
		}
		for( int y = -1; y <= Grid::GRID_SIZE; ++y )
		{
			//draw left and right borders
			D3DXMATRIX matWorld;
			if( y != m_pGrid->GetStartY() )
			{
				D3DXMatrixTranslation( &matWorld, -1 - halfSize, y - halfSize, 0.0f );
				m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
				m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumCubeVertices, 0, 
													m_dwNumCubePrims );
			}
			if( y != m_pGrid->GetGoalY() )
			{
				D3DXMatrixTranslation( &matWorld, halfSize, y - halfSize, 0.0f );
				m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
				m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumCubeVertices, 0, 
													m_dwNumCubePrims );
			}
		}

		//draw the player
		m_pd3dDevice->SetStreamSource( 0, m_pPlayerVB, sizeof( MESH_VERTEX ) );
		m_pd3dDevice->SetIndices( m_pPlayerIB, 0 );
		m_pd3dDevice->SetMaterial( &m_playerMaterial );

		D3DXVECTOR3 playerPos = m_pPlayer->GetPos();
		D3DXMATRIX matWorld;
		D3DXMatrixTranslation( &matWorld, playerPos[ 0 ], playerPos[ 1 ], 0.0f );
		m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
		m_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, m_dwNumPlayerVertices,
											0, m_dwNumPlayerPrims );

		//finished
		m_pd3dDevice->EndScene();
	}

	return S_OK;
}

//------------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Performs between-frame animation
//------------------------------------------------------------------------------
HRESULT App::FrameMove()
{
	if( GetKeyState( 49 ) & 0x8000 )	//1 key
	{
		//side-on view
		m_chaseCam = false;
		D3DXMATRIX matView;
		D3DXVECTOR3 vEyePt		= D3DXVECTOR3( -0.5f, -0.5f, ( Grid::GRID_SIZE + 2 ) * -1.4f );
		D3DXVECTOR3 vLookAtPt	= D3DXVECTOR3( -0.5f, -0.5f, 0.0f );
		D3DXVECTOR3 vUp			= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookAtPt, &vUp );
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	}
	else if( GetKeyState( 50 ) & 0x8000 ) //2 key
	{
		//chase-cam
		m_chaseCam = true;
	}

	//if we are using the chase-cam, position the camera accordingly
	if( m_chaseCam )
	{
        D3DXVECTOR3 vec = m_pPlayer->GetPos();
		D3DXMATRIX matView;
		D3DXVECTOR3 vEyePt		= D3DXVECTOR3( vec[ 0 ] - 8.0f,
											   vec[ 1 ] - 8.0f,
											   vec[ 2 ] - 10.0f );
		D3DXVECTOR3 vLookAtPt	= vec;
		D3DXVECTOR3 vUp			= D3DXVECTOR3( 0.0f, 0.0f, -1.0f );
		D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookAtPt, &vUp );
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	}

	if( m_newMaze )
	{
		//prevent the large time interval after maze generation from affecting motion
		m_fElapsedTime = 0.0f;
		m_newMaze = false;
	}

	//check to see if we need to get the next goal point for the player
	if( m_pPlayer->HasReachedGoal() )
	{
		float halfSize = Grid::GRID_SIZE / 2.0f;
		MazeSquare square;
		if( ! m_pGrid->PopNextSolution( square ) )
		{
			//no more solutions left, it's time to generate a new maze
			m_pGrid->GenerateMaze();
			m_pGrid->GenerateSolution();
			m_pGrid->PopNextSolution( square );
			m_pPlayer->SetPosition( square.x - halfSize, square.y - halfSize );
			m_pGrid->PopNextSolution( square );
			m_pPlayer->SetGoal( square.x - halfSize, square.y - halfSize );

			m_newMaze = true;

			return S_OK;
		}

		m_pPlayer->SetGoal( square.x - halfSize, square.y - halfSize );
	}

	//update the player's position
	m_pPlayer->MovePlayer( m_fElapsedTime );

    return S_OK;
}

//------------------------------------------------------------------------------
// Name: InvalidateDeviceObjects
// Desc: Tidies up device-specific data on res change
//------------------------------------------------------------------------------
HRESULT App::InvalidateDeviceObjects()
{
	return S_OK;
}

//------------------------------------------------------------------------------
// Name: DeleteDeviceObjects
// Desc: Tidies up device-specific data on device change
//------------------------------------------------------------------------------
HRESULT App::DeleteDeviceObjects()
{
	//delete buffers
	SAFE_RELEASE( m_pPlayerIB );
	SAFE_RELEASE( m_pPlayerVB );
	SAFE_RELEASE( m_pPlayerMesh );
	SAFE_RELEASE( m_pCubeIB );
	SAFE_RELEASE( m_pCubeVB );
	SAFE_RELEASE( m_pCubeMesh );

	m_dwMeshFVF				= 0;
	m_dwNumPlayerVertices	= 0;
	m_dwNumPlayerPrims		= 0;
	m_dwNumCubeVertices		= 0;
	m_dwNumCubePrims		= 0;

	return S_OK;
}

//------------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Tidies up application-specific data on shutdown
//------------------------------------------------------------------------------
HRESULT App::FinalCleanup()
{
	return S_OK;
}

//------------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point for the application
//------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd )
{
	App theApp;
	theApp.Create( hInstance );
	return theApp.Run();
}

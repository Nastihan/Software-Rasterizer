/******************************************************************************************
*	Chili DirectX Framework Version 16.10.01											  *
*	Game.cpp																			  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#include "MainWindow.h"
#include "Game.h"
//#include "CubeSkinScene.h"
//#include "CubeVertexColorScene.h"
//#include "CubeSolidScene.h"
//#include "DoubleCubeScene.h"
//#include "VertexWaveScene.h"
//#include "CubeVertexPositionColorScene.h"
//#include "CubeSolidGeometryScene.h"
//#include "CubeFlatIndependentScene.h"
//#include "GeometryFlatScene.h"
//#include "Sphere.h"
//#include "GouraudScene.h"
//#include "GouraudPointScene.h"
//#include "PhongPointScene.h"
#include "SpecularPhongPointScene.h"


Game::Game( MainWindow& wnd )
	:
	wnd( wnd ),
	gfx( wnd )
{
	//scenes.push_back(std::make_unique<CubeVertexColorScene>(gfx));
	//scenes.push_back(std::make_unique<CubeSolidScene>(gfx));
	//scenes.push_back(std::make_unique<CubeSkinScene>(gfx, L"images\\office_skin.jpg"));
	//scenes.push_back(std::make_unique<DoubleCubeScene>(gfx));
	//scenes.push_back(std::make_unique<VertexWaveScene>(gfx));
	//scenes.push_back(std::make_unique<CubeVertexPositionColorScene>(gfx));
	//scenes.push_back(std::make_unique<CubeSolidGeometryScene>(gfx));
	//scenes.push_back(std::make_unique<CubeFlatIndependentScene>(gfx));
	//scenes.push_back(std::make_unique<GeometryFlatScene>(gfx,IndexedTriangleList<GeometryFlatScene::Vertex>::Load("models\\bunny.obj")));
	//scenes.push_back(std::make_unique<GeometryFlatScene>(gfx, Sphere::GetPlain<GeometryFlatScene::Vertex>()));
	//scenes.push_back(std::make_unique<GouraudScene>(gfx, Sphere::GetPlainNormals<GouraudScene::Vertex>()));	
	//scenes.push_back(std::make_unique<GouraudPointScene>(gfx, IndexedTriangleList<GouraudPointScene::Vertex>::LoadNormals("Models\\suzanne.obj")));
	//scenes.push_back(std::make_unique<PhongPointScene>(gfx, Plane::GetNormals<PhongPointScene::Vertex>(1)));
	//scenes.push_back(std::make_unique<GouraudScene>(gfx, IndexedTriangleList<GouraudScene::Vertex>::LoadNormals("Models\\suzanne.obj")));
	//scenes.push_back(std::make_unique<PhongPointScene>(gfx, IndexedTriangleList<PhongPointScene::Vertex>::LoadNormals("Models\\suzanne.obj")));
	scenes.push_back(std::make_unique<SpecularPhongPointScene>(gfx, IndexedTriangleList<SpecularPhongPointScene::Vertex>::LoadNormals("Models\\suzanne.obj")));


	curScene = scenes.begin();
}

void Game::Go()
{
	gfx.BeginFrame();
	UpdateModel();
	ComposeFrame();
	gfx.EndFrame();
}

void Game::UpdateModel()
{
	
	const float dt = ft.Mark();
	// Cycle through scenes with tab key
	while (!wnd.kbd.KeyIsEmpty())
	{
		const auto e = wnd.kbd.ReadKey();
		if (e.GetCode() == VK_TAB && e.IsPress())
		{
			if (wnd.kbd.KeyIsPressed(VK_SHIFT))
			{
				ReverseCycleScenes();
			}
			else
			{
				CycleScenes();
			}
		}
		else if (e.GetCode() == VK_ESCAPE && e.IsPress())
		{
			wnd.Kill();
		}
	}

	(*curScene)->Update(wnd.kbd, wnd.mouse, dt);

}

void Game::ComposeFrame()
{	
	(*curScene)->Draw();
}

// CycleScenes function to cycle through scenes
void Game::CycleScenes()
{
	if (++curScene == scenes.end())
	{
		curScene = scenes.begin();
	}
}
void Game::ReverseCycleScenes()
{
	if (curScene == scenes.begin())
	{
		curScene = scenes.end() - 1;
	}
	else
	{
		--curScene;
	}
	
}

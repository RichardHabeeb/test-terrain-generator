//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game()
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

	mapSize = BmpReader::GetGreyScaleData("smallmap.bmp", height_map_file);
}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthNone(), 0);
	context->RSSetState(m_states->CullNone());
	context->RSSetState(m_states->Wireframe());

	m_effect->SetWorld(m_world);

	m_effect->Apply(context);

	context->IASetInputLayout(m_inputLayout.Get());

	m_batch->Begin();

/*	Vector3 xaxis(2.f, 0.f, 0.f);
	Vector3 yaxis(0.f, 0.f, 2.f);
	Vector3 origin = Vector3::Zero;


	VertexPositionColor vertices[] = {
		VertexPositionColor(origin, Colors::White),
		VertexPositionColor(xaxis, Colors::White),
		VertexPositionColor(yaxis, Colors::White),
		VertexPositionColor(Vector3(2.f,1.f,0.f), Colors::Yellow),
	};*/

	

	int terrainWidth = mapSize.x;
	int terrainHeight = mapSize.y;

	long vertexCount = terrainHeight*terrainWidth;

	VertexPositionColor * vertices = new VertexPositionColor[vertexCount];

	int index = 0;

	for (int z = 0; z < terrainHeight; z++)
	{
		for (int x = 0; x < terrainWidth; x++) {
			vertices[index] = VertexPositionColor(Vector3(float(x)*.1f, height_map_file[z*terrainWidth + x], float(z)*.1f), Colors::White);
			index++;
		}
	}
	
	//VertexPositionColor v1 = VertexPositionColor(Vector3(0.f, 0.f, 0.f), Colors::White);
	//VertexPositionColor v2 = VertexPositionColor(Vector3(4.f, 0.f, 0.f), Colors::White);
	//VertexPositionColor v3 = VertexPositionColor(Vector3(4.f, 0.f, 4.f), Colors::White);

	//VertexPositionColor vertices2[] = { vertices[0], vertices[4], vertices[24] };
	//m_batch->DrawTriangle(v1, v2, v3);

	//const uint16_t indices[] = { 0, 5, 1, 6, 2, 7, 3, 8, 4, 9, 9, 14, 9, 13, 8, 12, 7, 11, 6, 10, 5, 10, 15, 11, 16};
	
	//const int indexCount = terrainWidth*2;

	//int indexCount = (4 * terrainWidth + 1)*(terrainHeight / 2) + (terrainHeight % 2)*(2*terrainWidth);
	long indexCount = (((2 * terrainWidth) + 1) * (terrainHeight - 1)) - (terrainHeight/2);
	uint16_t * indices = new uint16_t[indexCount];

	index = 0;
	for (int j = 0; j < terrainHeight-1; j++) {

		for (int i= j*terrainWidth; i < j*terrainWidth +terrainWidth; i++)
		{
			indices[index] = i;
			index++;
			indices[index] = i + terrainWidth;
			index++;
		}

		j++;
		if (j < terrainHeight-1)
		{
			indices[index] = indices[index - 1];
			index++;

			for (int i = (j*terrainWidth) - 1; i >= (j - 1)*terrainWidth; i--)
			{
				int val = 2 * terrainWidth + i;
				indices[index] = val;
				index++;
				indices[index] = val - terrainWidth;
				index++;
			}
		}

	}

	m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, indices, indexCount, vertices, vertexCount);
	

	//m_batch->Draw(D3D_PRIMITIVE_TOPOLOGY_POINTLIST, vertices, 25);
	m_batch->End();

    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetBackBufferRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}

// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
	auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Initialize device dependent objects here (independent of window size).
	m_states = std::make_unique<CommonStates>(device);

	m_effect = std::make_unique<BasicEffect>(device);
	m_effect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(
		device->CreateInputLayout(VertexPositionColor::InputElements,
			VertexPositionColor::InputElementCount,
			shaderByteCode, byteCodeLength,
			m_inputLayout.ReleaseAndGetAddressOf()));

	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

	m_world = Matrix::Identity;
	
	device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
	
	RECT outputSize = m_deviceResources->GetOutputSize();
	UINT backBufferWidth = std::max<UINT>(outputSize.right - outputSize.left, 1);
	UINT backBufferHeight = std::max<UINT>(outputSize.bottom - outputSize.top, 1);

	m_view = Matrix::CreateLookAt(Vector3(5.5f, 5.5f, 5.5f),
		Vector3::Zero, Vector3::UnitY);
	m_proj = Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
		float(backBufferWidth) / float(backBufferHeight), 0.1f, 10.f);

	m_effect->SetView(m_view);
	m_effect->SetProjection(m_proj);
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
	m_states.reset();
	m_effect.reset();
	m_batch.reset();
	m_inputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma once
#include <d3d11_1.h>
#include <directxcolors.h>
#include <directxmath.h>
#include <d3dcompiler.h>

using namespace DirectX;

class Renderer
{
public:
	void DriverTypeIndex(D3D_DRIVER_TYPE g_driverType,
							D3D_FEATURE_LEVEL g_featureLevel,
							UINT createDeviceFlags);
	void CreateDevice();
	void CreateSwapChain(UINT width, UINT height, HWND g_hWnd);
	void CreteRenderTargetView();
	void SetupViewport(UINT width, UINT height);
	void CompileVertexShader();
	void CreateVertexShader();
	void CreateInputLayout();
	void SetInputLayout();
	void CompilePixelShader();
	void CreatePixelShader();
	void CreateVertexBuffer();
	void SetVertexBuffer();
	void SetIndexBuffer();
	void SetPrimitiveTopology();
	void CreateConstantBuffer(D3D11_BUFFER_DESC bd);
	void InitializeWorldMatrix();
	void InitializeViewMatrix();
	void InitializeProjectionMatrix(int width, int height);
	void CleanupDevice();
	void Render();

	HRESULT GetHR();

private:
	D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;

	ID3D11Device*			g_pd3dDevice = nullptr;
	ID3D11Device1*			g_pd3dDevice1 = nullptr;
	ID3D11DeviceContext*	g_pImmediateContext = nullptr;
	ID3D11DeviceContext1*	g_pImmediateContext1 = nullptr;
	IDXGISwapChain*			g_pSwapChain = nullptr;
	IDXGISwapChain1*		g_pSwapChain1 = nullptr;
	ID3D11RenderTargetView* g_pRenderTargetView = nullptr;

	ID3D11VertexShader*		g_pVertexShader = nullptr;
	ID3D11PixelShader*		g_pPixelShader = nullptr;
	ID3D11InputLayout*		g_pVertexLayout = nullptr;
	ID3D11Buffer*			g_pVertexBuffer = nullptr;
	ID3D11Buffer*           g_pIndexBuffer = nullptr;
	ID3D11Buffer*           g_pConstantBuffer = nullptr;

	XMMATRIX                g_World;
	XMMATRIX                g_View;
	XMMATRIX                g_Projection;

	IDXGIFactory1* dxgiFactory = nullptr;
	IDXGIFactory2* dxgiFactory2 = nullptr;

	ID3DBlob* pVSBlob = nullptr;
	ID3DBlob* pPSBlob = nullptr;

	D3D11_VIEWPORT vp;

	HRESULT hr = S_OK;

	struct SimpleVertex
	{
		XMFLOAT3 Pos;
		XMFLOAT4 Colour;
	};

	struct ConstantBuffer
	{
		XMMATRIX mWorld;
		XMMATRIX mView;
		XMMATRIX mProjection;
	};
};
/*

Source used for introduction to the Win32 API
Full credit to iedoc
https://www.braynzarsoft.net/viewtutorial/q16390-braynzar-soft-directx-11-tutorials


NOTE: If there is an error with LPCTSTR and *char change the character set from Unicode to Multi Byte

*/


/*
HINSTANCE:		The HINSTANCE is used to identify a window. Each process has its own window class list,
				and each entry in the windwo class list consists of an intstance handle and a class name.
				The window manager uses the combinatino of the instance handle and the class anme to look up the class,
				as such it is okay to have multiple DLLs all register a class called MyClass as the instance diferentiates them.
Class styles:
	CS_HREDRAW: Redraws the window if a movement or size adjustment changes the width of the client area.
	CS_VREDRAW: Redraws the entire window if a movemnt or size adjustment changes the height of the client area.

WPARAM:			A message parameter
LPARAM:			A message parameter
LRESULT:		A windows datatype
UINT:			An unsigned integer
LPSTR:			A windows datatype

Buffer:			A block of memory that serves as a temporary placeholder for data.
Swap Chain:		A collection of buffers that are used for displaying frames to the user.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vertex Shader
Hull Shader
Tesselator
Domain Shader
Geometry Shader
Stream Output
Rasterization
Pixel Shader
*/


#pragma region Headers

#include <windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
using namespace DirectX;
#pragma endregion


#pragma region Globals

IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DevCon;
ID3D11RenderTargetView* renderTargetView;

ID3D11Buffer* triangleVertBuffer;
ID3D11VertexShader* VS;
ID3D11PixelShader* PS;
ID3D10Blob* VS_Buffer;
ID3D10Blob* PS_Buffer;
ID3D11InputLayout* vertLayout;

float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;
#pragma endregion


#pragma region Variable Constants definitions

//Define window class name
LPCTSTR WndClassName = L"testwindow";
//Sets our windows handle to NULL
HWND hwnd = NULL;
HRESULT hr;

const int Width = 800;    //window width
const int Height = 600;    //window height
#pragma endregion


#pragma region Functions

#pragma region Function Prototypes

bool InitializeDirect3d11App(HINSTANCE hInstance);		// Initialize direct3d
void ReleaseObjects();									// Release unneeded objects to prevent memory leaks
bool InitScene();										// Set up scene
void UpdateScene();										// Change scene on new frame
void DrawScene();										// Draw scene on new frame
#pragma endregion


//Initialize window. Returns false if the window was not made
bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed);

//Main part of the program
int messageloop();

//Windows callback procedure
LRESULT CALLBACK WndProc(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

struct Vertex    //Overloaded Vertex Structure
{
	Vertex() {}
	Vertex(float x, float y, float z)
		: pos(x, y, z) {}
	XMFLOAT3 pos;
};

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
UINT numElements = ARRAYSIZE(layout);

//Main windows function
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	//Initialize Window//
	if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true))
	{
		//If initialization failed, display an error message
		MessageBox(0, L"Window Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if (!InitializeDirect3d11App(hInstance))    //Initialize Direct3D
	{
		MessageBox(0, L"Direct3D Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	if (!InitScene())    //Initialize our scene
	{
		MessageBox(0, L"Scene Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	messageloop();

	ReleaseObjects();

	return 0;
}

bool InitializeDirect3d11App(HINSTANCE hInstance)
{
	HRESULT hr;

	//Describe our Buffer
	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 144;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Describe our SwapChain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//Create our SwapChain

	/*HRESULT D3D11CreateDeviceAndSwapChain(
  __in   IDXGIAdapter *pAdapter,
  pAdapter is a pointer to the video adapter to use

  __in   D3D_DRIVER_TYPE DriverType,
  DriverType says how direct3d will be implimented

  __in   HMODULE Software,
  Software: HMODULE handle to a DLL that will be used to impliment software rasterizing

  __in   UINT Flags,
  Flags is one or more D3D11_CREATE_DEVICE_FLAG together

  __in   const D3D_FEATURE_LEVEL *pFeatureLevels,
  pFeatureLevels is a pointer to an array of D3D_FEATURE_LEVEL enumerated types that say what versino of directx features to use. NULL = highest

  __in   UINT FeatureLevels,
  FeatureLevels is the number of elements in the pFeatureLevels array.

  __in   UINT SDKVersion,
  The version of the DirectX SDK. Use the D3D11_SDK_VERSION

  __in   const DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,
  pSwapChainDesc is a pointer to the created DXGI_SWAP_CHAIN_DESC struct

  __out  IDXGISwapChain **ppSwapChain,
  ppSwapChain is a pointer to an IDXGISwapChain interface to recieve the created SwapChain

  __out  ID3D11Device **ppDevice,
  ppDevice is a pointer to the direct3d device

  __out  D3D_FEATURE_LEVEL *pFeatureLevel,
  pFeatureLevel is a pointer to a D3D_FEATURE_LEVEL which holds the highest feature level avaliable

  __out  ID3D11DeviceContext **ppImmediateContext
  ppImmediateContext is a pointer to the ID3d11DeviceContext. Will be used for the rendering motherods of the device
);*/

// Create swapchain
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &SwapChain, &d3d11Device, NULL, &d3d11DevCon);

	//Create backBuffer
	ID3D11Texture2D* BackBuffer;
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);

	//Create Render Target
	hr = d3d11Device->CreateRenderTargetView(BackBuffer, NULL, &renderTargetView);
	BackBuffer->Release();

	//Set Render Target
	d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, NULL);

	return true;
}

void CleanUp()
{
	// Release the COM Objects we created
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DevCon->Release();
	renderTargetView->Release();
	triangleVertBuffer->Release();
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();
	vertLayout->Release();
}

void ReleaseObjects()
{
	// Release the COM Objects we created
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DevCon->Release();
}

bool InitScene()
{
	//Compile Shaders from shader file
	/*

	HRESULT WINAPI D3DCompileFromFile(
  in      LPCWSTR pFileName,
  in_opt  const D3D_SHADER_MACRO pDefines,
  in_opt  ID3DInclude pInclude,
  in      LPCSTR pEntrypoint,
  in      LPCSTR pTarget,
  in      UINT Flags1,
  in      UINT Flags2,
  out     ID3DBlob ppCode,
  out_opt ID3DBlob ppErrorMsgs
);

	*/

	/*
	
	float4 VS(float4 inPos : POSITION) : SV_POSITION
	{
		return inPos;
	}

	float4 PS() : SV_TARGET
	{
		return float4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	*/

	// FOR BLOG
	/*
	
	Had issue with D3DX11CompileFromFile as it's depreciated
	D3DX11CompileFromFile took different arguments such that
	D3DX11CompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_5_0", 0, 0, 0, &VS_Buffer, 0, 0) was valid
	while
	D3DCompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_5_0", 0, 0, 0, &VS_Buffer, 0, 0) was not as
	the new function takes different arguments and in a different order.
	Furthermore I made the mistake of passing the arguments in in the wrong order:	
	D3DCompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_5_0", 0, 0, 0, &VS_Buffer)
	vs
	D3DCompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_5_0", 0, 0, &VS_Buffer, 0)
	leading to the VS_BUFFER not being written to from the shader and crashing on runtime

	*/

	//Compile Shaders from shader file
	hr = D3DCompileFromFile(L"Effects.fx", 0, 0, "VS", "vs_5_0", 0, 0, &VS_Buffer, 0);
	if (FAILED(hr)) { return false; }
	hr = D3DCompileFromFile(L"Effects.fx", 0, 0, "PS", "ps_5_0", 0, 0, &PS_Buffer, 0);
	if (FAILED(hr)) { return false; }

	//Create the Shader Objects
	hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);

	//Set Vertex and Pixel Shaders
	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);

	//Create the vertex buffer
	Vertex v[] =
	{
		Vertex(0.0f, 0.5f, 0.5f),
		Vertex(0.5f, -0.5f, 0.5f),
		Vertex(-0.5f, -0.5f, 0.5f),
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 3;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &triangleVertBuffer);

	//Set the vertex buffer
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	d3d11DevCon->IASetVertexBuffers(0, 1, &triangleVertBuffer, &stride, &offset);

	//Create the Input Layout
	hr = d3d11Device->CreateInputLayout(layout, numElements, VS_Buffer->GetBufferPointer(),
		VS_Buffer->GetBufferSize(), &vertLayout);

	//Set the Input Layout
	d3d11DevCon->IASetInputLayout(vertLayout);

	//Set Primitive Topology
	d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Create the Viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Width;
	viewport.Height = Height;

	//Set the Viewport
	d3d11DevCon->RSSetViewports(1, &viewport);

	return true;
}

void UpdateScene()
{
	//// Update the colours of our scene
	//red += colormodr * 0.0005f;
	//green += colormodg * 0.0002f;
	//blue += colormodb * 0.0001f;

	//if (red >= 1.0f || red <= 0.0f)
	//	colormodr *= -1;
	//if (green >= 1.0f || green <= 0.0f)
	//	colormodg *= -1;
	//if (blue >= 1.0f || blue <= 0.0f)
	//	colormodb *= -1;
}

void DrawScene()
{
	////Clear our backbuffer to the updated color
	//using RGBA = float[4];

	//d3d11DevCon->ClearRenderTargetView(renderTargetView, RGBA{red, green, blue, 1.0f });

	//Clear our backbuffer
	float bgColor[4] = { (0.0f, 0.0f, 0.0f, 0.0f) };
	d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);

	//Draw the triangle
	d3d11DevCon->Draw(3, 0);

	//Present the backbuffer to the screen
	SwapChain->Present(0, 0);
}


//Initialize the window
// Styles, icons, lablels, windows cursor types.
bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed)
{
	//Start creating the window//

	//Create a new extended windows classs
	WNDCLASSEX wc;

	// Variables that are used to change aspects of the window

	wc.cbSize = sizeof(WNDCLASSEX);						//Size of our windows class
	wc.style = CS_HREDRAW | CS_VREDRAW;					//class styles
	wc.lpfnWndProc = WndProc;							//Default windows procedure function
	wc.cbClsExtra = NULL;								//Extra bytes after our wc structure
	wc.cbWndExtra = NULL;								//Extra bytes after our windows instance
	wc.hInstance = hInstance;							//Instance to current application
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);				//Title bar Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			//Mouse Icon
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);		//Window bg color
	wc.lpszMenuName = NULL;								//Name of the menu attached to our window
	wc.lpszClassName = WndClassName;					//Name of our windows class
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);			//Icon in your taskbar

	if (!RegisterClassEx(&wc))							//Register the windows class
	{
		//if registration failed, display error
		MessageBox(NULL, L"Error registering class",
			L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	hwnd = CreateWindowEx(								//Create our Extended Window
		NULL,											//Extended style
		WndClassName,									//Name of our windows class
		WndClassName,									//Name in the title bar of our window
		WS_OVERLAPPEDWINDOW,							//style of our window
		CW_USEDEFAULT, CW_USEDEFAULT,					//Top left corner of window
		width,											//Width of our window
		height,											//Height of our window
		NULL,											//Handle to parent window
		NULL,											//Handle to a Menu
		hInstance,										//Specifies instance of current program
		NULL											//used for an MDI client window
	);

	if (!hwnd)    //Make sure our window has been created
	{
		//If not, display error
		MessageBox(NULL, L"Error creating window",
			L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(hwnd, ShowWnd);    //Shows our window
	UpdateWindow(hwnd);    //Its good to update our window

	return true;    //if there were no errors, return true
}


//	The message loop
int messageloop()
{
	//	Create a new message structure
	MSG msg;
	//	clear message structure to NULL
	ZeroMemory(&msg, sizeof(MSG));

	//	while there is a message
	while (true)
	{
		//	if there was a windows message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//	if the message was WM_QUIT
			if (msg.message == WM_QUIT)
				break;    //Exit the message loop

			//	Translate the message
			TranslateMessage(&msg);

			//	Send the message to default windows procedure
			DispatchMessage(&msg);
		}

		//Otherewise, keep the flow going
		else
		{
			// Run game code
			UpdateScene();
			DrawScene();
		}

	}

	//return the message
	return (int)msg.wParam;

}

//Default windows procedure
LRESULT CALLBACK WndProc(HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam)
{

	/*
	////////////////////////////////////////////////////////////
		CHANGE COLOUR WITH KEY PRESSES
	////////////////////////////////////////////////////////////
	*/
	//Check message
	switch (msg)
	{

		//For a key down
	case WM_KEYDOWN:

		//if escape key was pressed, display popup box
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?",
				L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)

				//Release the windows allocated memory  
				DestroyWindow(hwnd);
		}
		return 0;

		//if x button in top right was pressed
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	//return the message for windows to handle it
	return DefWindowProc(hwnd,
		msg,
		wParam,
		lParam);
}
#pragma endregion
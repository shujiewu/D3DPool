#include"d3dUtility.h"
GameSound gameSound;
//编写初始化函数
bool d3d::InitD3D(
	HINSTANCE hInstance,
	int width,
	int height,
	ID3D11RenderTargetView ** renderTargetView,
	ID3D11DeviceContext ** immediateContext,
	IDXGISwapChain ** swapChain,
	ID3D11Device ** device,
	ID3D11Texture2D ** depthStencilBuffer,//深度（模板）缓存区
	ID3D11DepthStencilView ** depthStencilView)//深度（模板）视图

{
	/*第一部分：创建一个窗口*/
	//设计
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)d3d::WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"Direct3D11App";

	//注册
	if (!::RegisterClass(&wc)){
		::MessageBox(0, L"RegisterClass() - Failed", 0, 0);
		return false;
	}

	//创建
	HWND hwnd = 0;
	hwnd = ::CreateWindow(
		L"Direct3D11App",
		L"D3D11",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		0,
		0,
		hInstance,
		0);

	if (!hwnd){
		::MessageBox(0, L"CreateWindow() - Failed", 0, 0);
		return false;
	}

	//窗口显示更新
	::ShowWindow(hwnd, SW_SHOW);
	::UpdateWindow(hwnd);

	/*第二部分：初始化D3D*/
	//第一步：描述交换链
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	//描述后台缓存用法
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	sd.OutputWindow = hwnd;
	//多重采样
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	//第二步：创建设备
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	if (FAILED(D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		featureLevels,
		numFeatureLevels,
		D3D11_SDK_VERSION,
		&sd,
		swapChain,
		device,
		NULL,
		immediateContext)))
	{
		::MessageBox(0, L"CreateDevice - Failed", 0, 0);
		return false;
	}

	//第三步：创建并设置渲染目标视图
	HRESULT hr = 0;
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = (*swapChain)->GetBuffer(0,
		__uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBuffer);

	if (FAILED(hr)){
		::MessageBox(0, L"GetBuffer - Failed", 0, 0);
		return false;
	}

	hr = (*device)->CreateRenderTargetView(
		pBackBuffer,
		NULL,
		renderTargetView);

	pBackBuffer->Release();//释放后台缓存

	if (FAILED(hr)){
		::MessageBox(0, L"CreateRender - Failed", 0, 0);
		return false;
	}

	//(*immediateContext)->OMSetRenderTargets(1,
	//	renderTargetView,
	//	NULL);

	//增加的部分//
	D3D11_TEXTURE2D_DESC dsDesc;

	dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsDesc.Width = 800;
	dsDesc.Height = 600;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsDesc.MipLevels = 1;
	dsDesc.ArraySize = 1;
	dsDesc.CPUAccessFlags = 0;
	dsDesc.SampleDesc.Count = 1;
	dsDesc.SampleDesc.Quality = 0;
	dsDesc.MiscFlags = 0;
	dsDesc.Usage = D3D11_USAGE_DEFAULT;

	//创建深度模板缓存
	hr = (*device)->CreateTexture2D(&dsDesc, 0, depthStencilBuffer);
	if (FAILED(hr)){
		::MessageBox(0, L"Create depth stencil buffer failed!", L"ERROR", MB_OK);
		return false;
	}
	//创建深度模板缓存视图
	hr = (*device)->CreateDepthStencilView(*depthStencilBuffer, 0, depthStencilView);
	if (FAILED(hr)){
		::MessageBox(0, L"Create depth stencil view failed!", L"ERROR", MB_OK);
		return false;
	}

	(*immediateContext)->OMSetRenderTargets(1,
		renderTargetView,
		*depthStencilView);//绑定模板




	//第四步：设置视口大小
	D3D11_VIEWPORT vp;
	vp.Width = width;
	vp.Height = height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;  //视口左上角的横坐标
	vp.TopLeftY = 0;   //视口左上角的纵坐标

	(*immediateContext)->RSSetViewports(1, &vp);

	gameSound.GameSoundInit(hwnd);
	return true;
}

//消息循环函数
int d3d::EnterMsgLoop(bool(*ptr_display)(float timeDelta)){
	MSG msg;
	::ZeroMemory(&msg, sizeof(msg));

	static float lastTime = (float)timeGetTime();
	while (msg.message != WM_QUIT){
		if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		else{
			float currTime = (float)timeGetTime();
			float timeDelta = (currTime - lastTime)*0.001f;
			ptr_display(timeDelta);
			lastTime = currTime;
		}
	}
	return msg.wParam;
}
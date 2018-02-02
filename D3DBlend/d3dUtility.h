#ifndef __d3dUtilityH__
#define __d3dUtilityH__

#include<Windows.h>

//XNA数学库相关头文件
#include<D3Dcompiler.h>
#include<xnamath.h>

//DX11相关头文件
#include<D3D11.h>
#include<D3DX11.h>
#include<d3dx11effect.h>   //新增头文件

//DX11相关库
#pragma comment(lib,"Effects11.lib")  //新增库文件
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx11.lib")

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")
#include "GameSound.h"

//函数原型
namespace d3d
{
	//初始化
	bool InitD3D(
		HINSTANCE hInstance,
		int width, int height,
		ID3D11RenderTargetView ** renderTargetView,
		ID3D11DeviceContext ** immediateContext,
		IDXGISwapChain ** swapChain,
		ID3D11Device ** device,
		ID3D11Texture2D ** depthStencilBuffer,//深度（模板）缓存区
		ID3D11DepthStencilView ** depthStencilView);//深度（模板）视图
	//消息循环
	int EnterMsgLoop(bool(*ptr_display)(float timeDelta));
	
	//回调函数
	LRESULT CALLBACK WndProc(
		HWND,
		UINT msg,
		WPARAM,
		LPARAM lParam);
}
#endif

#ifndef __d3dUtilityH__
#define __d3dUtilityH__

#include<Windows.h>

//XNA��ѧ�����ͷ�ļ�
#include<D3Dcompiler.h>
#include<xnamath.h>

//DX11���ͷ�ļ�
#include<D3D11.h>
#include<D3DX11.h>
#include<d3dx11effect.h>   //����ͷ�ļ�

//DX11��ؿ�
#pragma comment(lib,"Effects11.lib")  //�������ļ�
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx11.lib")

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"winmm.lib")
#include "GameSound.h"

//����ԭ��
namespace d3d
{
	//��ʼ��
	bool InitD3D(
		HINSTANCE hInstance,
		int width, int height,
		ID3D11RenderTargetView ** renderTargetView,
		ID3D11DeviceContext ** immediateContext,
		IDXGISwapChain ** swapChain,
		ID3D11Device ** device,
		ID3D11Texture2D ** depthStencilBuffer,//��ȣ�ģ�壩������
		ID3D11DepthStencilView ** depthStencilView);//��ȣ�ģ�壩��ͼ
	//��Ϣѭ��
	int EnterMsgLoop(bool(*ptr_display)(float timeDelta));
	
	//�ص�����
	LRESULT CALLBACK WndProc(
		HWND,
		UINT msg,
		WPARAM,
		LPARAM lParam);
}
#endif

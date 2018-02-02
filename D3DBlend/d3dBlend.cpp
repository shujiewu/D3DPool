#include "d3dUtility.h"
#include "Light.h"
#include "GeometryGens.h"
#include <vector>
#include <windowsx.h>
#include "Sphere.h"

using namespace std;
//����ȫ�ֵ�ָ��
ID3D11Device* device = NULL;//D3D11�豸�ӿ�
IDXGISwapChain* swapChain = NULL;//������ָ��
ID3D11DeviceContext* immediateContext = NULL;//ִ��������ָ��
ID3D11RenderTargetView* renderTargetView = NULL;//��ȾĿ����ͼָ��

//Effect ���ȫ��ָ��
ID3D11InputLayout* vertexLayout;
ID3DX11Effect* effect;
ID3DX11EffectTechnique* technique;

POINT	m_lastPos;
float	m_theta = XM_PI*1.5f, m_phy = XM_PI*0.4f; //
float	m_radius = 10.f;

//������������ϵ����
XMMATRIX world;     //��������任�ľ���
XMMATRIX view;      //���ڹ۲�任�ľ���
XMMATRIX projection;//����ͶӰ�任�ľ���

//�������ʺ͹��յ�ȫ�ֶ���
Material boxMaterial;		//���Ӳ���
Material floorMaterial;     //�ذ����
Material waterMaterial;     //ˮ�����
Light light[3];			    //��Դ����
int lightType = 0;			//��Դ����

ID3D11ShaderResourceView* textureBox;  //��������
ID3D11ShaderResourceView* textureFloor;//�ذ�����
ID3D11ShaderResourceView* textureWater;//ˮ������
ID3D11ShaderResourceView* textureGreen;
ID3D11ShaderResourceView* textureWhite;
ID3D11ShaderResourceView* textureTank; //������
ID3D11ShaderResourceView* texturePower;//������
ID3D11ShaderResourceView* textureFrame3;//���
ID3D11ShaderResourceView* textureFrame4;//���
ID3D11ShaderResourceView* textureBlack;//�ڱ�
ID3D11ShaderResourceView* textureStage;//����̨��ɫ
ID3D11ShaderResourceView* textureRed;//����̨��ɫ
ID3D11ShaderResourceView* textureYellow;//����̨��ɫ

static float tankw = 0.0f;
static float powerw = 0.0f;
//ID3D11DepthStencilView * depthStencilView;		//���ģ����ͼ
//XMVECTOR eyePosition;							//�ӵ�λ��



ID3D11DepthStencilView * depthStencilView;
ID3D11Texture2D * depthStencilBuffer;

ID3D11BlendState * blendStateAlpha;
ID3D11BlendState * noColorWriteBS;

ID3D11RasterizerState * noCullRS;
ID3D11RasterizerState * counterClockFrontRS;

ID3D11DepthStencilState * markMirrorDSS;
ID3D11DepthStencilState * drawReflectionDSS;



extern GameSound gameSound;
void SetLightEffect(Light light);


//��������
void initpara();
bool BuildBuffers();
GeoGen::MeshData	m_grid;
GeoGen::MeshData	m_box;
GeoGen::MeshData	m_sphere;
GeoGen::MeshData	m_cylinder;
GeoGen::MeshData	m_frame;
GeoGen::MeshData	m_frame2;
GeoGen::MeshData	m_frame3;
GeoGen::MeshData	m_frame4;
GeoGen::MeshData	m_tabletop;
GeoGen::MeshData	m_stage;
GeoGen::MeshData	m_tank;
GeoGen::MeshData	m_power;
GeoGen::MeshData	m_room;
//�������嶥�㡢����λ����Ϣ
UINT	m_gridVStart, m_gridIStart;
UINT	m_boxVStart, m_boxIStart;
UINT	m_sphereVStart, m_sphereIStart;
UINT	m_cylinderVStart, m_cylinderIStart;
UINT	m_frameVStart, m_frameIStart;
UINT	m_frame2VStart, m_frame2IStart;
UINT	m_frame3VStart, m_frame3IStart;
UINT	m_frame4VStart, m_frame4IStart;
UINT	m_tabletopVStart, m_tabletopIStart;
UINT	m_stageVStart, m_stageIStart;
UINT	m_tankVStart, m_tankIStart;
UINT	m_powerVStart, m_powerIStart;
UINT	m_roomVStart, m_roomIStart;
//���㡢����������
ID3D11Buffer	*m_VB;
ID3D11Buffer	*m_IB;
ID3D11InputLayout	*m_inputLayout;
Material	m_matGrid;
Material	m_matBox;
Material	m_matSphere;
Material	m_matCylinder;
Material	m_matFrame;
Material	m_matFrame2;
Material	m_matFrame3;
Material	m_matFrame4;
Material	m_matTabletop;
Material	m_matStage;
Material	m_matTank;
Material	m_matPower;
Material	m_matRoom;

XMFLOAT3	m_eyePos;
Material	m_matCue;


//�ӽǡ�ͶӰ����
XMFLOAT4X4	m_view;
XMFLOAT4X4	m_proj;
//������������任����
XMFLOAT4X4	m_gridWorld;
XMFLOAT4X4	m_boxWorld;
XMFLOAT4X4	m_frameWorld;
XMFLOAT4X4	m_frame2World;
XMFLOAT4X4	m_frame3World;
XMFLOAT4X4	m_frame4World;
XMFLOAT4X4	m_tabletopWorld;
XMFLOAT4X4	m_stageWorld;
XMFLOAT4X4	m_tankWorld;
XMFLOAT4X4	m_powerWorld;
XMFLOAT4X4	m_roomWorld;
XMFLOAT4X4	m_sphereWorld[5];
//XMFLOAT4X4	m_spherePos[16];
XMFLOAT4X4	m_cueWorld;

XMFLOAT4X4	m_cylinderWorld[4];
//Effectȫ�ֱ���
//���ÿ������
ID3DX11EffectMatrixVariable		*m_fxWorldViewProj;
ID3DX11EffectMatrixVariable		*m_fxWorld;
ID3DX11EffectMatrixVariable		*m_fxWorldInvTranspose;
ID3DX11EffectVariable			*m_fxMaterial;
//���ÿһ֡
ID3DX11EffectVariable			*m_fxLights;
ID3DX11EffectVariable			*m_fxEyePos;
Light				m_lights[3];

XMVECTOR Eye, Up, At;

float cueMove = 0.0f;
bool m_bButtonDown = false;
float m_fCueVelocity = 0.0f;
XMFLOAT3 m_vecCuePosition;
XMFLOAT3 m_vecCueDir;
#define CUEMAXSPEED 2.0f
CSphere *m_pSphere[16];
void InitilizeSpheresPosition();
void SphereHitBarProcess(int NumOfSphere);
void SphereHitSphereProcess(int NumOfSphere);
void CollisionResponseBetweenSphere(int i, int j);
#define SPHEREHEIGHT  0.22f
float holePos[6][2] = { { 13.5, -26.9 }, { -13.5, -26.9 }, { 14.4, 0 }, { -14.4, 0 }, { 13.5, 26.9 }, { -13.5, 26.9 } };
bool IntersectTriangle(const XMVECTOR& orig, const XMVECTOR& dir,
	XMVECTOR& v0, XMVECTOR& v1, XMVECTOR& v2,
	float& t, float& u, float& v);
XMVECTOR m_vecRayDir[16];
XMVECTOR m_degreeN[12];
XMVECTOR m_degreePoint[12];


SoundState state;
void SphereFallInHoleProcess(int NumOfSphere);
XMMATRIX InverseTranspose(CXMMATRIX m)
{
	XMMATRIX tmp = m;
	tmp.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	return XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(tmp), tmp));
}
enum  gameState
{
	state_building,
	state_ready,
	state_shooting,
	state_showResult
};


int ngameState = state_ready;



struct Vertexs
{
	XMFLOAT3	pos;
	XMFLOAT3	normal;
};

//����һ������ṹ
struct Vertex
{
	XMFLOAT3 Pos;   //����
	XMFLOAT3 Normal;//������
	XMFLOAT2 Tex;   //��������
};

bool Setup()
{
	//��һ���������ⲿ�ļ�(����fx�ļ���ͼ���ļ�),��.fx�ļ�����ID3DEffect����**************************
	HRESULT hr = S_OK;//����HRESULT�������ڼ�¼���������Ƿ�ɹ�
	ID3DBlob* pTechBlob = NULL;//����ID3DBlob�������ڴ�Ŵ��ļ���ȡ����Ϣ
	//��dx�ļ���ȡ��ɫ���������Ϣ
	hr = D3DX11CompileFromFile(L"Shader.fx",
		NULL,
		NULL,
		NULL,
		"fx_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS,
		0,
		NULL,
		&pTechBlob,
		NULL,
		NULL);

	if (FAILED(hr))
	{
		::MessageBox(NULL, L"fx�ļ�����ʧ��", L"Error", MB_OK);
		return hr;
	}

	//����D3DX11CreateEffectFromMemory����ID3DEffect����
	hr = D3DX11CreateEffectFromMemory(
		pTechBlob->GetBufferPointer(),
		pTechBlob->GetBufferSize(),
		0,
		device,
		&effect);

	if (FAILED(hr))
	{
		::MessageBox(NULL, L"����Effetʧ��", L"Error", MB_OK);
		return hr;
	}

	//���ⲿͼ���ļ���������
	//��������
	D3DX11CreateShaderResourceViewFromFile(device, L"frame.jpg", NULL, NULL, &textureBox, NULL);
	//ˮ�صذ��Լ�ǽ������
	D3DX11CreateShaderResourceViewFromFile(device, L"Green.BMP", NULL, NULL, &textureGreen, NULL);
	//ˮ������
	D3DX11CreateShaderResourceViewFromFile(device, L"water.dds", NULL, NULL, &textureWater, NULL);

	D3DX11CreateShaderResourceViewFromFile(device, L"white.jpg", NULL, NULL, &textureWhite, NULL);

	D3DX11CreateShaderResourceViewFromFile(device, L"tank.png", NULL, NULL, &textureTank, NULL);

	D3DX11CreateShaderResourceViewFromFile(device, L"power.BMP", NULL, NULL, &texturePower, NULL);

	D3DX11CreateShaderResourceViewFromFile(device, L"���.png", NULL, NULL, &textureFrame3, NULL);
	D3DX11CreateShaderResourceViewFromFile(device, L"���.png", NULL, NULL, &textureFrame4, NULL);
	D3DX11CreateShaderResourceViewFromFile(device, L"�ڱ�.png", NULL, NULL, &textureBlack, NULL);
	D3DX11CreateShaderResourceViewFromFile(device, L"̨��.png", NULL, NULL, &textureStage, NULL);
	D3DX11CreateShaderResourceViewFromFile(device, L"��̺.jpg", NULL, NULL, &textureFloor, NULL);

	D3DX11CreateShaderResourceViewFromFile(device, L"Yellow.png", NULL, NULL, &textureYellow, NULL);
	D3DX11CreateShaderResourceViewFromFile(device, L"Red.png", NULL, NULL, &textureRed, NULL);

	initpara();


	D3D11_DEPTH_STENCIL_DESC noDoubleBlendDesc;
	noDoubleBlendDesc.DepthEnable = true;
	noDoubleBlendDesc.DepthFunc = D3D11_COMPARISON_LESS;
	noDoubleBlendDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	noDoubleBlendDesc.StencilEnable = true;
	noDoubleBlendDesc.StencilReadMask = 0xff;
	noDoubleBlendDesc.StencilWriteMask = 0xff;
	noDoubleBlendDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	noDoubleBlendDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
	noDoubleBlendDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	noDoubleBlendDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	noDoubleBlendDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	noDoubleBlendDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	////�ڶ���������������Ⱦ״̬*******************************************************************
	////�˲��ִ������ص�
	////�ȴ���һ�����״̬������
	////�ڶ���������������Ⱦ״̬

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));//�������
	blendDesc.AlphaToCoverageEnable = false;//�ر�AlphaToCoverageEnable���ز�������
	//�رն��RenderTargetʹ�ò�ͬ�Ļ��״̬
	blendDesc.IndependentBlendEnable = false;
	//ֻ���RenderTarget[0]���û��ƻ��״̬������1-7
	blendDesc.RenderTarget[0].BlendEnable = true;                   //�������
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;     //����Դ����
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;//Ŀ������
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;         //��ϲ���
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;      //Դ��ϰٷֱ�

	//Ŀ���ϰٷֱ�����
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//��ϰٷֱȵĲ���
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D11_COLOR_WRITE_ENABLE_ALL;//д����
	//����ID3D11BlendState�ӿ�
	device->CreateBlendState(&blendDesc, &blendStateAlpha);

	//�رձ�������
	D3D11_RASTERIZER_DESC ncDesc;
	ZeroMemory(&ncDesc, sizeof(ncDesc));  //�������
	ncDesc.CullMode = D3D11_CULL_NONE;     //�޳��ض������Σ����ﲻɾ���κ�������
	ncDesc.FillMode = D3D11_FILL_SOLID;   //���ģʽ������Ϊ�������������
	ncDesc.FrontCounterClockwise = false;  //�Ƿ�������ʱ���ƶ���������Ϊ����
	ncDesc.DepthClipEnable = true;         //������ȼ���

	//����һ���رձ���������״̬������Ҫ�õ�ʱ������ø�ִ��������
	if (FAILED(device->CreateRasterizerState(&ncDesc, &noCullRS)))
	{
		MessageBox(NULL, L"Create'NoCull' rasterizzer state failed!", L"Error", MB_OK);
		return false;
	}


	//�������������벼��****************************************************
	//��GetTachniqueByName��ȡID3DX11EffectTechnique�Ķ���
	//�������޹��յ�TechniqueByNamede����
	technique = effect->GetTechniqueByName("TexTech");//�޹��յ�Technique

	//D3DX11_PASS_DESC�ṹ��������һ��Effect Pass
	D3DX11_PASS_DESC PassDesc;

	//����GetPassByIndex��ȡEffect Pass
	//������GetPass��ȡEffect Pass����������������PassDesc������
	technique->GetPassByIndex(0)->GetDesc(&PassDesc);

	//�������������벼��
	//�˴�����һ��D3D11_INPUT_ELEMENT_DESC����
	//�ö���ṹ����λ�����ꡢ���������������꣬�����������������Ԫ��
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//layoutԪ�ظ���
	UINT numElements = ARRAYSIZE(layout);
	//����CreateInputLayout�������벼��
	hr = device->CreateInputLayout(
		layout,
		numElements,
		PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize,
		&vertexLayout);

	//�������ɵ����벼�ֵ�ִ����������
	immediateContext->IASetInputLayout(vertexLayout);
	if (FAILED(hr))
	{
		::MessageBox(NULL, L"����Input Layoutʧ��", L"Error", MB_OK);
		return hr;
	}

	BuildBuffers();


	//���岽 ���ò��ʺ͹���
	//���ò��ʣ�3�ֹ��յķ������Լ�����ⷴ��ϵ��
	//��������ǰ��λ��ʾ�졢�̡�����ķ����ʣ�1��ʾ��ȫ���䣬0��ʾ��ȫ����
	//ˮ�صذ��Լ�ǽ����
	floorMaterial.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	floorMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	floorMaterial.specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
	floorMaterial.power = 5.0f;

	//���Ӳ���
	boxMaterial.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	boxMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	boxMaterial.specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
	boxMaterial.power = 5.0f;

	//ˮ�����
	waterMaterial.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	//ˮ���AlphaֵΪ0.6������͸����Ϊ40%
	waterMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.6f);
	waterMaterial.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
	waterMaterial.power = 5.0f;

	//���ù�Դ
	Light dirLight, pointLight, spotLight;
	dirLight.type = 0;
	//dirLight.direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f);
	dirLight.direction = XMFLOAT4(0.57735f, -0.57735f, 0.57735f, 0.0f);
	dirLight.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.diffuse = XMFLOAT4(0.5, 0.5f, 0.5f, 1.0f);
	dirLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	//���Դ��Ҫ���ã�λ�á�3�ֹ���ǿ�ȡ�3��˥������
	pointLight.type = 1;
	pointLight.position = XMFLOAT4(0.0f, 30.0f, 0.0f, 1.0f);//��Դλ��
	pointLight.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	pointLight.diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	pointLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	pointLight.attenuation0 = 0;//����˥������
	pointLight.attenuation1 = 0.010f;//һ��˥������
	pointLight.attenuation2 = 0.0001;//����˥������

	//�۹����Ҫ����Light�ṹ�����г�Ա
	spotLight.type = 2;
	spotLight.position = XMFLOAT4(0.0f, 10.0f, 0.0f, 1.0f);//��Դλ��
	spotLight.direction = XMFLOAT4(0.0f, -1.0f, 0.0f, 1.0f);//���շ���
	spotLight.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	spotLight.diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	spotLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

	spotLight.attenuation0 = 0;//����˥������
	spotLight.attenuation1 = 0.1f;//һ��˥������
	spotLight.attenuation2 = 0;//����˥������

	spotLight.alpha = XM_PI / 6;//��׶�Ƕ�
	spotLight.beta = XM_PI / 3;//��׶�Ƕ�
	spotLight.fallOff = 1.0f;//˥��ϵ����һ��Ϊ1.0

	light[0] = dirLight;
	light[1] = pointLight;
	light[2] = spotLight;
	return true;
}

void Cleanup()
{
	//�ͷ�ָ��
	if (renderTargetView) renderTargetView->Release();
	if (immediateContext) immediateContext->Release();
	if (swapChain) swapChain->Release();
	if (device) device->Release();

	if (vertexLayout)vertexLayout->Release();
	if (effect)effect->Release();

	if (textureBox) textureBox->Release();
	if (textureFloor) textureFloor->Release();
	if (textureWater) textureWater->Release();

	if (blendStateAlpha) blendStateAlpha->Release();
	if (noCullRS) noCullRS->Release();
}


bool Display(float timeDelta)
{
	if (device)
	{
		//����һ����������ɫ��Ϣ��4��Ԫ�طֱ��ʾ�죬�̣����Լ�alpha
		float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
		//�����ȾĿ����ͼ
		immediateContext->ClearRenderTargetView(renderTargetView, ClearColor);

		//�����ǰ�󶨵����ģ����ͼ
		immediateContext->ClearDepthStencilView(depthStencilView,
			D3D11_CLEAR_DEPTH, 1.0f, 0);

		//ָ��������ӣ�һ�㲻����������������������ָ��Ϊʹ��blend factor
		float BlendFactor[] = { 0, 0, 0, 0, };


		//��һ���֣�������������ϵ�Լ����յ��ⲿ����********
		//ͨ�����̵��������Ҽ����ı���������ͷ����
		//PlaySound(L"hitball.wav", NULL,  SND_ASYNC);

		//	XMStoreFloat4x4(&m_view, view);
		//����۲��
		//	XMStoreFloat3(&m_eyePos, pos);



		//if (height < -5.0f) height = -5.f;//���ƾ�ͷ��Զ����
		//if (height > 5.0f)height = 5.f;//���ƾ�ͷ�������

		//��ʼ���������
		world = XMMatrixIdentity();

		//	world = XMMatrixIdentity();
		//	XMVECTOR Eye = XMVectorSet(cosf(angle)*height, 3.0f, sinf(angle)*height, 0.0f); // ���λ��
		////XMVECTOR Eye = XMVectorSet(cosf(angle)*3.0f, height, sinf(angle)*3.0f, height); // ���λ��
		//	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);   //Ŀ��������λ��
		//	//XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);  //������Ϸ��ķ���
		//	//XMVECTOR Eye = XMVectorSet(0.0F, 30.0f, 0.0f, 0.0f); // ���λ��sinf(angle)*height * 6
		//	////XMVECTOR Eye = XMVectorSet(cosf(angle)*3.0f, height, sinf(angle)*3.0f, height); // ���λ��
		//	//XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);   //Ŀ��������λ��
		//	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);  //������Ϸ��ķ���
		//	//XMVECTOR Up = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);  //������Ϸ��ķ���
		//sinf(m_phy)*sinf(m_phy)*

		//XMVECTOR Eye = XMVectorSet(m_radius*sin(m_phy)*cos(m_theta), m_radius*cos(m_phy), m_radius*sin(m_phy)*sin(m_theta), 1.f);
		//XMVECTOR Eye = XMVectorSet(m_radius*sinf(m_phy)*cosf(m_theta), m_radius*cosf(m_phy), m_radius*sinf(m_phy)*sinf(m_theta) + m_pSphere[0]->m_spherePos._43, 1.f);
		//XMVECTOR At = XMVectorSet(m_pSphere[0]->m_spherePos._41, m_pSphere[0]->m_spherePos._42, m_pSphere[0]->m_spherePos._43, 1.f);
		////XMVECTOR At = XMVectorSet(0.0f, 0.22f, -12.75f, 1.f); 
		//XMVECTOR Up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		//XMMATRIX view = XMMatrixLookAtLH(Eye, At, Up);

		float y;
		if (ngameState == state_ready)
		{
			y = m_radius*cosf(m_phy) + m_pSphere[0]->m_spherePos._42;
			if (y <= -0.35)
				y = 0;
			Eye = XMVectorSet(m_radius*sinf(m_phy)*cosf(m_theta) + m_pSphere[0]->m_spherePos._41, y, m_radius*sinf(m_phy)*sinf(m_theta) + m_pSphere[0]->m_spherePos._43, 1.f);// 
			At = XMVectorSet(m_pSphere[0]->m_spherePos._41, m_pSphere[0]->m_spherePos._42, m_pSphere[0]->m_spherePos._43, 1.f);
			//XMVECTOR At = XMVectorSet(0.0f, 0.22f, -12.75f, 1.f); 
			Up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
			//XMMATRIX view = XMMatrixLookAtLH(Eye, At, Up);
		}
		else
		{
			Eye = XMVectorSet(0.0f, 15.0f, -26.0f, 1.f);
			At = XMVectorSet(0.0f, 0.0f, -16.0f, 1.f);
			//XMVECTOR At = XMVectorSet(0.0f, 0.22f, -12.75f, 1.f); 
			Up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		}
		view = XMMatrixLookAtLH(Eye, At, Up);//���ù۲�����ϵ
		//����ͶӰ����
		projection = XMMatrixPerspectiveFovLH(
			XM_PIDIV2,
			800.0f / 600.0f,
			0.01f,
			100.0f);

		//������任����ĳ��������еľ�����������õ�Effect�����
		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		//��Դ�ĳ��������еĹ�Դ��Ϣ���õ�Effect�����
		SetLightEffect(light[lightType]);

		//�ڶ����֣����Ƹ�������********
		//ע�⣺���ƶ�������ʱ������Ȼ��Ʋ�͸�������壬�ٻ���͸�������壬
		//��Ϊ����Ƶ�����ᵲס�Ȼ��Ƶ����壬�����б��밴��ˮ��-����-ˮ���˳�����

		D3DX11_TECHNIQUE_DESC techDesc;
		technique->GetDesc(&techDesc);//��ȡTechnique������

		immediateContext->OMSetBlendState(blendStateAlpha, BlendFactor, 0xffffffff);//�������


		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		immediateContext->IASetVertexBuffers(0, 1, &m_VB, &stride, &offset);
		immediateContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
		immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matGrid.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matGrid.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matGrid.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matGrid.power);



		//		D3DX11_TECHNIQUE_DESC techDesc;
		technique->GetDesc(&techDesc);

		m_fxWorldViewProj = effect->GetVariableByName("g_worldViewProj")->AsMatrix();
		m_fxWorld = effect->GetVariableByName("")->AsMatrix();
		m_fxWorldInvTranspose = effect->GetVariableByName("g_worldInvTranspose")->AsMatrix();
		m_fxLights = effect->GetVariableByName("g_lights");
		m_fxMaterial = effect->GetVariableByName("g_material");
		m_fxEyePos = effect->GetVariableByName("g_eyePos");



		XMMATRIX world = XMLoadFloat4x4(&m_gridWorld);
		//��������任����
		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		//effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		//m_fxWorld->SetMatrix(reinterpret_cast<float*>(&world));
		//����ͶӰ�任����

		//effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		//m_fxWorldViewProj->SetMatrix(reinterpret_cast<float*>(&(world*view*projection)));
		//���ò���
		//m_fxMaterial->SetRawValue((void*)&m_matGrid, 0, sizeof(m_matGrid));
		XMVECTOR det = XMMatrixDeterminant(world);
		XMMATRIX worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(&det, world));

		//ľ������
		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureBox);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		//����
		//����
		world = XMLoadFloat4x4(&m_roomWorld);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matRoom.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matRoom.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matRoom.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matRoom.power);



		//��ǽ

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_room.indices.size() / 6, m_roomIStart, m_roomVStart);

		//��ǽ

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_room.indices.size() / 6, m_roomIStart + m_room.indices.size() * 1 / 6, m_roomVStart);

		//ǰǽ

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_room.indices.size() / 6, m_roomIStart + m_room.indices.size() * 2 / 6, m_roomVStart);

		//��ǽ

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_room.indices.size() / 6, m_roomIStart + m_room.indices.size() * 3 / 6, m_roomVStart);

		//��̺
		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureFloor);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_room.indices.size() / 6, m_roomIStart + m_room.indices.size() * 4 / 6, m_roomVStart);

		//��ǽ

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_room.indices.size() / 6, m_roomIStart + m_room.indices.size() * 5 / 6, m_roomVStart);


		//������
		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureTank);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);


		world = XMLoadFloat4x4(&m_tankWorld);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matTank.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matTank.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matTank.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matTank.power);




		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_tank.indices.size(), m_tankIStart, m_tankVStart);

		//������
		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(texturePower);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);


		world = XMLoadFloat4x4(&m_powerWorld)*XMMatrixTranslation(powerw / 2.0f, 0.0f, 0.0f);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matPower.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matPower.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matPower.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matPower.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_power.indices.size(), m_powerIStart, m_powerVStart);




		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureStage);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		//�����Һ�

		world = XMLoadFloat4x4(&m_boxWorld)*XMMatrixTranslation(0.0f, 0.0f, 10.0f);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matBox.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matBox.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matBox.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matBox.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_box.indices.size(), m_boxIStart, m_boxVStart);

		//������ǰ
		world = XMLoadFloat4x4(&m_boxWorld)*XMMatrixRotationY(-1.57f)*XMMatrixTranslation(0.0f, 0.0f, 10.0f);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matBox.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matBox.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matBox.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matBox.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_box.indices.size(), m_boxIStart, m_boxVStart);

		//�������
		world = XMLoadFloat4x4(&m_boxWorld)*XMMatrixRotationY(1.57f)*XMMatrixTranslation(0.0f, 0.0f, -10.0f);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matBox.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matBox.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matBox.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matBox.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_box.indices.size(), m_boxIStart, m_boxVStart);

		//������ǰ
		world = XMLoadFloat4x4(&m_boxWorld)*XMMatrixRotationY(3.14f)*XMMatrixTranslation(0.0f, 0.0f, -10.0f);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matBox.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matBox.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matBox.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matBox.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_box.indices.size(), m_boxIStart, m_boxVStart);


		//̨��

		world = XMLoadFloat4x4(&m_stageWorld);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matStage.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matStage.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matStage.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matStage.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_stage.indices.size(), m_stageIStart, m_stageVStart);



		//��߿��ұ�

		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureFrame3);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		world = XMLoadFloat4x4(&m_frame3World);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matFrame3.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matFrame3.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matFrame3.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matFrame3.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame3.indices.size() / 6, m_frame3IStart, m_frame3VStart);

		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureBlack);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame3.indices.size() / 6, m_frame3IStart + m_frame3.indices.size() / 6, m_frame3VStart);

		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureFrame3);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame3.indices.size() * 4 / 6, m_frame3IStart + m_frame3.indices.size() * 2 / 6, m_frame3VStart);

		//��߿����

		world = XMLoadFloat4x4(&m_frame3World)*XMMatrixRotationX(3.14f)*XMMatrixRotationZ(3.14f);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matFrame3.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matFrame3.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matFrame3.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matFrame3.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame3.indices.size() / 6, m_frame3IStart, m_frame3VStart);

		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureBlack);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame3.indices.size() / 6, m_frame3IStart + m_frame3.indices.size() / 6, m_frame3VStart);

		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureFrame3);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame3.indices.size() * 4 / 6, m_frame3IStart + m_frame3.indices.size() * 2 / 6, m_frame3VStart);




		//��߿���
		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureFrame4);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		world = XMLoadFloat4x4(&m_frame4World);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matFrame4.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matFrame4.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matFrame4.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matFrame4.power);


		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureBlack);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame4.indices.size() / 6, m_frame4IStart, m_frame4VStart);

		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureFrame4);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame4.indices.size() / 6, m_frame4IStart + m_frame4.indices.size() / 6, m_frame4VStart);

		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureFrame3);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame4.indices.size() / 6, m_frame4IStart + m_frame4.indices.size() * 2 / 6, m_frame4VStart);

		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureFrame4);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame4.indices.size() * 3 / 6, m_frame4IStart + m_frame4.indices.size() * 3 / 6, m_frame4VStart);
		//��߿�ǰ��


		world = XMLoadFloat4x4(&m_frame4World)*XMMatrixRotationX(3.14f)*XMMatrixRotationZ(3.14f);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matFrame4.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matFrame4.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matFrame4.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matFrame4.power);



		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureBlack);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame4.indices.size() / 6, m_frame4IStart, m_frame4VStart);

		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureFrame4);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame4.indices.size() / 6, m_frame4IStart + m_frame4.indices.size() / 6, m_frame4VStart);

		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureFrame3);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame4.indices.size() / 6, m_frame4IStart + m_frame4.indices.size() * 2 / 6, m_frame4VStart);

		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureFrame4);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame4.indices.size() * 3 / 6, m_frame4IStart + m_frame4.indices.size() * 3 / 6, m_frame4VStart);



		//green����
		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureGreen);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);


		world = XMLoadFloat4x4(&m_tabletopWorld)*XMMatrixRotationX(3.14f)*XMMatrixRotationZ(3.14f);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matTabletop.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matTabletop.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matTabletop.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matTabletop.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_tabletop.indices.size(), m_tabletopIStart, m_tabletopVStart);









		//�� �� ��һ����һ����
		world = XMLoadFloat4x4(&m_frameWorld);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matFrame.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matFrame.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matFrame.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matFrame.power);




		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame.indices.size(), m_frameIStart, m_frameVStart);



		//�ڶ��� 4����

		world = XMLoadFloat4x4(&m_frameWorld)*XMMatrixRotationX(3.14f);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matFrame.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matFrame.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matFrame.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matFrame.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame.indices.size(), m_frameIStart, m_frameVStart);


		//������ 3����

		world = XMLoadFloat4x4(&m_frameWorld)*XMMatrixRotationZ(3.14f);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matFrame.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matFrame.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matFrame.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matFrame.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame.indices.size(), m_frameIStart, m_frameVStart);

		//���ĸ�  2����

		world = XMLoadFloat4x4(&m_frameWorld)*XMMatrixRotationZ(3.14f)*XMMatrixRotationX(3.14f);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matFrame.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matFrame.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matFrame.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matFrame.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame.indices.size(), m_frameIStart, m_frameVStart);

		//ǰ���1��   ��

		world = XMLoadFloat4x4(&m_frame2World);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matFrame2.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matFrame2.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matFrame2.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matFrame2.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame2.indices.size(), m_frame2IStart, m_frame2VStart);



		//ǰ���2��  ǰ


		world = XMLoadFloat4x4(&m_frame2World)*XMMatrixRotationX(3.14f);

		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
		effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matFrame2.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matFrame2.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matFrame2.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matFrame2.power);



		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_frame2.indices.size(), m_frame2IStart, m_frame2VStart);




		//green����
		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureWhite);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);

		for (UINT i = 0; i < 16; ++i)
		{
			world = XMLoadFloat4x4(&(m_pSphere[i]->m_spherePos));
			effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
			effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
			effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
			effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�
			if (i == 0)
			{
				effect->GetVariableByName("Texture")->AsShaderResource()
					->SetResource(textureWhite);
				technique->GetPassByIndex(0)->Apply(0, immediateContext);
				//technique->GetPassByIndex(0)->Apply(0, immediateContext);
				immediateContext->DrawIndexed(m_sphere.indices.size(), m_sphereIStart, m_sphereVStart);
			}
			if (i == 5)
			{
				effect->GetVariableByName("Texture")->AsShaderResource()
					->SetResource(textureBlack);
				technique->GetPassByIndex(0)->Apply(0, immediateContext);
				//technique->GetPassByIndex(0)->Apply(0, immediateContext);
				immediateContext->DrawIndexed(m_sphere.indices.size(), m_sphereIStart, m_sphereVStart);
			}
			else if (i % 2 == 0)
			{

				effect->GetVariableByName("Texture")->AsShaderResource()
					->SetResource(textureYellow);
				technique->GetPassByIndex(0)->Apply(0, immediateContext);
				//technique->GetPassByIndex(0)->Apply(0, immediateContext);
				immediateContext->DrawIndexed(m_sphere.indices.size(), m_sphereIStart, m_sphereVStart);
			}
			else
			{
				effect->GetVariableByName("Texture")->AsShaderResource()
					->SetResource(textureRed);
				technique->GetPassByIndex(0)->Apply(0, immediateContext);
				//technique->GetPassByIndex(0)->Apply(0, immediateContext);
				immediateContext->DrawIndexed(m_sphere.indices.size(), m_sphereIStart, m_sphereVStart);
			}


		}



		//������Ӱ  
		//����ƽ�����ѧ��ʾ����0.f,1.f,2.f,2.5f��  
		Material	m_materialShadow;
		m_materialShadow.ambient = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
		m_materialShadow.diffuse = XMFLOAT4(0.f, 0.f, 0.f, 0.5f);
		m_materialShadow.specular = XMFLOAT4(0.f, 0.f, 0.f, 16.f);
		XMFLOAT3 lightdir;
		lightdir.x = light[0].direction.x;
		lightdir.y = light[0].direction.y;
		lightdir.z = light[0].direction.z;
		XMVECTOR ground = XMVectorSet(0.f, -0.22f, 0.f, 2.5f);//(0.f, -0.22f, 0.f, 2.5f);
		//ͶӰ���򣺹�Դ����ķ���  
		XMVECTOR lightDir = -XMLoadFloat3(&lightdir);
		//����ͶӰ����  
		XMMATRIX S = XMMatrixShadow(ground, lightDir);
		//������Ӱ ��ر任����  
		XMMATRIX worldShadow = XMLoadFloat4x4(&(m_pSphere[0]->m_spherePos)) * S * XMMatrixTranslation(0.f, 0.001f, 0.f);
		XMMATRIX worldInvTransposeShadow = InverseTranspose(worldShadow);
		XMMATRIX wvpShadow = worldShadow * view * projection;
		XMMATRIX texTransShadow = XMMatrixIdentity();
		//���ú�ģ��״̬��NoDoubleBlending  
		immediateContext->OMSetDepthStencilState(NULL, 0x0);

		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_materialShadow.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_materialShadow.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_materialShadow.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_materialShadow.power);
		effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&worldShadow);//������������ϵ
		effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
		effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&wvpShadow);//����ͶӰ����ϵ



		//����������Ӱ  
		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		immediateContext->DrawIndexed(m_sphere.indices.size(), m_sphereIStart, m_sphereVStart);


		immediateContext->OMSetBlendState(0, 0, 0xffffffff);
		immediateContext->OMSetDepthStencilState(0, 0);
		//�ָ�״̬  
		immediateContext->OMSetDepthStencilState(0, 0);


		//ľ������
		effect->GetVariableByName("Texture")->AsShaderResource()
			->SetResource(textureBox);
		technique->GetPassByIndex(0)->Apply(0, immediateContext);
		effect->GetVariableByName("MatAmbient")->AsVector()
			->SetFloatVector((float*)&(m_matCue.ambient));

		effect->GetVariableByName("MatDiffuse")->AsVector()
			->SetFloatVector((float*)&(m_matCue.diffuse));

		effect->GetVariableByName("MatSpecular")->AsVector()
			->SetFloatVector((float*)&(m_matCue.specular));

		effect->GetVariableByName("MatPower")->AsScalar()
			->SetFloat(m_matCue.power);


		static float angler = 0.0f;                         //����һ����̬�������ڼ�¼�Ƕ�
		angler += 2 * timeDelta;                                //����ǰ�Ƕȼ���һ��ʱ��ʹ�Ƕ�������
		if (angler >= 6.28f)                                //�����ǰ�Ƕȴ���2PI�������
		{
			angler = 0.0f;
		}


		XMFLOAT3 vecSpherePos;
		if (ngameState == state_shooting)
		{
			bool hasMove = false;
			for (int i = 0; i <16; i++)
			{
				m_pSphere[i]->GetSpherePosition(vecSpherePos);
				if (abs(vecSpherePos.x)>13.1 || abs(vecSpherePos.z) > 26.5)
				{
					m_pSphere[i]->SetSphereStateToFalse();
					m_pSphere[i]->SetSphereVelocity(0.0f);
					m_pSphere[i]->SetSpherePosition(10000.0f, 10000.0f, 10000.0f);
				}
					
				//{
				if (m_pSphere[i]->GetSphereState())
				{
					SphereHitSphereProcess(i);

					if (m_pSphere[i]->GetSphereVelocity() != 0.0f)
					{
						SphereHitBarProcess(i);
						hasMove = true;
					}

					//if (m_pSphere[i]->GetSpherePosition())
					m_pSphere[i]->MoveSphere();
					SphereFallInHoleProcess(i);

				}
				//�жϰ���

				//}
			}
			if (hasMove == false)
			{
				ngameState = state_ready;
				//m_lastPos.x = 400;
				//m_lastPos.y = 300;
				Sleep(500);
				if (!m_pSphere[0]->GetSphereState())
				{
					m_pSphere[0]->SetSphereStateToTrue();
					m_pSphere[0]->SetSpherePosition(0.0f, 0.22f, -12.75f);
				}
			}

		}


		XMMATRIX mRota;
		mRota = XMMatrixRotationY(-(m_theta - XM_PI*1.5f));
		if (ngameState == state_ready)
		{
			m_cueWorld = m_pSphere[0]->m_spherePos;
			//
			//world = XMMatrixRotationX(1.65f)*XMMatrixTranslation(0.0f, 0.0f, -cueMove)*XMMatrixTranslation(0.0f, 0.0f, -(m_pSphere[0]->m_spherePos._43 - m_cueWorld._43))*XMMatrixRotationY(-(m_theta - XM_PI*1.5f))*XMMatrixTranslation(0.0f, 0.0f, m_pSphere[0]->m_spherePos._43 - m_cueWorld._43)* XMLoadFloat4x4(&m_cueWorld);//
			world = XMMatrixRotationX(1.70f)*XMMatrixTranslation(0.0f, 0.0f, -cueMove)*XMMatrixTranslation(0.0f, 1.5f, -11.5f)*mRota*XMLoadFloat4x4(&m_cueWorld);//XMMatrixRotationY(-(m_theta - XM_PI*1.5f))
			XMFLOAT4X4 cueTemp;
			XMStoreFloat4x4(&cueTemp, world);
			if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000f)
			{
				cueMove += 0.002f;
				m_fCueVelocity += 0.00025f;
				if (m_fCueVelocity > CUEMAXSPEED)
					m_fCueVelocity = CUEMAXSPEED;
			}

			if (m_bButtonDown)
			{
				cueMove -= 0.03f;
				if (cueMove <= -1.0)
				{
					state = tuck;
					gameSound.playsound(state);
					m_bButtonDown = false;
					cueMove = 0.0;
					//m_vecCueDir.x=
					XMFLOAT3 vecSpherePos;
					m_pSphere[0]->GetSpherePosition(vecSpherePos);

					m_vecCueDir.x = vecSpherePos.x - cueTemp._41;
					m_vecCueDir.y = 0;
					m_vecCueDir.z = vecSpherePos.z - cueTemp._43;

					m_pSphere[0]->SetSphereVelocityDir(m_vecCueDir);
					m_pSphere[0]->SetSphereVelocity(m_fCueVelocity);
					ngameState = state_shooting;
					m_fCueVelocity = 0.0f;
				}
			}
			//	if (::GetAsyncKeyState(VK_LBUTTON) & 0x8000f)
			//		cueMove += 0.01f;
			//XMFLOAT3 a;

			//XMStoreFloat4x4(&m_spherePos, world);
			effect->GetVariableByName("World")->AsMatrix()->SetMatrix((float *)&world);//������������ϵ
			effect->GetVariableByName("View")->AsMatrix()->SetMatrix((float *)&view);//���ù۲�����ϵ
			effect->GetVariableByName("Projection")->AsMatrix()->SetMatrix((float *)&projection);//����ͶӰ����ϵ
			effect->GetVariableByName("EyePosition")->AsMatrix()->SetMatrix((float *)&Eye);//�����ӵ�
			technique->GetPassByIndex(0)->Apply(0, immediateContext);
			immediateContext->DrawIndexed(m_cylinder.indices.size(), m_cylinderIStart, m_cylinderVStart);
		}





		//�رջ��
		immediateContext->OMSetBlendState(0, 0, 0xffffffff);


		swapChain->Present(0, 0);


	}
	return true;
}
bool BuildBuffers()
{
	GeoGen::CreateGrid(20.f, 20.f, 50, 50, m_grid);
	GeoGen::CreateBox(2, 1.5f, 2, m_box);
	GeoGen::CreateSphere(0.57, 40, 30, m_sphere);
	GeoGen::CreateCylinder(0.1f, 0.5f, 20, 30, 20, m_cylinder);
	GeoGen::AddCylinderBottomCap(0.1f, 0.5f, 20, 30, 20, m_cylinder);
	//�̿�
	GeoGen::CreateFrame(1, 0.7, -11.6, 11, 12, -12, m_frame);
	GeoGen::CreateFrame2(21, 23, 0.7, 1, m_frame2);
	//ľ��
	GeoGen::CreateFrame3(2, 4, 57, m_frame3);
	GeoGen::CreateFrame4(26.2, 4, 2, m_frame4);
	//����
	GeoGen::CreateTabletop(26.2, 0, 53.6, m_tabletop);
	//̨
	GeoGen::CreateStage(21, 30.2, 6, 50, 57, m_stage);
	//��
	GeoGen::CreateBox(4, 8.0f, 4, m_box);
	//������
	//GeoGen::CreateTank(15, 1.0f, 0, m_tank);
	//GeoGen::CreatePower(powerw, 1.0f, 0, m_power);
	//����
	GeoGen::CreateRoom(80, 80.0f, 100.0f, m_room);

	m_gridVStart = 0;											m_gridIStart = 0;
	m_boxVStart = m_grid.vertices.size();						m_boxIStart = m_grid.indices.size();
	m_sphereVStart = m_boxVStart + m_box.vertices.size();			m_sphereIStart = m_boxIStart + m_box.indices.size();
	m_cylinderVStart = m_sphereVStart + m_sphere.vertices.size();	m_cylinderIStart = m_sphereIStart + m_sphere.indices.size();
	m_frameVStart = m_cylinderVStart + m_cylinder.vertices.size();					m_frameIStart = m_cylinderIStart + m_cylinder.indices.size();
	m_frame2VStart = m_frameVStart + m_frame.vertices.size();					m_frame2IStart = m_frameIStart + m_frame.indices.size();
	m_frame3VStart = m_frame2VStart + m_frame2.vertices.size();					m_frame3IStart = m_frame2IStart + m_frame2.indices.size();
	m_frame4VStart = m_frame3VStart + m_frame3.vertices.size();					m_frame4IStart = m_frame3IStart + m_frame3.indices.size();
	m_tabletopVStart = m_frame4VStart + m_frame4.vertices.size();					m_tabletopIStart = m_frame4IStart + m_frame4.indices.size();
	m_stageVStart = m_tabletopVStart + m_tabletop.vertices.size();					m_stageIStart = m_tabletopIStart + m_tabletop.indices.size();
	m_tankVStart = m_stageVStart + m_stage.vertices.size();					 m_tankIStart = m_stageIStart + m_stage.indices.size();
	m_powerVStart = m_tankVStart + m_tank.vertices.size();					m_powerIStart = m_tankIStart + m_tank.indices.size();
	m_roomVStart = m_powerVStart + m_power.vertices.size();					m_roomIStart = m_powerIStart + m_power.indices.size();


	UINT totalVerts = m_roomVStart + m_room.vertices.size();
	UINT totalIndices = m_roomIStart + m_room.indices.size();

	vector<Vertex> vertices(totalVerts);
	for (UINT i = 0; i < m_grid.vertices.size(); ++i)
	{
		vertices[m_gridVStart + i].Pos = m_grid.vertices[i].pos;
		vertices[m_gridVStart + i].Normal = m_grid.vertices[i].normal;
		vertices[m_gridVStart + i].Tex = m_grid.vertices[i].tex;
	}
	for (UINT i = 0; i < m_box.vertices.size(); ++i)
	{
		vertices[m_boxVStart + i].Pos = m_box.vertices[i].pos;
		vertices[m_boxVStart + i].Normal = m_box.vertices[i].normal;
		vertices[m_boxVStart + i].Tex = m_grid.vertices[i].tex;
	}
	for (UINT i = 0; i < m_sphere.vertices.size(); ++i)
	{
		vertices[m_sphereVStart + i].Pos = m_sphere.vertices[i].pos;
		vertices[m_sphereVStart + i].Normal = m_sphere.vertices[i].normal;
		vertices[m_sphereVStart + i].Tex = m_grid.vertices[i].tex;
	}
	for (UINT i = 0; i < m_cylinder.vertices.size(); ++i)
	{
		vertices[m_cylinderVStart + i].Pos = m_cylinder.vertices[i].pos;
		vertices[m_cylinderVStart + i].Normal = m_cylinder.vertices[i].normal;
		vertices[m_cylinderVStart + i].Tex = m_grid.vertices[i].tex;
	}
	for (UINT i = 0; i < m_frame.vertices.size(); ++i)
	{
		vertices[m_frameVStart + i].Pos = m_frame.vertices[i].pos;
		vertices[m_frameVStart + i].Normal = m_frame.vertices[i].normal;
		vertices[m_frameVStart + i].Tex = m_frame.vertices[i].tex;
	}
	for (UINT i = 0; i < m_frame2.vertices.size(); ++i)
	{
		vertices[m_frame2VStart + i].Pos = m_frame2.vertices[i].pos;
		vertices[m_frame2VStart + i].Normal = m_frame2.vertices[i].normal;
		vertices[m_frame2VStart + i].Tex = m_frame2.vertices[i].tex;
	}
	for (UINT i = 0; i < m_frame3.vertices.size(); ++i)
	{
		vertices[m_frame3VStart + i].Pos = m_frame3.vertices[i].pos;
		vertices[m_frame3VStart + i].Normal = m_frame3.vertices[i].normal;
		vertices[m_frame3VStart + i].Tex = m_frame3.vertices[i].tex;
	}
	for (UINT i = 0; i < m_frame4.vertices.size(); ++i)
	{
		vertices[m_frame4VStart + i].Pos = m_frame4.vertices[i].pos;
		vertices[m_frame4VStart + i].Normal = m_frame4.vertices[i].normal;
		vertices[m_frame4VStart + i].Tex = m_frame4.vertices[i].tex;
	}
	for (UINT i = 0; i < m_tabletop.vertices.size(); ++i)
	{
		vertices[m_tabletopVStart + i].Pos = m_tabletop.vertices[i].pos;
		vertices[m_tabletopVStart + i].Normal = m_tabletop.vertices[i].normal;
		vertices[m_tabletopVStart + i].Tex = m_tabletop.vertices[i].tex;
	}
	for (UINT i = 0; i < m_stage.vertices.size(); ++i)
	{
		vertices[m_stageVStart + i].Pos = m_stage.vertices[i].pos;
		vertices[m_stageVStart + i].Normal = m_stage.vertices[i].normal;
		vertices[m_stageVStart + i].Tex = m_stage.vertices[i].tex;
	}
	for (UINT i = 0; i < m_tank.vertices.size(); ++i)
	{
		vertices[m_tankVStart + i].Pos = m_tank.vertices[i].pos;
		vertices[m_tankVStart + i].Normal = m_tank.vertices[i].normal;
		vertices[m_tankVStart + i].Tex = m_tank.vertices[i].tex;
	}
	for (UINT i = 0; i < m_power.vertices.size(); ++i)
	{
		vertices[m_powerVStart + i].Pos = m_power.vertices[i].pos;
		vertices[m_powerVStart + i].Normal = m_power.vertices[i].normal;
		vertices[m_powerVStart + i].Tex = m_power.vertices[i].tex;
	}
	for (UINT i = 0; i < m_room.vertices.size(); ++i)
	{
		vertices[m_roomVStart + i].Pos = m_room.vertices[i].pos;
		vertices[m_roomVStart + i].Normal = m_room.vertices[i].normal;
		vertices[m_roomVStart + i].Tex = m_room.vertices[i].tex;
	}


	D3D11_BUFFER_DESC vbDesc = { 0 };
	vbDesc.ByteWidth = totalVerts * sizeof(Vertex);
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = 0;
	vbDesc.MiscFlags = 0;
	vbDesc.StructureByteStride = 0;
	vbDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = &vertices[0];
	vbData.SysMemPitch = 0;
	vbData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&vbDesc, &vbData, &m_VB)))
	{
		MessageBox(NULL, L"CreateVertexBuffer failed!", L"Error", MB_OK);
		return false;
	}

	vector<UINT> indices(totalIndices);
	for (UINT i = 0; i < m_grid.indices.size(); ++i)
	{
		indices[m_gridIStart + i] = m_grid.indices[i];
	}
	for (UINT i = 0; i < m_box.indices.size(); ++i)
	{
		indices[m_boxIStart + i] = m_box.indices[i];
	}
	for (UINT i = 0; i < m_sphere.indices.size(); ++i)
	{
		indices[m_sphereIStart + i] = m_sphere.indices[i];
	}
	for (UINT i = 0; i < m_cylinder.indices.size(); ++i)
	{
		indices[m_cylinderIStart + i] = m_cylinder.indices[i];
	}
	for (UINT i = 0; i < m_frame.indices.size(); ++i)
	{
		indices[m_frameIStart + i] = m_frame.indices[i];
	}
	for (UINT i = 0; i < m_frame2.indices.size(); ++i)
	{
		indices[m_frame2IStart + i] = m_frame2.indices[i];
	}
	for (UINT i = 0; i < m_frame3.indices.size(); ++i)
	{
		indices[m_frame3IStart + i] = m_frame3.indices[i];
	}
	for (UINT i = 0; i < m_frame4.indices.size(); ++i)
	{
		indices[m_frame4IStart + i] = m_frame4.indices[i];
	}
	for (UINT i = 0; i < m_tabletop.indices.size(); ++i)
	{
		indices[m_tabletopIStart + i] = m_tabletop.indices[i];
	}
	for (UINT i = 0; i < m_stage.indices.size(); ++i)
	{
		indices[m_stageIStart + i] = m_stage.indices[i];
	}
	for (UINT i = 0; i < m_tank.indices.size(); ++i)
	{
		indices[m_tankIStart + i] = m_tank.indices[i];
	}
	for (UINT i = 0; i < m_power.indices.size(); ++i)
	{
		indices[m_powerIStart + i] = m_power.indices[i];
	}
	for (UINT i = 0; i < m_room.indices.size(); ++i)
	{
		indices[m_roomIStart + i] = m_room.indices[i];
	}

	D3D11_BUFFER_DESC ibDesc = { 0 };
	ibDesc.ByteWidth = totalIndices * sizeof(UINT);
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.MiscFlags = 0;
	ibDesc.StructureByteStride = 0;
	ibDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA ibData;
	ibData.pSysMem = &indices[0];
	ibData.SysMemPitch = 0;
	ibData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&ibDesc, &ibData, &m_IB)))
	{
		MessageBox(NULL, L"CreateIndexBuffer failed!", L"Error", MB_OK);
		return false;
	}
	ShowCursor(FALSE);
	return true;
}
void initpara()
{
	XMMATRIX gridWorld = XMMatrixIdentity();
	XMStoreFloat4x4(&m_gridWorld, gridWorld);
	XMMATRIX sphereWorld = XMMatrixTranslation(0.f, 3.5f, 0.f);
	XMStoreFloat4x4(&m_sphereWorld[4], sphereWorld);
	//�̱߿�
	XMMATRIX frameWorld = XMMatrixTranslation(12.6f, 0.f, 13.0f);
	XMStoreFloat4x4(&m_frameWorld, frameWorld);
	XMMATRIX frame2World = XMMatrixTranslation(0.0f, 0.f, 26.0f);
	XMStoreFloat4x4(&m_frame2World, frame2World);
	//ľ�߿�
	XMMATRIX frame3World = XMMatrixTranslation(14.1f, -1.75f, 0.0f);
	XMStoreFloat4x4(&m_frame3World, frame3World);
	XMMATRIX frame4World = XMMatrixTranslation(0.f, -1.75f, 27.5f);
	XMStoreFloat4x4(&m_frame4World, frame4World);
	//���棬̨��
	XMMATRIX tabletopWorld = XMMatrixTranslation(0.f, -0.35f, 0.0f);
	XMStoreFloat4x4(&m_tabletopWorld, tabletopWorld);
	XMMATRIX stageWorld = XMMatrixTranslation(0.f, -6.7f, 0.0f);
	XMStoreFloat4x4(&m_stageWorld, stageWorld);
	//��
	XMMATRIX boxWorld = XMMatrixTranslation(8.0f, -13.2f, 8.0f);
	XMStoreFloat4x4(&m_boxWorld, boxWorld);

	XMMATRIX tankWorld = XMMatrixTranslation(0.0f, 5.0f, 0.0f);
	XMStoreFloat4x4(&m_tankWorld, tankWorld);
	XMMATRIX powerWorld = XMMatrixTranslation(-7.5f, 5.0f, 0.0f);
	XMStoreFloat4x4(&m_powerWorld, powerWorld);

	//����
	XMMATRIX roomWorld = XMMatrixTranslation(0.0f, 23.0f, 0.0f);
	XMStoreFloat4x4(&m_roomWorld, roomWorld);

	InitilizeSpheresPosition();
	//int count = 1;
	//XMMATRIX sphereWorld1 = XMMatrixTranslation(0.0f, 0.22f, -12.75f);
	//XMStoreFloat4x4(&m_spherePos[0], sphereWorld1);
	//for (UINT i = 0; i<5; ++i)
	//{
	//	for (UINT j = 0; j <= i; ++j)
	//	{
	//		
	//		XMMATRIX sphereWorld2 = XMMatrixTranslation(i*(-0.57f) + 2.0 * 0.57f*j, 0.22f, 12.75f + i*1.0f);
	//		XMStoreFloat4x4(&m_spherePos[count], sphereWorld2);
	//		count++;
	//	}
	//}

	XMMATRIX cueWorld = XMMatrixTranslation(0.0f, 1.0f, -24.0f);
	XMStoreFloat4x4(&m_cueWorld, cueWorld);
	//for (UINT i = 0; i<2; ++i)
	//{
	//	for (UINT j = 0; j<2; ++j)
	//	{
	//		XMMATRIX cylinderWorld = XMMatrixTranslation(-5.f + i*10.f, 1.f, -5.f + j*10.f);
	//		XMStoreFloat4x4(&m_cylinderWorld[i * 2 + j], cylinderWorld);
	//		XMMATRIX sphereWorld = XMMatrixTranslation(-5.f + i*10.f, 4.f, -5.f + j*10.f);
	//		XMStoreFloat4x4(&m_sphereWorld[i * 2 + j], sphereWorld);
	//	}
	//}

	//"����ʽ"����
	//����Դ
	m_lights[0].ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_lights[0].diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_lights[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_lights[0].direction = XMFLOAT4(0.57735f, -0.57735f, 0.57735f, 1.0f);
	//���Դ
	m_lights[1].ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_lights[1].diffuse = XMFLOAT4(0.20f, 0.20f, 0.20f, 1.0f);
	m_lights[1].specular = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_lights[1].direction = XMFLOAT4(-0.57735f, -0.57735f, 0.57735f, 1.0f);
	//����Դ
	m_lights[2].ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_lights[2].diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_lights[2].specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_lights[2].direction = XMFLOAT4(0.0f, -0.707f, -0.707f, 1.0f);

	//����
	m_matGrid.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matGrid.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matGrid.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_matFrame.power = 5.0f;

	m_matBox.ambient = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
	m_matBox.diffuse = XMFLOAT4(0.7f, 0.85f, 0.7f, 1.0f);
	m_matBox.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	m_matBox.power = 5.0f;

	m_matSphere.ambient = XMFLOAT4(0.1f, 0.2f, 0.3f, 1.0f);
	m_matSphere.diffuse = XMFLOAT4(0.2f, 0.4f, 0.6f, 1.0f);
	m_matSphere.specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 16.0f);
	m_matSphere.power = 5.0f;

	m_matCylinder.ambient = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
	m_matCylinder.diffuse = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
	m_matCylinder.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_matCylinder.power = 5.0f;

	m_matFrame.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matFrame.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matFrame.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_matFrame.power = 5.0f;

	m_matFrame2.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matFrame2.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matFrame2.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_matFrame2.power = 5.0f;

	m_matFrame3.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matFrame3.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matFrame3.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_matFrame3.power = 5.0f;

	m_matFrame4.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matFrame4.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matFrame4.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_matFrame4.power = 5.0f;

	m_matTabletop.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matTabletop.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matTabletop.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_matTabletop.power = 5.0f;

	m_matStage.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matStage.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matStage.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_matStage.power = 5.0f;

	m_matTank.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matTank.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matTank.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_matTank.power = 5.0f;

	m_matPower.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matPower.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matPower.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_matPower.power = 5.0f;

	m_matRoom.ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matRoom.diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	m_matRoom.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_matRoom.power = 5.0f;

	m_matCue.ambient = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
	m_matCue.diffuse = XMFLOAT4(0.651f, 0.5f, 0.392f, 1.0f);
	m_matCue.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	m_matCue.power = 5.0f;
	m_lastPos.x = 400;
	m_lastPos.y = 300;






	//For Z-axis proitive  ��������
	m_vecRayDir[0] = XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);
	m_vecRayDir[1] = XMVectorSet(-1.0f, 0.0f, 0.0f, 1.0f);
	m_vecRayDir[2] = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);
	m_vecRayDir[3] = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);

	//
	m_vecRayDir[4] = XMVectorSet(1.0f, 0.0f, -1.0f, 1.0f);
	//
	//m_vecRayDir[4]=XMVector3Normalize(m_vecRayDir[4]);
	m_vecRayDir[5] = XMVectorSet(-1.0f, 0.0f, 1.0f, 1.0f);
	//m_vecRayDir[5] = XMVector3Normalize(m_vecRayDir[5]);

	//����
	m_vecRayDir[6] = XMVectorSet(2.0f, 0.0f, 5.0f, 1.0f);
	//����
	m_vecRayDir[7] = XMVectorSet(2.0f, 0.0f, -5.0f, 1.0f);

	//����
	m_vecRayDir[8] = XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f);
	//����
	m_vecRayDir[9] = XMVectorSet(2.0f, 0.0f, -5.0f, 1.0f);
	m_vecRayDir[10] = XMVectorSet(2.0f, 0.0f, -5.0f, 1.0f);
	m_vecRayDir[11] = XMVectorSet(2.0f, 0.0f, -5.0f, 1.0f);
	m_vecRayDir[12] = XMVectorSet(2.0f, 0.0f, -5.0f, 1.0f);
	m_vecRayDir[13] = XMVectorSet(2.0f, 0.0f, -5.0f, 1.0f);
	m_vecRayDir[14] = XMVectorSet(2.0f, 0.0f, -5.0f, 1.0f);
	m_vecRayDir[15] = XMVectorSet(2.0f, 0.0f, -5.0f, 1.0f);



	m_degreeN[0] = XMVectorSet(1.0f, 0.0f, -1.0f, 1.0f);
	m_degreeN[1] = XMVectorSet(-1.0f, 0.0f, 1.0f, 1.0f);

	m_degreeN[2] = XMVectorSet(2.0f, 0.0f, 5.0f, 1.0f);
	m_degreeN[3] = XMVectorSet(2.0f, 0.0f, -5.0f, 1.0f);


	m_degreeN[4] = XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f);
	m_degreeN[5] = XMVectorSet(-1.0f, 0.0f, -1.0f, 1.0f);

	m_degreeN[6] = XMVectorSet(1.0f, 0.0f, -1.0f, 1.0f);
	m_degreeN[7] = XMVectorSet(-1.0f, 0.0f, 1.0f, 1.0f);

	m_degreeN[8] = XMVectorSet(-2.0f, 0.0f, -5.0f, 1.0f);
	m_degreeN[9] = XMVectorSet(-2.0f, 0.0f, 5.0f, 1.0f);

	m_degreeN[10] = XMVectorSet(-1.0f, 0.0f, -1.0f, 1.0f);
	m_degreeN[11] = XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f);

	for (int i = 0; i < 12; i++)
	{
		m_degreeN[i] = XMVector3Normalize(m_degreeN[i]);
	}


	m_degreePoint[0] = XMVectorSet(-12.1f, 0.f, -24.0f, 1.0f);
	m_degreePoint[1] = XMVectorSet(-10.5f, 0.f, -25.5f, 1.0f);
	m_degreePoint[2] = XMVectorSet(-12.1f, 0.0f, -1.4f, 1.0f);
	m_degreePoint[3] = XMVectorSet(-12.1f, 0.0f, 1.4f, 1.0f);
	m_degreePoint[4] = XMVectorSet(-12.1f, 0.0f, 24.0f, 1.0f);
	m_degreePoint[5] = XMVectorSet(-10.5f, 0.0f, 25.5f, 1.0f);
	m_degreePoint[6] = XMVectorSet(10.5f, 0.0f, 25.5f, 1.0f);
	m_degreePoint[7] = XMVectorSet(12.1f, 0.0f, 24.0f, 1.0f);
	m_degreePoint[8] = XMVectorSet(12.1f, 0.0f, 1.4f, 1.0f);
	m_degreePoint[9] = XMVectorSet(12.1f, 0.0f, -1.4f, 1.0f);
	m_degreePoint[10] = XMVectorSet(12.1f, 0.0f, -24.0f, 1.0f);
	m_degreePoint[11] = XMVectorSet(10.5f, 0.0f, -25.5f, 1.0f);
}

template<typename T>
T Clamp(T vMin, T vMax, T value)
{
	value = max(vMin, value);
	value = min(vMax, value);

	return value;
}

void OnMouseDown(HWND &hwnd, WPARAM btnState, int x, int y)
{
}

void OnMouseUp(HWND &hwnd, WPARAM btnState, int x, int y)
{
	m_bButtonDown = true;
	//cueMove = 0;
}

void OnMouseMove(HWND &hwnd, WPARAM btnState, int x, int y)
{
	//if ((btnState& MK_LBUTTON) != 0)// 
	{
		float dx = XMConvertToRadians(0.35f*(x - m_lastPos.x));
		float dy = XMConvertToRadians(0.35f*(y - m_lastPos.y));

		m_theta -= 2 * dx;
		m_phy -= 2 * dy;

		m_phy = Clamp(0.01f, XM_PI - 0.01f, m_phy);
	}
	//else if ((btnState) != 0) //& MK_RBUTTON
	//{
	//	float dRadius = 0.01f * static_cast<float>(x - m_lastPos.x);
	//	m_radius -= dRadius;

	//	m_radius = Clamp(3.f, 300.f, m_radius);
	//}

	m_lastPos.x = x;
	m_lastPos.y = y;
}
void OnMouseWheel(HWND &hwnd, WPARAM btnState, int x, int y)
{
	int zDelta = GET_WHEEL_DELTA_WPARAM(btnState);

	if (zDelta > 0)
	{
		float dRadius = 1.0f;
		m_radius -= dRadius;

		m_radius = Clamp(3.f, 300.f, m_radius);
	}
	if (zDelta < 0)
	{
		float dRadius = -1.0f;
		m_radius -= dRadius;

		m_radius = Clamp(3.f, 300.f, m_radius);
	}
	//�������¸�������
}

////////////////////////////////////////////////////////////////////
//�ص�����
////////////////////////////////////////////////////////////////////
LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		if (wParam == VK_F1)
			lightType = 0;
		if (wParam == VK_F2)
			lightType = 1;
		if (wParam == VK_F3)
			lightType = 2;
		break;
	case WM_LBUTTONDOWN:
		OnMouseDown(hwnd, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
		//case WM_MBUTTONDOWN:
		//case WM_RBUTTONDOWN:
		//
		//	return 0;

		//����ͷ�ʱ
	case WM_LBUTTONUP:
		OnMouseUp(hwnd, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
		//case WM_MBUTTONUP:
		//case WM_RBUTTONUP:
		//
		//	return 0;
		//
		//����ƶ�ʱ
	case WM_MOUSEMOVE:
		OnMouseMove(hwnd, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEWHEEL:
		OnMouseWheel(hwnd, wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	default:
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////
//��Դ�����������õ�Effect�����
//���ڹ������ñȽϸ��ӣ�������һ����������������
/////////////////////////////////////////////////////
void SetLightEffect(Light light)
{
	//���Ƚ��������͡�������ǿ�ȡ������ǿ�ȡ������ǿ�����õ�Effect��
	effect->GetVariableByName("type")->AsScalar()
		->SetInt(light.type);
	effect->GetVariableByName("LightAmbient")->AsVector()
		->SetFloatVector((float*)&(light.ambient));

	effect->GetVariableByName("LightDiffuse")->AsVector()
		->SetFloatVector((float*)&(light.diffuse));

	effect->GetVariableByName("LightSpecular")->AsVector()
		->SetFloatVector((float*)&(light.specular));

	//���ݹ������͵Ĳ�ͬ���ò�ͬ����
	if (light.type == 0)		//�����
	{
		//�����ֻ��Ҫ���á������������
		effect->GetVariableByName("LightDirection")->AsVector()
			->SetFloatVector((float*)&(light.direction));
		//��������Technique���õ�Effect
		technique = effect->GetTechniqueByName("T_DirLight");
	}
	else if (light.type == 1)			//���Դ
	{
		effect->GetVariableByName("LightPosition")->AsVector()
			->SetFloatVector((float*)&(light.position));

		effect->GetVariableByName("LightAtt0")->AsScalar()
			->SetFloat(light.attenuation0);

		effect->GetVariableByName("LightAtt1")->AsScalar()
			->SetFloat(light.attenuation1);

		effect->GetVariableByName("LightAtt2")->AsScalar()
			->SetFloat(light.attenuation2);

		technique = effect->GetTechniqueByName("T_PointLight");
	}
	else if (light.type == 2)		//�۹�ƹ�Դ
	{
		//�۹�ƹ�Դ��Ҫ���á����򡱣�������˥�����ӡ�����һ��˥�����ӡ���������˥�����ӡ�������׶�Ƕȡ�������׶�Ƕȡ������۹��˥��ϵ����
		effect->GetVariableByName("LightPosition")->AsVector()
			->SetFloatVector((float*)&(light.position));

		effect->GetVariableByName("LightDirection")->AsVector()
			->SetFloatVector((float*)&(light.direction));

		effect->GetVariableByName("LightAtt0")->AsScalar()
			->SetFloat(light.attenuation0);

		effect->GetVariableByName("LightAtt1")->AsScalar()
			->SetFloat(light.attenuation1);

		effect->GetVariableByName("LightAtt2")->AsScalar()
			->SetFloat(light.attenuation2);

		effect->GetVariableByName("LightAlpha")->AsScalar()
			->SetFloat(light.alpha);

		effect->GetVariableByName("LightBeta")->AsScalar()
			->SetFloat(light.beta);

		effect->GetVariableByName("LightFallOff")->AsScalar()
			->SetFloat(light.fallOff);

		technique = effect->GetTechniqueByName("T_SpotLight");

	}
}

//////////////////////////////////////////////////////////////////////
//������WinMain
//////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	if (!d3d::InitD3D(hinstance,
		800,
		600,
		&renderTargetView,
		&immediateContext,
		&swapChain,
		&device,
		&depthStencilBuffer,
		&depthStencilView))//[out] The cteated divice
	{
		::MessageBox(0, L"InitD3D-FAILED", 0, 0);
		return 0;
	}
	if (!Setup()) {
		::MessageBox(0, L"Setup-FAILED", 0, 0);
		return 0;
	}
	//ִ����Ϣѭ������
	d3d::EnterMsgLoop(Display);
	Cleanup();
	return 0;
}

void InitilizeSpheresPosition()
{

	for (int i = 0; i < 16; i++)
		m_pSphere[i] = new CSphere();
	int count = 1;
	//XMMATRIX sphereWorld1 = XMMatrixTranslation(0.0f, 0.22f, -12.75f);
	//XMStoreFloat4x4(&m_spherePos[0], sphereWorld1);
	m_pSphere[0]->SetSpherePosition(0.0f, 0.22f, -12.75f);
	for (UINT i = 0; i < 5; ++i)
	{
		for (UINT j = 0; j <= i; ++j)
		{

			//XMMATRIX sphereWorld2 = XMMatrixTranslation(i*(-0.57f) + 2.0 * 0.57f*j, 0.22f, 12.75f + i*1.0f);
			//XMStoreFloat4x4(&m_spherePos[count], sphereWorld2);
			m_pSphere[count]->SetSpherePosition(i*(-0.57f) + 2.0 * 0.57f*j, 0.22f, 12.75f + i*1.0f);
			count++;
		}
	}
	//add code for others's position.
}
void transForm1(XMMATRIX &mFinal)
{
	XMMATRIX mTrans, mRota;
	mRota = XMMatrixRotationZ(XM_PI)* XMMatrixRotationX(XM_PI);
	mTrans = XMMatrixTranslation(-12.6f, 0.f, -13.0f);
	mFinal = XMMatrixMultiply(mRota, mTrans);
}
void transForm2(XMMATRIX &mFinal)
{
	XMMATRIX mTrans, mRota;
	mRota = XMMatrixRotationZ(XM_PI);
	mTrans = XMMatrixTranslation(-12.6f, 0.f, 13.0f);
	mFinal = XMMatrixMultiply(mRota, mTrans);
}
void transForm3(XMMATRIX &mFinal)
{
	XMMATRIX mTrans, mRota;
	//mRota = XMMatrixRotationZ(XM_PI)* XMMatrixRotationX(XM_PI);
	mTrans = XMMatrixTranslation(0.0f, 0.f, 26.0f);
	mFinal = XMMatrixMultiply(mRota, mTrans);
}
void transForm4(XMMATRIX &mFinal)
{
	XMMATRIX mTrans, mRota;
	//mRota = XMMatrixRotationZ(XM_PI)* XMMatrixRotationX(XM_PI);
	mTrans = XMMatrixTranslation(12.6f, 0.f, 13.0f);
	mFinal = XMMatrixMultiply(mRota, mTrans);
}
void transForm5(XMMATRIX &mFinal)
{
	XMMATRIX mTrans, mRota;
	mRota = XMMatrixRotationX(XM_PI);
	mTrans = XMMatrixTranslation(12.6f, 0.f, -13.0f);
	mFinal = XMMatrixMultiply(mRota, mTrans);
}
void transForm6(XMMATRIX &mFinal)
{
	XMMATRIX mTrans, mRota;
	mRota = XMMatrixRotationX(XM_PI);
	mTrans = XMMatrixTranslation(0.0f, 0.f, -26.0f);
	mFinal = XMMatrixMultiply(mRota, mTrans);
}
bool checkHit(int NumOfSphere,

	XMVECTOR& tri1,
	XMVECTOR& tri2, XMVECTOR& tri3, XMVECTOR& tri4, XMVECTOR& tri5, XMVECTOR& tri6, float &t, float &u, float &v)
	//FXMVECTOR tri1, FXMVECTOR tri2, FXMVECTOR tri3, CXMVECTOR tri4, CXMVECTOR tri5,CXMVECTOR tri6,

{
	XMFLOAT3 vecSpherePos;
	m_pSphere[NumOfSphere]->GetSpherePosition(vecSpherePos);
	XMFLOAT3 vDir;
	m_pSphere[NumOfSphere]->GetSphereVelocityDir(vDir);
	XMVECTOR orig = XMLoadFloat3(&vecSpherePos);//XMVectorSet(-12.1, -0.35, 24, 1.0);
	XMVECTOR dir = XMLoadFloat3(&vDir);
	//XMVECTOR 
	if (IntersectTriangle(orig, dir,
		tri1, tri2, tri3,
		t, u, v))
	{
		if (t <= 0.57f)
		{
			return true;
		}
	}
	if (IntersectTriangle(orig, dir,
		tri4, tri5, tri6,
		t, u, v))
	{
		if (t <= 0.57f)
		{
			return true;
		}
	}
}
XMFLOAT3 m_vecRayPos;
XMFLOAT3 m_vecRayPos2;
//m_vecRayDir[];
void lrHit(int NumOfSphere, float t, int dir)
{

	if (t < 0.57f)
	{
		//state = hitbar;
		//gameSound.playsound(state);
		m_pSphere[NumOfSphere]->ContradictoryXv();
		m_pSphere[NumOfSphere]->ReduceSphereVelocity(0.85f);
		m_pSphere[NumOfSphere]->GetSavedSpherePosition(m_vecRayPos);
		m_pSphere[NumOfSphere]->SetSpherePosition(m_vecRayPos.x, m_vecRayPos.y, m_vecRayPos.z);
		//*********************************************************************
		//*********************Code for spehre stick bar***********************			
		if (m_vecRayPos2.x == m_vecRayPos.x &&m_vecRayPos2.y == m_vecRayPos.y&&m_vecRayPos2.z == m_vecRayPos.z)
		{
			XMVECTOR temp;
			temp = (t - 0.57f)*m_vecRayDir[dir] + XMLoadFloat3(&m_vecRayPos);
			m_pSphere[NumOfSphere]->SetSpherePosition(XMVectorGetX(temp), XMVectorGetY(temp), XMVectorGetZ(temp));
			m_pSphere[NumOfSphere]->SaveSpherePosition();
		}
		m_vecRayPos2 = m_vecRayPos;
		return;
	}
	m_pSphere[NumOfSphere]->SaveSpherePosition();
	//m_pSphere[NumOfSphere]->GetSavedSpherePosition(m_vecRayPos);
	//m_pSphere[NumOfSphere]->SetSpherePosition(m_vecRayPos.x, m_vecRayPos.y, m_vecRayPos.z);
}
void tbHit(int NumOfSphere, float t, int dir)
{
	//XMFLOAT3 m_fvecRayPos;
	//m_pSphere[NumOfSphere]->GetSpherePosition(m_vecRayPos);
	//m_vecRayPos = XMLoadFloat3(&m_fvecRayPos);
	if (t < 0.57f)
	{
		//state = hitbar;
		//gameSound.playsound(state);
		m_pSphere[NumOfSphere]->ContradictoryZv();
		m_pSphere[NumOfSphere]->ReduceSphereVelocity(0.85f);
		m_pSphere[NumOfSphere]->GetSavedSpherePosition(m_vecRayPos);
		m_pSphere[NumOfSphere]->SetSpherePosition(m_vecRayPos.x, m_vecRayPos.y, m_vecRayPos.z);
		if (m_vecRayPos2.x == m_vecRayPos.x &&m_vecRayPos2.y == m_vecRayPos.y&&m_vecRayPos2.z == m_vecRayPos.z)
		{
			XMVECTOR temp;
			temp = (t - 0.57f)*m_vecRayDir[dir] + XMLoadFloat3(&m_vecRayPos);
			m_pSphere[NumOfSphere]->SetSpherePosition(XMVectorGetX(temp), XMVectorGetY(temp), XMVectorGetZ(temp));
			m_pSphere[NumOfSphere]->SaveSpherePosition();
		}
		m_vecRayPos2 = m_vecRayPos;
		return;
	}
	m_pSphere[NumOfSphere]->SaveSpherePosition();
}
void SphereHit(XMFLOAT3 m_vecRayPos, int i, float &distance)
{
	XMVECTOR P, Q, TEMP;
	Q = XMLoadFloat3(&m_vecRayPos);
	P = m_degreePoint[i];
	TEMP = XMVector3Dot(Q - P, m_degreeN[i]);
	distance = XMVectorGetX(TEMP);
	//if (distance < 0)
	//{
	//	distance = 1.0;
	//}

	/*switch (i)
	{
	case 0:
	{
	P = XMVectorSet(-12.1f, 0.f, -24.0f, 1.0f);
	TEMP = XMVector3Dot(P-Q, m_degreeN[0]);
	distance = abs(XMVectorGetX(TEMP));
	break;
	}
	case 1:
	{
	P = XMVectorSet(-10.5f, 0.f, -25.5f, 1.0f);
	TEMP = XMVector3Dot(Q - P, m_degreeN[1]);
	distance = abs(XMVectorGetX(TEMP));
	break;
	}*/

}
void SphereFallInHoleProcess(int NumOfSphere)
{
	XMFLOAT3 m_vecRayPos;
	float m_fDistance;
	m_pSphere[NumOfSphere]->GetSpherePosition(m_vecRayPos);
	if (m_vecRayPos.x < -10.5 && m_vecRayPos.z<-24)
	{
		m_fDistance = sqrtf((m_vecRayPos.x - holePos[1][0])* (m_vecRayPos.x - holePos[1][0]) + (m_vecRayPos.z - holePos[1][1])* (m_vecRayPos.z - holePos[1][1]));

	}
	else if (m_vecRayPos.x > 10.5 && m_vecRayPos.z < -24)
	{
		m_fDistance = sqrtf((m_vecRayPos.x - holePos[0][0])* (m_vecRayPos.x - holePos[0][0]) + (m_vecRayPos.z - holePos[0][1])* (m_vecRayPos.z - holePos[0][1]));
	}
	else if (abs(m_vecRayPos.z) < 1.4 && m_vecRayPos.x < -12.1)
	{
		m_fDistance = sqrtf((m_vecRayPos.x - holePos[3][0])* (m_vecRayPos.x - holePos[3][0]) + (m_vecRayPos.z - holePos[3][1])* (m_vecRayPos.z - holePos[3][1]));
	}
	else if (abs(m_vecRayPos.z) < 1.4 && m_vecRayPos.x>12.1)
	{
		m_fDistance = sqrtf((m_vecRayPos.x - holePos[2][0])* (m_vecRayPos.x - holePos[2][0]) + (m_vecRayPos.z - holePos[2][1])* (m_vecRayPos.z - holePos[2][1]));
	}
	else if (m_vecRayPos.x < -10.5 && m_vecRayPos.z>24)
	{
		m_fDistance = sqrtf((m_vecRayPos.x - holePos[5][0])* (m_vecRayPos.x - holePos[5][0]) + (m_vecRayPos.z - holePos[5][1])* (m_vecRayPos.z - holePos[5][1]));
	}
	else if (m_vecRayPos.x > 10.5  && m_vecRayPos.z > 24)
	{
		m_fDistance = sqrtf((m_vecRayPos.x - holePos[4][0])* (m_vecRayPos.x - holePos[4][0]) + (m_vecRayPos.z - holePos[4][1])* (m_vecRayPos.z - holePos[4][1]));
	}
	else
	{
		return;
	}
	if (m_fDistance < 2.1f)
		m_pSphere[NumOfSphere]->SetSphereVelocityY();

	if (m_pSphere[NumOfSphere]->GetSphereY() <= -2.0f)
	{
		m_pSphere[NumOfSphere]->SetSphereStateToFalse();
		m_pSphere[NumOfSphere]->SetSphereVelocity(0.0f);
		m_pSphere[NumOfSphere]->SetSpherePosition(10000.0f, 10000.0f, 10000.0f);
		state = pocket;
		gameSound.playsound(state);
	}

}
void SphereHitDegree(int NumOfSphere, float t, int i)
{

	state = hitbar;
	gameSound.playsound(state);
	m_pSphere[NumOfSphere]->MirrorVAoubtAxis(m_degreeN[i]); //m_vecRayDir[i]
	m_pSphere[NumOfSphere]->ReduceSphereVelocity(0.65f);
	m_pSphere[NumOfSphere]->GetSavedSpherePosition(m_vecRayPos);
	m_pSphere[NumOfSphere]->SetSpherePosition(m_vecRayPos.x, m_vecRayPos.y, m_vecRayPos.z);
	///*if (m_vecRayPos2.x == m_vecRayPos.x &&m_vecRayPos2.y == m_vecRayPos.y&&m_vecRayPos2.z == m_vecRayPos.z)*/
	//{
	XMVECTOR temp;
	temp = (t - 0.57f)*m_degreeN[i] + XMLoadFloat3(&m_vecRayPos);
	m_pSphere[NumOfSphere]->SetSpherePosition(XMVectorGetX(temp), XMVectorGetY(temp), XMVectorGetZ(temp));
	m_pSphere[NumOfSphere]->SaveSpherePosition();
	//}
	m_vecRayPos2 = m_vecRayPos;
}

void  SphereHitBarProcess(int NumOfSphere)
{
	int i;
	XMFLOAT3 m_vecRayPos, m_vdir;
	m_pSphere[NumOfSphere]->GetSpherePosition(m_vecRayPos);
	m_pSphere[NumOfSphere]->GetSphereVelocityDir(m_vdir);
	float distance;



	if (abs(m_vecRayPos.x) <= 10.5&&abs(m_vecRayPos.z) <= 24.0)//��ȫ��
	{
		m_pSphere[NumOfSphere]->SaveSpherePosition();
		return;
	}
	else if (abs(m_vecRayPos.x) >= 11.0&&abs(m_vecRayPos.z) <= 24.0 && abs(m_vecRayPos.z) >= 0.9)//����ƽ��
	{

		if (m_vecRayPos.x > 0)
			lrHit(NumOfSphere, 12.1 - m_vecRayPos.x, 1);
		else
			lrHit(NumOfSphere, m_vecRayPos.x + 12.1, 3);
		return;
	}
	else if (abs(m_vecRayPos.z) >= 24.0&&abs(m_vecRayPos.x) <= 11.0)//����ƽ��
	{

		if (m_vecRayPos.z > 0)
			tbHit(NumOfSphere, 25.5 - m_vecRayPos.z, 2);
		else
			tbHit(NumOfSphere, m_vecRayPos.z + 25.5, 0);
		return;
	}
	else if (m_vecRayPos.x <= -11.0&&m_vecRayPos.z <= -24.0)
	{
		SphereHit(m_vecRayPos, 0, distance);
		if (distance < 0.57f && distance >0.0f)
		{
			SphereHitDegree(NumOfSphere, distance, 0);
			//m_pSphere[NumOfSphere]->ReduceSphereVelocity(0.05f);
			return;
		}
		SphereHit(m_vecRayPos, 1, distance);
		if (distance < 0.57f&& distance >0.0f)
		{
			SphereHitDegree(NumOfSphere, distance, 1);
			return;
		}
	}
	else if (m_vecRayPos.x <= -11.0&&m_vecRayPos.z >= 24.0)
	{
		SphereHit(m_vecRayPos, 4, distance);
		if (distance < 0.57f&& distance >0.0f)
		{
			SphereHitDegree(NumOfSphere, distance, 4);
			//m_pSphere[NumOfSphere]->ReduceSphereVelocity(0.05f);
			return;
		}
		SphereHit(m_vecRayPos, 5, distance);
		if (distance < 0.57f&& distance >0.0f)
		{
			SphereHitDegree(NumOfSphere, distance, 5);
			return;
		}
	}
	else if (m_vecRayPos.x >= 11.0&&m_vecRayPos.z >= 24.0)
	{
		SphereHit(m_vecRayPos, 6, distance);
		if (distance < 0.57f&& distance >0.0f)
		{
			SphereHitDegree(NumOfSphere, distance, 6);
			//m_pSphere[NumOfSphere]->ReduceSphereVelocity(0.05f);
			return;
		}
		SphereHit(m_vecRayPos, 7, distance);
		if (distance < 0.57f&& distance >0.0f)
		{
			SphereHitDegree(NumOfSphere, distance, 7);
			return;
		}
	}
	else if (m_vecRayPos.x >= 11.0&&m_vecRayPos.z <= -24.0)
	{
		SphereHit(m_vecRayPos, 10, distance);
		if (distance < 0.57f&& distance >0.0f)
		{
			SphereHitDegree(NumOfSphere, distance, 10);
			//m_pSphere[NumOfSphere]->ReduceSphereVelocity(0.05f);
			return;
		}
		SphereHit(m_vecRayPos, 11, distance);
		if (distance < 0.57f&& distance >0.0f)
		{
			SphereHitDegree(NumOfSphere, distance, 11);
			return;
		}
	}
	else if (abs(m_vecRayPos.z) <= 0.9&&m_vecRayPos.x <= -11.0)
	{
		SphereHit(m_vecRayPos, 2, distance);
		if (distance < 0.57f&& distance >0.0f)
		{
			SphereHitDegree(NumOfSphere, distance, 2);
			//m_pSphere[NumOfSphere]->ReduceSphereVelocity(0.05f);
			return;
		}
		SphereHit(m_vecRayPos, 3, distance);
		if (distance < 0.57f&& distance >0.0f)
		{
			SphereHitDegree(NumOfSphere, distance, 3);
			return;
		}
	}
	else if (abs(m_vecRayPos.z) <= 0.9&&m_vecRayPos.x >= 11.0)
	{
		SphereHit(m_vecRayPos, 8, distance);
		if (distance < 0.57f&& distance >0.0f)
		{
			SphereHitDegree(NumOfSphere, distance, 8);
			//m_pSphere[NumOfSphere]->ReduceSphereVelocity(0.05f);
			return;
		}
		SphereHit(m_vecRayPos, 9, distance);
		if (distance < 0.57f&& distance >0.0f)
		{
			SphereHitDegree(NumOfSphere, distance, 9);
			return;
		}
	}
	else
	{
		m_pSphere[NumOfSphere]->SaveSpherePosition();
	}



	//XMFLOAT3 vecSpherePos;
	//m_pSphere[NumOfSphere]->GetSpherePosition(vecSpherePos);
	//XMFLOAT3 vDir;
	//m_pSphere[NumOfSphere]->GetSphereVelocityDir(vDir);
	////XMVECTOR tri1 = XMVectorSet(-12.1, 0.35, 1.4, 1.0);
	////XMVECTOR tri2 = XMVectorSet(-12.1, -0.35, 1.4, 1.0);
	////XMVECTOR tri3 = XMVectorSet(-12.1, -0.35, 24, 1.0);
	////XMVECTOR tri4 = XMVectorSet(-12.1, 0.35, 1.4, 1.0);
	////XMVECTOR tri5 = XMVectorSet(-12.1, 0.35, 24, 1.0);
	////XMVECTOR tri6 = XMVectorSet(-12.1, -0.35, 24, 1.0);
	//XMVECTOR orig = XMLoadFloat3(&vecSpherePos);//XMVectorSet(-12.1, -0.35, 24, 1.0);
	//XMVECTOR dir = XMLoadFloat3(&vDir);
	//float t, u, v;
	////XMVECTOR tri4 = XMVectorSet(FLOAT x, FLOAT y, FLOAT z, FLOAT w);
	//XMVECTOR tri1, tri2, tri3,tri4,tri5,tri6;
	//XMVECTOR TRANS1 = XMVectorSet(12.6f, 0.f, 13.0f, 1.0f);
	//XMVECTOR TRANS2 = XMVectorSet(0.0f, 0.f, 26.0f, 1.0f);
	//m_pSphere[NumOfSphere]->SaveSpherePosition();
	//XMMATRIX mFinal;//mTrans, mRota, 
	//mRota = XMMatrixRotationZ(XM_PI)* XMMatrixRotationX(XM_PI);
	//mTrans = XMMatrixTranslation(1.0f, 2.0f, -3.0f);
	//mFinal = XMMatrixMultiply(mRota, mTrans);
	//vector = XMVector4Transform(vector, mFinal);

	//	int i;
	//if (m_vecRayPos.z <= -1.97)
	//{
	//	//world = XMMatrixRotationX(3.14f)*TRANS1;
	//	transForm1(mFinal);
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[8].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[9].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[10].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[8].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[10].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[11].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////
	//		return;
	//	}
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[4].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[5].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[6].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[4].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[6].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[7].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v)) 
	//	{
	//		/////////chuli
	//		lrHit(NumOfSphere,t,3);
	//		return;
	//	}
	//	transForm6(mFinal);
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[0].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[1].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[2].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[0].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[2].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[3].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		tbHit(NumOfSphere, t, 0);
	//		return;
	//	}
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[4].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[5].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[6].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[4].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[6].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[7].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		return;
	//	}

	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[12].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[13].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[14].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[12].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[14].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[15].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		return;
	//	}

	//	transForm5(mFinal);
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[4].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[5].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[6].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[4].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[6].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[7].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		lrHit(NumOfSphere, t, 1);
	//		return;
	//	}
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[8].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[9].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[10].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[8].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[10].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[11].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		return;
	//	}

	//}
	//else if (m_vecRayPos.z > 1.97)
	//{
	//	transForm2(mFinal);
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[8].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[9].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[10].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[8].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[10].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[11].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////
	//		return;
	//	}
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[4].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[5].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[6].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[4].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[6].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[7].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		lrHit(NumOfSphere, t, 3);
	//		return;
	//	}
	//	transForm3(mFinal);
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[0].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[1].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[2].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[0].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[2].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[3].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		tbHit(NumOfSphere, t, 3);
	//		return;
	//	}
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[4].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[5].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[6].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[4].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[6].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[7].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		return;
	//	}

	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[12].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[13].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[14].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[12].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[14].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame2.vertices[15].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		return;
	//	}

	//	transForm4(mFinal);
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[4].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[5].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[6].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[4].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[6].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[7].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		return;
	//	}
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[8].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[9].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[10].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[8].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[10].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[11].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		lrHit(NumOfSphere, t, 1);
	//		return;
	//	}
	//}
	//else
	//{
	//	transForm1(mFinal);
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[0].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[1].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[2].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[0].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[2].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[3].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		return;
	//	}
	//	transForm2(mFinal);
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[0].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[1].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[2].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[0].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[2].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[3].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		return;
	//	}
	//	transForm4(mFinal);
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[0].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[1].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[2].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[0].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[2].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[3].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		return;
	//	}
	//	transForm5(mFinal);
	//	tri1 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[0].pos), mFinal);
	//	tri2 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[1].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri3 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[2].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[2].pos);
	//	tri4 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[0].pos), mFinal);
	//	tri5 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[2].pos), mFinal); //XMLoadFloat3(&m_frame.vertices[1].pos);
	//	tri6 = XMVector4Transform(XMLoadFloat3(&m_frame.vertices[3].pos), mFinal);
	//	if (checkHit(NumOfSphere, tri1, tri2, tri3, tri4, tri5, tri6, t, u, v))
	//	{
	//		/////////chuli
	//		return;
	//	}
	//}

	//if (IntersectTriangle(orig, dir,
	//	tri1, tri2, tri3,
	//	t, u, v))
	//{
	//	if (t <= 0.57f)
	//	{
	//		m_pSphere[NumOfSphere]->ContradictoryXv();
	//		m_pSphere[NumOfSphere]->ReduceSphereVelocity(0.85f);
	//		m_pSphere[NumOfSphere]->GetSavedSpherePosition(m_vecRayPos);//����ȥ
	//		m_pSphere[NumOfSphere]->SetSpherePosition(m_vecRayPos.x, m_vecRayPos.y, m_vecRayPos.z);
	//		m_pSphere[NumOfSphere]->SaveSpherePosition();

	//	}

	//}
	//if (IntersectTriangle(orig, dir,
	//	tri4, tri5, tri6,
	//	t, u, v))
	//{
	//	if (t <= 0.57f)
	//	{
	//		m_pSphere[NumOfSphere]->ContradictoryXv();
	//		m_pSphere[NumOfSphere]->ReduceSphereVelocity(0.85f);
	//		m_pSphere[NumOfSphere]->GetSavedSpherePosition(m_vecRayPos);//����ȥ
	//		m_pSphere[NumOfSphere]->SetSpherePosition(m_vecRayPos.x, m_vecRayPos.y, m_vecRayPos.z);
	//		m_pSphere[NumOfSphere]->SaveSpherePosition();
	//	}
	//}
}

void SphereHitSphereProcess(int NumOfSphere)
{
	XMFLOAT3 sph1, sph2;
	int i;
	BOOL bFlag = FALSE;
	XMFLOAT3 a;
	a.x = 2.0f;
	a.y = 0;
	a.z = -2.0f;
	m_pSphere[NumOfSphere]->GetSpherePosition(sph1);

	for (i = 0; i < 16; i++)
	{
		if (i != NumOfSphere)
		{
			m_pSphere[i]->GetSpherePosition(sph2);
			if ((sph1.x - sph2.x)*(sph1.x - sph2.x) +
				(sph1.z - sph2.z)*(sph1.z - sph2.z) < 1.29f)
			{
				m_pSphere[NumOfSphere]->GetSavedSpherePosition2(sph1);
				m_pSphere[NumOfSphere]->SetSpherePosition(sph1.x, SPHEREHEIGHT, sph1.z);
				m_pSphere[i]->GetSavedSpherePosition2(sph2);
				m_pSphere[i]->SetSpherePosition(sph2.x, SPHEREHEIGHT, sph2.z);
				//m_pSphere[i]->SetSphereVelocityDir(a);
				//vjm = sqrtf(xj*xj + zj*zj)*0.95f;
				//m_pSphere[i]->SetSphereVelocity(0.3f);
				CollisionResponseBetweenSphere(NumOfSphere, i);
				bFlag = TRUE;
				state = hitball;
				gameSound.playsound(state);
				//������
				//if (m_bSoundCardState)
				//{
				//	if (m_pSphere[i]->GetSphereVelocity() < HALFCUEMAXSPEED)
				//		m_pSoundBimpacting0->Play();
				//	else
				//		m_pSoundBimpacting1->Play();
				//}

			}
		}
	}
	if (!bFlag)
	{
		m_pSphere[NumOfSphere]->SaveSpherePosition2();
	}
}



void CollisionResponseBetweenSphere(int i, int j)
{
	XMFLOAT3 n, t, poti, potj;
	XMFLOAT3 viDir, vjDir, vi, vj;
	XMFLOAT3 xzx, xzz, nni, nnj, tti, ttj;
	float vim, vjm;
	float ni, ti, nj, tj;
	float temp;
	float xi, zi, xj, zj;

	XMFLOAT3 pDestj;
	XMFLOAT3 pDesti;
	xzx.x = 1.0f;
	xzx.y = 0.0f;
	xzx.z = 0.0f;

	xzz.x = 0.0f;
	xzz.y = 0.0f;
	xzz.z = 1.0f;

	m_pSphere[i]->GetSavedSpherePosition2(poti);
	m_pSphere[j]->GetSavedSpherePosition2(potj);

	m_pSphere[i]->GetSphereVelocityDir(viDir);

	XMStoreFloat3(&viDir, XMVector3Normalize(XMLoadFloat3(&viDir)));
	m_pSphere[j]->GetSphereVelocityDir(vjDir);
	XMStoreFloat3(&vjDir, XMVector3Normalize(XMLoadFloat3(&vjDir)));

	vim = m_pSphere[i]->GetSphereVelocity();
	vjm = m_pSphere[j]->GetSphereVelocity();

	vi.x = vim*viDir.x;
	vi.y = vim*viDir.y;
	vi.z = vim*viDir.z;
	vj.x = vjm*vjDir.x;
	vj.y = vjm*vjDir.y;
	vj.z = vjm*vjDir.z;

	n.x = poti.x - potj.x;
	n.y = poti.y - potj.y;
	n.z = poti.z - potj.z;

	XMStoreFloat3(&n, XMVector3Normalize(XMLoadFloat3(&n)));

	t.x = -n.z;
	t.z = n.x;
	t.y = 0.0f;

	ni = vi.x*n.x + vi.y*n.y + vi.z*n.z;
	ti = vi.x*t.x + vi.y*t.y + vi.z * t.z;

	nj = vj.x*n.x + vj.y*n.y + vj.z * n.z;
	tj = vj.x*t.x + vj.y*t.y + vj.z * t.z;


	temp = ni;
	ni = nj;
	nj = temp;

	nni.x = ni*n.x;
	nni.y = ni*n.y;
	nni.z = ni*n.z;

	tti.x = ti*t.x;
	tti.y = ti*t.y;
	tti.z = ti*t.z;

	nnj.x = nj*n.x;
	nnj.y = nj*n.y;
	nnj.z = nj*n.z;

	ttj.x = tj*t.x;
	ttj.y = tj*t.y;
	ttj.z = tj*t.z;

	xi = nni.x*xzx.x + nni.y*xzx.y + nni.z*xzx.z + tti.x*xzx.x + tti.y*xzx.y + tti.z*xzx.z;
	zi = nni.x*xzz.x + nni.y*xzz.y + nni.z*xzz.z + tti.x*xzz.x + tti.y*xzz.y + tti.z*xzz.z;

	poti.x = xi;
	poti.y = 0.0f;
	poti.z = zi;

	XMStoreFloat3(&poti, XMVector3Normalize(XMLoadFloat3(&poti)));
	m_pSphere[i]->SetSphereVelocityDir(poti);
	vim = sqrtf(xi*xi + zi*zi)*0.95f;
	m_pSphere[i]->SetSphereVelocity(vim);

	xj = nnj.x*xzx.x + nnj.y*xzx.y + nnj.z*xzx.z + ttj.x*xzx.x + ttj.y*xzx.y + ttj.z*xzx.z;
	zj = nnj.x*xzz.x + nnj.y*xzz.y + nnj.z*xzz.z + ttj.x*xzz.x + ttj.y*xzz.y + ttj.z*xzz.z;

	potj.x = xj;
	potj.y = 0.0f;
	potj.z = zj;
	XMStoreFloat3(&potj, XMVector3Normalize(XMLoadFloat3(&potj)));
	m_pSphere[j]->SetSphereVelocityDir(potj);
	vjm = sqrtf(xj*xj + zj*zj)*0.95f;
	m_pSphere[j]->SetSphereVelocity(vjm);
}




bool IntersectTriangle(const XMVECTOR& orig, const XMVECTOR& dir,
	XMVECTOR& v0, XMVECTOR& v1, XMVECTOR& v2,
	float& t, float& u, float& v)
{
	// E1
	XMVECTOR TEMP;
	XMVECTOR E1 = v1 - v0;

	// E2
	XMVECTOR E2 = v2 - v0;

	// P
	XMVECTOR P = XMVector3Cross(dir, E2);// dir.Cross(E2);

	// determinant
	TEMP = XMVector3Dot(E1, P);
	float det = XMVectorGetX(TEMP);//E1.Dot(P);

	// keep det > 0, modify T accordingly
	XMVECTOR T;
	if (det > 0)
	{
		T = orig - v0;
	}
	else
	{
		T = v0 - orig;
		det = -det;
	}

	// If determinant is near zero, ray lies in plane of triangle
	if (det < 0.0001f)
		return false;

	// Calculate u and make sure u <= 1


	TEMP = XMVector3Dot(T, P);
	u = XMVectorGetX(TEMP);
	if (u < 0.0f || u > det)
		return false;

	// Q
	XMVECTOR Q = XMVector3Cross(T, E1);


	TEMP = XMVector3Dot(dir, Q);
	v = XMVectorGetX(TEMP);
	if (v < 0.0f || u + v > det)
		return false;

	TEMP = XMVector3Dot(E2, Q);
	t = XMVectorGetX(TEMP);


	float fInvDet = 1.0f / det;
	t *= fInvDet;
	u *= fInvDet;
	v *= fInvDet;


	return true;
}
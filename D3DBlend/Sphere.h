
#include <Windows.h>
#include <xnamath.h>
#include <vector>
#include "GeometryGens.h"
struct SPHERE_STATE
{
	XMFLOAT3 sVector;  
	XMFLOAT3 vVector;  
	float v; 
};

class CSphere
{
public:
	CSphere();
	
	SPHERE_STATE *m_pSphereState;
	XMFLOAT3 m_vecSavePosition;  
	XMFLOAT3 m_vecSavePosition2; 
	XMFLOAT3 m_vecSphereRotationAxis;//xuanzhuan
	XMFLOAT4X4	m_spherePos;
	XMMATRIX mTrans;
	XMMATRIX world;
	BOOL m_bSphereInUse;
	float m_fTotalDis;//zongjuli
	void CreateSphere(float radius, int slice, int stack, GeoGen::MeshData &mesh);
	void MoveSphere();
	void TransformSphere();
	void TransformSphereForUser();
	void UpdateSpherePosition();
	void FrictionReduseVelocity();
	void MoveSphereForUser(float x, float z);
	void SetSphereVelocityY();
	inline void SetSpherePosition(float x, float y, float z)
	{
		
		//m_pSphereState->sVector=XMVectorSet(x, y, z, 1.f);
		m_pSphereState->sVector.x = x;
		m_pSphereState->sVector.y = y;
		m_pSphereState->sVector.z = z;
		XMMATRIX sphereWorld = XMMatrixTranslation(x,y,z);
		XMStoreFloat4x4(&m_spherePos, sphereWorld);

	};
	inline void GetSpherePosition(XMFLOAT3 &vecSpherePos)
	{
		vecSpherePos = m_pSphereState->sVector;
	};
	inline void GetSavedSpherePosition(XMFLOAT3 &vecSavedSpherePos)
	{
		vecSavedSpherePos = m_vecSavePosition;
	};
	inline void GetSavedSpherePosition2(XMFLOAT3 &vecSavedSpherePos)
	{
		vecSavedSpherePos = m_vecSavePosition2;
	};

	inline void SaveSpherePosition()
	{
		m_vecSavePosition = m_pSphereState->sVector;
	};

	inline void SaveSpherePosition2()
	{
		m_vecSavePosition2 = m_pSphereState->sVector;
	};

	inline void ContradictoryZv()
	{
	    m_pSphereState->vVector.z= -m_pSphereState->vVector.z;
	};

	inline void ContradictoryXv()
	{
		//XMVectorSetX(m_pSphereState->vVector, -XMVectorGetX(m_pSphereState->vVector));
		m_pSphereState->vVector.x = -m_pSphereState->vVector.x;
	};

	inline void ReduceSphereVelocity(float percent)
	{
		m_pSphereState->v = m_pSphereState->v*percent;
	};

	inline float CheckSphereEnergy()
	{
		return m_pSphereState->v;
	};

	inline void SetSphereStateToFalse()
	{
		m_bSphereInUse = FALSE;
	};

	inline void SetSphereStateToTrue()
	{
		m_bSphereInUse = TRUE;
	};
	inline void SetSphereVelocityDir(const XMFLOAT3 &vDir)
	{
		m_pSphereState->vVector = vDir;
	};

	inline void SetSphereVelocity(const float &velocity)
	{
		m_pSphereState->v = velocity;
	};

	inline void GetSphereVelocityDir(XMFLOAT3 &vDir)
	{
		vDir = m_pSphereState->vVector;
	};

	inline float GetSphereVelocity()
	{
		return m_pSphereState->v;
	};
	inline float GetSphereY()
	{
		return m_pSphereState->sVector.y;
	};
	inline void MirrorVAoubtAxis(XMVECTOR &n)
	{
		XMVECTOR N = n;
		XMVectorSetY(N, 0.0f);
		N=XMVector3Normalize(N);
		XMStoreFloat3(&m_pSphereState->vVector, (-2 * XMVectorGetX(XMVector3Dot(XMLoadFloat3(&m_pSphereState->vVector), N))*
			N + XMLoadFloat3(&m_pSphereState->vVector)));
	}
	inline BOOL GetSphereState()
	{
		return m_bSphereInUse;
	};
};
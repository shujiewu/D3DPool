#include "Sphere.h"
#define SPHERE_RADIUS 0.57f


CSphere::CSphere()
{
	m_pSphereState = new(SPHERE_STATE);
	//m_pSphereState->sVector = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	//m_pSphereState->vVector = XMVectorSet(0.f, 0.f, 0.f, 1.f);


	m_pSphereState->sVector.x = 0.0f;
	m_pSphereState->sVector.y = 0.0f;
	m_pSphereState->sVector.z = 0.0f;
	m_pSphereState->vVector.x = 0.0f;
	m_pSphereState->vVector.y = 0.0f;
	m_pSphereState->vVector.z = 0.0f;
	m_pSphereState->v = 0.0f;
	m_bSphereInUse = TRUE;
}

void CSphere::MoveSphere()
{
	UpdateSpherePosition();
	FrictionReduseVelocity();
}
void CSphere::TransformSphere()
{
	XMFLOAT4X4 matWorld, matMove, matRotationAboutAxis;
	//XMFLOAT3 matMove, matRotationAboutAxis;
	
	XMMATRIX matW = XMMatrixTranslation(m_pSphereState->sVector.x, m_pSphereState->sVector.y, m_pSphereState->sVector.z);
	XMStoreFloat4x4(&matWorld, matW);

	m_vecSphereRotationAxis.x = m_pSphereState->vVector.z;
	m_vecSphereRotationAxis.z = -m_pSphereState->vVector.x;

	m_fTotalDis += m_pSphereState->v / SPHERE_RADIUS;//jiaodu

}

void CSphere::UpdateSpherePosition()
{
	XMVECTOR a=XMVector3Normalize(XMLoadFloat3(&m_pSphereState->vVector));
	m_pSphereState->vVector.x = XMVectorGetX(a)*m_pSphereState->v;
	m_pSphereState->vVector.y = XMVectorGetY(a)*m_pSphereState->v;
	m_pSphereState->vVector.z = XMVectorGetZ(a)*m_pSphereState->v;
	m_pSphereState->sVector.x += m_pSphereState->vVector.x;
	m_pSphereState->sVector.y += m_pSphereState->vVector.y;
	m_pSphereState->sVector.z += m_pSphereState->vVector.z;
	
	mTrans = XMMatrixTranslation(m_pSphereState->vVector.x, m_pSphereState->vVector.y, m_pSphereState->vVector.z);		
	world = mTrans*XMLoadFloat4x4(&m_spherePos);//
	XMStoreFloat4x4(&m_spherePos, world);
	//m_pSphereState->sVector = m_pSphereState->vVector*m_pSphereState->v;// (m_pSphereState->v)*m_pSphereState->vVector;
	
	//XMStoreFloat4x4(&mTransFL, m_pSphereState->vVector*m_pSphereState->v);
	
}
#define CUEMAXSPEED 2.0f
void CSphere::FrictionReduseVelocity()
{
	if (m_pSphereState->v>0.00001f)
	{
		float v = m_pSphereState->v;
		float a = 0.0f;

		if (0.4f<v && v <= CUEMAXSPEED)
			a = 0.010f;
		else if (0.3f<v && v <= 0.4f)
			a = 0.008f;
		else if (0.25f<v && v <= 0.3f)
			a = 0.005f;
		else if (0.2f<v && v <= 0.25f)
			a = 0.001f;
		else if (0.15f<v && v <= 0.2f)
			a = 0.0001f;
		else if (0.05f<v && v <= 0.15f)
			a = 0.000075f;
		else
			a = 0.000015f;
		m_pSphereState->v = m_pSphereState->v - a;
	}
	else if (m_pSphereState->v != 0.0f)
		m_pSphereState->v = 0.0f;
}

void CSphere::TransformSphereForUser()//Ðý×ª
{
	
}

void CSphere::MoveSphereForUser(float x, float z)
{
	
	//m_pSphereState->sVector = XMVectorSet(x,0.0f, z, 1.f);
	m_pSphereState->sVector.x = x;
	m_pSphereState->sVector.y = 0.0f;
	m_pSphereState->sVector.z = z;
	TransformSphereForUser();
}

void CSphere::SetSphereVelocityY()
{
	m_pSphereState->vVector.y = -2.0f;
	m_pSphereState->v = 0.05f;
}
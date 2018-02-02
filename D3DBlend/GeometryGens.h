#ifndef _GEOMETRY_GENS_H_
#define _GEOMETRY_GENS_H_

#include <Windows.h>
#include <xnamath.h>
#include <vector>

namespace GeoGen
{
	//����һ��ͨ�õĶ���ṹ��λ�á����ߡ����ߡ���������
	struct Vertex
	{
		Vertex(){}
		Vertex(const XMFLOAT3 _pos, XMFLOAT3 _normal, XMFLOAT3 _tangent, XMFLOAT2 _tex):
			pos(_pos),normal(_normal),tangent(_tangent),tex(_tex){}

		XMFLOAT3	pos;
		XMFLOAT3	normal;
		XMFLOAT3	tangent;
		XMFLOAT2	tex;
	};

	//��������ṹ�����㼯��+��������
	struct MeshData
	{
		std::vector<Vertex>	vertices;
		std::vector<UINT>	indices;
	};

	//����һ�������壺ָ����(X����)����(Y����)����(Z����)
	void CreateBox(float width, float height, float depth, MeshData &mesh);
	//����һ��������ӣ�ָ���ܿ��(X����)���ܸ߶�(Z����); m��nΪ���߷����ϵĸ�������
	void CreateGrid(float width, float height, UINT m, UINT n, MeshData &mesh);
	//����һ��Բ��(�����ϡ���������)��ָ���϶˰뾶(topRadius)���¶˰뾶(bottomRadius)���߶�(height)��
	//sllice��stack
	void CreateCylinder(float topRadius, float bottomRadius, float height, int slice, int stack, MeshData &mesh);
	//���ֳɵ�Բ������Ͽ�
	void AddCylinderTopCap(float topRadius, float bottomRadius, float height, int slice, int stack, MeshData &mesh);
	//���ֳɵ�Բ������¿�
	void AddCylinderBottomCap(float topRadius, float bottomRadius, float height, int slice, int stack, MeshData &mesh);
	//����һ�����壺ָ���뾶(radius)��slice��stack
	void CreateSphere(float radius, int slice, int stack, MeshData &mesh);
	//���1
	void CreateFrame(float width, float height, float z1, float z2, float z3, float z4, MeshData &mesh);
	//���2
	void CreateFrame2(float width1, float width2, float height, float depth, MeshData &mesh);
	//�߿�����
	void CreateFrame3(float width, float height, float depth,  MeshData &mesh);
	//�����
	void CreateFrame4(float width, float height, float depth, MeshData &mesh);
	//����
	void CreateTabletop(float width, float height, float depth, MeshData &mesh);
	//̨��
	void CreateStage(float width1, float width2, float height, float depth1, float depth2, MeshData &mesh);
	//������
	void CreateTank(float width, float height, float depth, MeshData &mesh);
	//������
	void CreatePower(float width, float height, float depth, MeshData &mesh);
	//����
	void CreateRoom(float width, float height, float depth, MeshData &mesh);
};


#endif
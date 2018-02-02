#ifndef LIGHT_H_H
#define LIGHT_H_H

#include<xnamath.h>

struct Material
{
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 specular;
	float    power;
};

struct Light
{
	int  type;          //��Դ���ͣ������0�����Դ1���۹��2
	XMFLOAT4 position;  //��Դλ��
	XMFLOAT4 direction; //��������

	XMFLOAT4 ambient;   //������ǿ��
	XMFLOAT4 diffuse;   //�������ǿ��
	XMFLOAT4 specular;  //�����ǿ��

	float attenuation0; //����˥������
	float attenuation1; //һ��˥������
	float attenuation2; //����˥������

	float alpha;        //�۹����׶�Ƕ�
	float beta;         //�۹����׶�Ƕ�
	float fallOff;      //�۹��˥��ϵ����һ��ȡֵΪ1.0
};

#endif
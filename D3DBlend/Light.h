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
	int  type;          //光源类型：方向光0，点光源1，聚光灯2
	XMFLOAT4 position;  //光源位置
	XMFLOAT4 direction; //方向向量

	XMFLOAT4 ambient;   //环境光强度
	XMFLOAT4 diffuse;   //漫反射光强度
	XMFLOAT4 specular;  //镜面光强度

	float attenuation0; //常量衰减因子
	float attenuation1; //一次衰减因子
	float attenuation2; //二次衰减因子

	float alpha;        //聚光灯内锥角度
	float beta;         //聚光灯外锥角度
	float fallOff;      //聚光灯衰减系数，一般取值为1.0
};

#endif
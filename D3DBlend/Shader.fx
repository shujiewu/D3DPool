//�ⲿ�������洢��Ӧ�ó����еõ������ݵı���
//�ⲿ����һ��洢�ڳ��������У�����������������������ڽṹ������
//���ؼ�����cbuffer
//register(b0)��ʾ�ֶ�����ɫ�������󶨵��ض��Ĵ���������b��ʾ��������
//0��ʾ�Ĵ�����ţ�������ɾ����:register��b0����Ч��һ��

cbuffer MatrixBuffer
{
	matrix World;             //����任��������matrix��ʾһ��4x4�ľ���
	matrix View;              //�۲�任����
	matrix Projection;        //ͶӰ�任����
	float4 EyePosition;
};

cbuffer MaterialBuffer
{
	float4 MatAmbient;
	float4 MatDiffuse;
	float4 MatSpecular;
	float MatPower;
}

//��Դ�ĳ�������
cbuffer LightBuffer
{
	int type;
	float4 LightPosition;
	float4 LightDirection;
	float4 LightAmbient;
	float4 LightDiffuse;
	float4 LightSpecular;
	float LightAtt0;
	float LightAtt1;
	float LightAtt2;
	float LightAlpha;
	float LightBeta;
	float LightFallOff;
}

Texture2D Texture;            //�������

SamplerState Sampler
{
	Filter = MIN_MAG_MIP_LINEAR;  //�������Թ���
	AddressU = WRAP;            //ѰַģʽΪWrap
	AddressV = WRAP;
};

//���嶥��ṹ//
//��������ṹ
//���ﶥ������ṹ����λ����Ϣ����ɫ��Ϣ
//HLSL�����У�����������һ�㰴�ա�����  �����������塱�ĸ�ʽ����

struct VS_INPUT
{
	float4 Pos:POSITION;     //λ��//float4�����ͣ�SV_POSITIONΪ���嶨��λ��
	float3 Norm:NORMAL;      //������
	float2 Tex:TEXCOORD0;    //��������
};

//������ɫ��������ṹ
struct VS_OUTPUT
{
	float4 Pos:SV_POSITION;
	float2 Tex:TEXCOORD0;
	float3 Norm:TEXCOORD1;
	float4 ViewDirection: TEXCOORD2;
	float4 LightVector:TEXCOORD3;
};

//������ɫ������
//�����ж�����Ϣ����λ�ú���ɫ�����Է�������Ϊ���Ƕ���Ķ���ṹVS_INPUT

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;          //����һ������ṹ�Ķ���
	output.Pos = mul(input.Pos, World);       //��������任��mulΪ����˷��ĺ���//��input�����Ͻ�������任
	output.Pos = mul(output.Pos, View);       //�۲�����任��mul��һ������Ϊ����
	output.Pos = mul(output.Pos, Projection); //ͶӰ����任��mul�ڶ�������Ϊ����

	output.Norm = mul(input.Norm, (float3x3)World);
	output.Norm = normalize(output.Norm);

	float4 worldPosition = mul(input.Pos, World);
		output.ViewDirection = EyePosition - worldPosition;
	output.ViewDirection = normalize(output.ViewDirection);

	output.LightVector = LightPosition - worldPosition;
	output.LightVector = normalize(output.LightVector);
	output.LightVector.w = length(LightPosition - worldPosition);

	output.Tex = input.Tex;                    //��������
	return output;                           //����VS_INPUT�Ķ���
}

//������ɫ������
//�͵�5�µ�������ɫ���Ĳ�֮ͬ�����ڣ�����Ϊһ��VS_INPUT�Ķ��󣬼�������ɫ�������ֵ

float4 PS(VS_OUTPUT input) :SV_Target
{
	return Texture.Sample(Sampler, input.Tex);    //��������
}

float4 PSDirectionalLight(VS_OUTPUT input) : SV_Target
{
	float4 finalColor;
	float4 ambientColor, diffuseColor, specularColor;

	float3 lightVector = -LightDirection.xyz;
		lightVector = normalize(lightVector);

	ambientColor = MatAmbient*LightAmbient;

	float diffuseFactor = dot(lightVector, input.Norm);
	if (diffuseFactor > 0.0f)
	{
		diffuseColor = MatDiffuse*LightDiffuse*diffuseFactor;

		float3 reflection = reflect(-lightVector, input.Norm);
			float specularFactor = pow(max(dot(reflection, input.ViewDirection.xyz), 0.0f), MatPower);
		specularColor = MatSpecular*LightSpecular*specularFactor;
	}
	float4 texColor = float4(1.f, 1.f, 1.f, 1.f);
		texColor = Texture.Sample(Sampler, input.Tex);

	finalColor = saturate(ambientColor + diffuseColor + specularColor);
	finalColor = texColor*finalColor;
	finalColor.a = texColor.a*MatDiffuse.a;  //����͸����

	return finalColor;    //��������
}

float4 PSPointLight(VS_OUTPUT input) :SV_Target
{
	float4 finalColor;
	float4 ambientColor, diffuseColor, specularColor;

	float3 lightVector = input.LightVector.xyz;
		lightVector = normalize(lightVector);

	ambientColor = MatAmbient*LightAmbient;

	float diffuseFactor = dot(lightVector, input.Norm);

	if (diffuseFactor > 0.0f)
	{
		diffuseColor = MatDiffuse*LightDiffuse*diffuseFactor;

		float3 reflection = reflect(-lightVector, input.Norm);
			float specularFactor = pow(max(dot(reflection, input.ViewDirection.xyz), 0.0f), MatPower);
		specularColor = MatSpecular*LightSpecular*specularFactor;
	}

	float d = input.LightVector.w;
	float att = LightAtt0 + LightAtt1*d + LightAtt2*d*d;

	finalColor = saturate(ambientColor + diffuseColor + specularColor) / att;
	float4 texColor = Texture.Sample(Sampler, input.Tex);
		finalColor = finalColor*texColor;
	finalColor.a = texColor.a*MatDiffuse.a;

	return finalColor;
}

float4 PSSpotLight(VS_OUTPUT input) :SV_Target
{
	float4 finalColor;
	float4 ambientColor, diffuseColor, specularColor;

	//�����������ڶ���Ĺ�������
	float3 lightVector = input.LightVector.xyz;
		lightVector = normalize(lightVector);
	float d = input.LightVector.w;
	float3 lightDirection = LightDirection.xyz;
		lightDirection = normalize(lightDirection);

	float cosTheta = dot(-lightVector, lightDirection);

	if (cosTheta < cos(LightBeta / 2))
		return float4(0.0f, 0.0f, 0.0f, 1.0f);

	ambientColor = MatAmbient*LightAmbient;

	float diffuseFactor = dot(lightVector, input.Norm);

	if (diffuseFactor>0.0f)
	{
		diffuseColor = MatDiffuse*LightDiffuse*diffuseFactor;

		float3 reflection = reflect(-lightVector, input.Norm);
			float specularFactor = pow(max(dot(reflection, input.ViewDirection.xyz), 0.0f), MatPower);
		specularColor = MatSpecular*LightSpecular*specularFactor;
	}

	float att = LightAtt0 + LightAtt1*d + LightAtt2*d*d;

	if (cosTheta > cos(LightAlpha / 2))
	{
		finalColor = saturate(ambientColor + diffuseColor + specularColor) / att;
	}
	else if (cosTheta >= cos(LightBeta / 2) && cosTheta <= cos(LightAlpha / 2))
	{
		float spotFactor = pow((cosTheta - cos(LightAlpha / 2)) / (cos(LightAlpha / 2) - cos(LightBeta / 2)), 1);
		finalColor = spotFactor*saturate(ambientColor + diffuseColor + specularColor) / att;
	}

	float4 texColor = Texture.Sample(Sampler, input.Tex);
		finalColor = finalColor*texColor;
	finalColor.a = texColor.a*MatDiffuse.a;
	return finalColor;
}

//Technique����ͬ��ɫ����������ʵ��ĳ�ֹ���
//����Technique��technique11�ؼ��ֽ���

technique11 TexTech
{
	//Techniqueͨ��Pass��Ӧ�ò�ͬ��Ч����ÿ��Technique�����ж��Pass
	//����ֻ��һ��Pass
	pass P0
	{
		//���ö�����ɫ��
		//CompileShader��������������һ����Ŀ����ɫ���汾����һ��Ϊ���Ƕ������ɫ������
		SetVertexShader(CompileShader(vs_5_0, VS()));   //���ö�����ɫ��
		SetGeometryShader(NULL);                        //������ɫ��
		SetPixelShader(CompileShader(ps_5_0, PS()));    //����������ɫ��
	}
}


//�����Technique
technique11 T_DirLight
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));   //���ö�����ɫ��
		SetPixelShader(CompileShader(ps_5_0, PSDirectionalLight()));    //����������ɫ��
	}
}


//���ԴTechnique
technique11 T_PointLight
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));   //���ö�����ɫ��
		SetPixelShader(CompileShader(ps_5_0, PSPointLight()));    //����������ɫ��
	}
}


//�۹�ƹ�ԴTechnique
technique11 T_SpotLight
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));   //���ö�����ɫ��
		SetPixelShader(CompileShader(ps_5_0, PSSpotLight()));    //����������ɫ��
	}
}
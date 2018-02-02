//外部变量：存储从应用程序中得到的数据的变量
//外部变量一般存储在常量缓存中，常量缓存的声明方法近似于结构的声明
//但关键字是cbuffer
//register(b0)表示手动将着色器变量绑定到特定寄存器，其中b表示常量缓存
//0表示寄存器编号，本例中删除“:register（b0）”效果一样

cbuffer MatrixBuffer
{
	matrix World;             //世界变换矩阵，其中matrix表示一个4x4的矩阵
	matrix View;              //观察变换矩阵
	matrix Projection;        //投影变换矩阵
	float4 EyePosition;
};

cbuffer MaterialBuffer
{
	float4 MatAmbient;
	float4 MatDiffuse;
	float4 MatSpecular;
	float MatPower;
}

//光源的常量缓存
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

Texture2D Texture;            //纹理变量

SamplerState Sampler
{
	Filter = MIN_MAG_MIP_LINEAR;  //采用线性过滤
	AddressU = WRAP;            //寻址模式为Wrap
	AddressV = WRAP;
};

//定义顶点结构//
//顶点输入结构
//这里顶点输入结构包含位置信息和颜色信息
//HLSL语言中，变量的声明一般按照“类型  变量名：语义”的格式声明

struct VS_INPUT
{
	float4 Pos:POSITION;     //位置//float4是类型，SV_POSITION为语义定义位置
	float3 Norm:NORMAL;      //法向量
	float2 Tex:TEXCOORD0;    //纹理坐标
};

//顶点着色器的输出结构
struct VS_OUTPUT
{
	float4 Pos:SV_POSITION;
	float2 Tex:TEXCOORD0;
	float3 Norm:TEXCOORD1;
	float4 ViewDirection: TEXCOORD2;
	float4 LightVector:TEXCOORD3;
};

//顶点着色器函数
//本例中顶点信息包含位置和颜色，所以返回类型为我们定义的顶点结构VS_INPUT

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;          //定义一个顶点结构的对象
	output.Pos = mul(input.Pos, World);       //世界坐标变换，mul为矩阵乘法的函数//在input坐标上进行世界变换
	output.Pos = mul(output.Pos, View);       //观察坐标变换，mul第一个参数为向量
	output.Pos = mul(output.Pos, Projection); //投影坐标变换，mul第二个参数为矩阵

	output.Norm = mul(input.Norm, (float3x3)World);
	output.Norm = normalize(output.Norm);

	float4 worldPosition = mul(input.Pos, World);
		output.ViewDirection = EyePosition - worldPosition;
	output.ViewDirection = normalize(output.ViewDirection);

	output.LightVector = LightPosition - worldPosition;
	output.LightVector = normalize(output.LightVector);
	output.LightVector.w = length(LightPosition - worldPosition);

	output.Tex = input.Tex;                    //纹理设置
	return output;                           //返回VS_INPUT的对象
}

//像素着色器函数
//和第5章的像素着色器的不同之处在于，参数为一个VS_INPUT的对象，即顶点着色器的输出值

float4 PS(VS_OUTPUT input) :SV_Target
{
	return Texture.Sample(Sampler, input.Tex);    //返回纹理
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
	finalColor.a = texColor.a*MatDiffuse.a;  //计算透明度

	return finalColor;    //返回纹理
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

	//光照向量等于顶点的光照向量
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

//Technique将不同着色器整合起来实现某种功能
//定义Technique由technique11关键字进行

technique11 TexTech
{
	//Technique通过Pass来应用不同的效果，每个Technique可以有多个Pass
	//本例只有一个Pass
	pass P0
	{
		//设置顶点着色器
		//CompileShader包含两个参数，一个是目标着色器版本，另一个为我们定义的着色器函数
		SetVertexShader(CompileShader(vs_5_0, VS()));   //设置顶点着色器
		SetGeometryShader(NULL);                        //几何着色器
		SetPixelShader(CompileShader(ps_5_0, PS()));    //设置像素着色器
	}
}


//方向光Technique
technique11 T_DirLight
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));   //设置顶点着色器
		SetPixelShader(CompileShader(ps_5_0, PSDirectionalLight()));    //设置像素着色器
	}
}


//点光源Technique
technique11 T_PointLight
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));   //设置顶点着色器
		SetPixelShader(CompileShader(ps_5_0, PSPointLight()));    //设置像素着色器
	}
}


//聚光灯光源Technique
technique11 T_SpotLight
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));   //设置顶点着色器
		SetPixelShader(CompileShader(ps_5_0, PSSpotLight()));    //设置像素着色器
	}
}
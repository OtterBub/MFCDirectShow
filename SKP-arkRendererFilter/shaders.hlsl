[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}

// ----- Vertex Shader -----
struct vs_in {
	float3 position_local : POS;
};

struct vs_out {
	float4 position_clip : SV_POSITION;
};

vs_out vs_main(vs_in input) {
	vs_out output = (vs_out)0;
	output.position_clip = float4(input.position_local, 2.0);
	return output;
}

float4 ps_main(vs_out input) : SV_TARGET{
	return float4(0.0, 0.0, 1.0, 1.0);
}

// ---- Texture Shader ----

struct VS_Tex_Input {
	float2 pos : POS;
	float2 uv : TEX;
};

struct VS_Tex_Output {
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D mytexture : register(t0);
SamplerState mysampler : register(s0);

VS_Tex_Output vs_texture(VS_Tex_Input input)
{
	VS_Tex_Output output;
	output.pos = float4(input.pos, 0.0f, 0.8f);
	output.uv = input.uv;
	return output;

}

float4 ps_texture(VS_Tex_Output input) : SV_TARGET
{
	float4 textureColor;

	textureColor = mytexture.Sample(mysampler, input.uv);
	return textureColor;
}
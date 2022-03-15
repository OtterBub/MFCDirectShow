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

// ----- Pixel Shader -----

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 tex : TEXCOORD0;
};

float4 ps_texture(PixelInputType input) : SV_TARGET
{
	float4 textureColor;

	textureColor = shaderTexture.Sample(SampleType, input.tex);
	return textureColor;
}
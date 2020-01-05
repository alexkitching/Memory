struct VSOut
{
	float3 colour : COLOUR;
	float4 pos : SV_POSITION;
};

cbuffer CBuf
{
	row_major matrix transform;
};

VSOut main(float2 pos : POSITION, float3 colour : COLOUR)
{
	VSOut vso;
	vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transform);
	vso.colour = colour;

	return vso;
}
struct VSOut
{
	float3 colour : COLOUR;
	float4 pos : SV_POSITION;
};

VSOut main(float2 pos : POSITION, float3 colour : COLOUR)
{
	VSOut vso;
	vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
	vso.colour = colour;

	return vso;
}
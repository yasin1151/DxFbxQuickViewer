struct PixelIn
{
	float4 Position : SV_POSITION;
	float4 Normal : NORMAL;
};

float4 main(PixelIn pin) : SV_TARGET
{
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}
struct VertexIn
{
	float4 Position : POSITION;
	float4 Normal : NORMAL;
};

struct PixelIn
{
	float4 Position : SV_POSITION;
	float4 Normal : NORMAL;
};

cbuffer PerFrame : register(b0)
{
	matrix mModel;
	matrix mView;
	matrix mProj;
};

PixelIn main(VertexIn vin)
{
	PixelIn vout;

	vout.Position = mul(vin.Position, mModel);
	vout.Position = mul(vout.Position, mView);
	vout.Position = mul(vout.Position, mProj);

	vout.Normal = mul(vin.Normal, mModel);

	return vout;
}
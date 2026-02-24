//==================================================================================
//
// スカイボックスシェーダー [skybox.fx]
// Author : TENMA
//
//==================================================================================

// 変換行列
float4x4 WorldViewProj : WORLDVIEWPROJECTION;

// スカイボックス
texture Skybox : TEXTUREFILENAME;	// テクスチャポインタ
samplerCUBE SkyboxSampler = sampler_state
{
	Texture = <Skybox>;
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

// 入出力構造体
struct VS_INPUT
{
    float4 position : POSITION;		// 位置
};

struct VS_OUTPUT
{
	float4 position : POSITION;		// 頂点座標
	float3 tex : TEXCOORD0;			// ルックアップベクトル
};

//===================================================================
//
// 頂点シェーダー関連
//
//===================================================================

// 頂点シェーダー
VS_OUTPUT Skybox_VS(VS_INPUT In)
{
    VS_OUTPUT output;

    // カメラは常にスカイボックスの中心にあるようにする
    output.tex = normalize(In.position.xyz);

    // プロジェクション行列を使って頂点をクリップ空間へ変換
    output.position = mul(In.position, WorldViewProj);
    output.position.z = output.position.w;

    return output;
}

//===================================================================
//
// ピクセルシェーダー関連
//
//===================================================================

// ピクセルシェーダー
float4 Skybox_PS(VS_OUTPUT input) : COLOR0
{
    // キューブマップからテクスチャをサンプリング
    return texCUBE(SkyboxSampler, normalize(input.tex));
}

//===================================================================
//
// テクニック・パス関連
//
//===================================================================

// テクニック
technique SkyTec
{
    // パス
    pass Skybox
    {
        // 頂点シェーダーをコンパイル
        VertexShader = compile vs_3_0 Skybox_VS();

        // ピクセルシェーダーをコンパイル
        PixelShader = compile ps_3_0 Skybox_PS();
    }
}
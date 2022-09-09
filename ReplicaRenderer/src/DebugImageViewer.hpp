#pragma once
#include "MinWindow.hpp"
#include <imgui.h>
#include <imfilebrowser.h>
#include <DirectXTex.h>

#include "D3D11/Shaders/BuiltInShaders.hpp"
#include "D3D11/RenderComponent.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/Mesh.hpp"
#include "D3D11/Primitives.hpp"
#include "D3D11/Resources/RWTexture2D.hpp"
#include "D3D11/Resources/Sampler.hpp"
#include "D3D11/Effect.hpp"
#include "D3D11/Shaders/ComputeShader.hpp"

using DirectX::ScratchImage;

namespace Replica::D3D11
{
	class DebugImageViewer : public RenderComponentBase
	{
	public:
		DebugImageViewer(Renderer& renderer) : 
            RenderComponentBase(renderer),
            computeShader(renderer.GetDevice(), g_TestCS),
            samp(renderer.GetDevice(), TexFilterMode::LINEAR, TexClampMode::BORDER),
            texQuadEffect(renderer.GetDevice(), g_PosTextured2DEffect),
            tex(GetDevice(), Formats::R8G8B8A8_UNORM)
        {
            fileDialog.SetTitle("Load Image");
            fileDialog.SetTypeFilters({".bmp", ".gif", ".ico", ".jpg", ".png", ".tiff", ".tif", ".dds"});

            MeshDef quadDef = Primitives::GeneratePlane<VertexPos2D>(ivec2(0), 2.0f);
            quad = Mesh(renderer.GetDevice(), quadDef);
        }

        void DrawUI(Context& ctx)
        {
            if (ImGui::Begin("Image Viewer Debug"))
            {
                if (ImGui::Button("Load Image"))
                    fileDialog.Open();

                if (ImGui::Button("Toggle Fullscreen"))
                {
                    GetWindow().SetFullScreen(!GetWindow().GetIsFullScreen());
                }
            }
            ImGui::End();

            fileDialog.Display();

            if (fileDialog.HasSelected())
            {
                tex.SetTextureWIC(ctx, fileDialog.GetSelected().native(), buffer);
                fileDialog.ClearSelected();
            }
        }

        void Update(Context& ctx) override
        {
            DrawUI(ctx);

            if (tex.GetIsValid())
            { 
                texQuadEffect.SetTexture(L"tex", tex);

                const vec2 bodySize = GetWindow().GetBodySize(),
                    imgSize = tex.GetSize();
                vec2 vpSize = bodySize;
                const float wndAspect = bodySize.x / bodySize.y,
                    imgAspect = imgSize.x / imgSize.y;

                if (wndAspect >= imgAspect)
                    vpSize.x = bodySize.y * imgAspect;
                else
                    vpSize.y = bodySize.x / imgAspect;

                ctx.RSSetViewport(vpSize, 0.5f * (bodySize - vpSize));
            }

            texQuadEffect.SetSampler(L"samp", samp);
            texQuadEffect.Update(ctx);
            quad.Update(ctx);
        }

		void Draw(Context& ctx) override 
		{
            quad.Draw(ctx);
		}

	private:
        ComputeShader computeShader;
        ScratchImage buffer;
		ImGui::FileBrowser fileDialog;
        RWTexture2D tex;
        Sampler samp;
        Effect texQuadEffect;
        Mesh quad;
	};
}
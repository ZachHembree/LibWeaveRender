#pragma once
#include "MinWindow.hpp"
#include <imgui.h>
#include <imfilebrowser.h>
#include "D3D11/RenderComponent.hpp"
#include "D3D11/Renderer.hpp"
#include "D3D11/Effect.hpp"
#include "D3D11/Resources/Texture2D.hpp"
#include "D3D11/Resources/Sampler.hpp"
#include "D3D11/Mesh.hpp"
#include "D3D11/Primitives.hpp"
#include "D3D11/Shaders/BuiltInShaders.hpp"

namespace Replica::D3D11
{
	class DebugImageViewer : public RenderComponentBase
	{
	public:
		DebugImageViewer(Renderer& renderer) : 
            RenderComponentBase(renderer),
            tex(Texture2D::FromImageWIC(renderer.GetDevice(), L"lena_color_512.png")),
            samp(renderer.GetDevice(), TexFilterMode::LINEAR, TexClampMode::BORDER),
            texQuadEffect(renderer.GetDevice(), g_PosTextured2DEffect)
        {
            fileDialog.SetTitle("Test dialog");

            MeshDef quadDef = Primitives::GeneratePlane<VertexPos2D>(ivec2(0), 2.0f);
            quad = Mesh(renderer.GetDevice(), quadDef);
        }

        void DrawUI()
        {
            if (ImGui::Begin("dummy window"))
            {
                // open file dialog when user clicks this button
                if (ImGui::Button("open file dialog"))
                    fileDialog.Open();

                if (ImGui::Button("Enter Fullscreen"))
                {
                    MinWindow& win = GetRenderer().GetWindow();
                    ivec2 mres = win.GetMonitorResolution();
                    win.DisableStyleFlags(WndStyle(g_DefaultWndStyle, 0));
                    win.SetBodySize(mres);
                    win.SetPos(ivec2(0));
                }

                if (ImGui::Button("Exit Fullscreen"))
                {
                    MinWindow& win = GetRenderer().GetWindow();
                    win.EnableStyleFlags(WndStyle(g_DefaultWndStyle, 0));
                    win.SetBodySize(vec2(1280, 800));
                }
            }
            ImGui::End();

            fileDialog.Display();

            if (fileDialog.HasSelected())
            {
                tex = Texture2D::FromImageWIC(GetRenderer().GetDevice(), fileDialog.GetSelected().c_str());
                fileDialog.ClearSelected();
            }
        }

        void Update(Context& ctx) override
        {
            DrawUI();

            texQuadEffect.SetSampler(L"samp", samp);
            texQuadEffect.SetTexture(L"tex", tex);
            texQuadEffect.Update(ctx);
            quad.Update(ctx);

            const vec2 bodySize = GetRenderer().GetWindow().GetBodySize(),
                imgSize = tex.GetSize();
            vec2 vpSize = bodySize;
            const float wndAspect = bodySize.x / bodySize.y,
                imgAspect = imgSize.x / imgSize.y;

            if (wndAspect >= imgAspect)
            {
                vpSize.x = bodySize.y * imgAspect;
            }
            else
            {
                vpSize.y = bodySize.x / imgAspect;
            }

            ctx.RSSetViewport(vpSize, 0.5f * (bodySize - vpSize));
        }

		void Draw(Context& ctx) override 
		{
            quad.Draw(ctx);
		}

	private:
		ImGui::FileBrowser fileDialog;
        Texture2D tex;
        Sampler samp;
        Effect texQuadEffect;
        Mesh quad;
	};
}
#pragma once

#include "d3dinit.h"

/**
 * Class that defines runtime behavior of the program.
 * 
 * Inherits from D3DBase, creates and maintains all other objects.
 */
class D3DApplication : public D3DBase
{
	std::unique_ptr<Camera>								mCamera = nullptr;

	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

private:
	void D3DBase::InitializeComponents() override
	{
		LoadResources();
		mCamera = std::make_unique<Camera>(DirectX::XMVectorSet(5.0f, 2.0f, 5.0f, 1.0f),
			DirectX::XM_PI * 7 / 4, -0.2f, mTimer.get());

		BuildShadersAndInputLayout();
		BuildPSO();
	}

private:
	void LoadResources();
	void BuildShadersAndInputLayout();			// Compiles shaders and defines input layout
	void BuildPSO();							// Configures rendering pipeline

	void DrawRenderItems();						// Draw every render item

	void UpdatePassCB();						// Update and store in CB pass constants

	void Update() override;
	void Draw() override;
	void OnResize() override
	{
		D3DBase::OnResize();
		// Update/set projection matrix as it only depends on aspect ratio
		DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi,
			AspectRatio(), 1.0f, 1000.0f);
		XMStoreFloat4x4(&mProj, P);
	}

	void OnMouseDown(WPARAM btnState, int x, int y) override;
	void OnMouseUp(WPARAM btnState, int x, int y) override;
	void OnMouseMove(WPARAM btnState, int x, int y) override;

};

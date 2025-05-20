#pragma once

#include <d3d12.h>
#include <string>
#include <unordered_map>

#include <DDSTextureLoader.h>
#include <ResourceUploadBatch.h>

struct TEXTURE2D_DESC
{
	std::string name;
	std::string file;
};

class Texture2D
{
public:
	static std::unordered_map<std::string, std::unique_ptr<Texture2D>>& Texture2DDictionary()
	{
		static std::unordered_map<std::string, std::unique_ptr<Texture2D>> map;
		return map;
	}

	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DescriptorHeap;

	D3D12_GPU_DESCRIPTOR_HANDLE GetSRV() const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE textureHandle = DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		textureHandle.ptr += static_cast<UINT64>(SRVHeapIndex) * cbvSrvDescriptorSize;
		return textureHandle;
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> pTexture = nullptr;

	UINT SRVHeapIndex = 0;
	static UINT cbvSrvDescriptorSize;
	Texture2D(ID3D12Resource**& ppResource)
	{
		ppResource = pTexture.GetAddressOf();
	}

	friend class Texture2DAssembler;
};

class Texture2DAssembler
{
public:
	static void AssembleTexture(const TEXTURE2D_DESC& desc,
		ID3D12Device* pDevice,
		ID3D12CommandQueue* pQueue)
	{
		// Retrieve the underlying resource pointer
		ID3D12Resource** ppResource = nullptr;
		Texture2D::Texture2DDictionary()[desc.name] = std::make_unique<Texture2D>(ppResource);
	
		// The resource has been added to the dictionary, now create it

		std::wstring path = L"resources\\textures\\";
		path += AnsiToWString(desc.file);

		DirectX::ResourceUploadBatch upload(pDevice);
		upload.Begin();

		DirectX::CreateDDSTextureFromFile(pDevice,
			upload,
			path.c_str(),
			ppResource);

		auto finish = upload.End(pQueue);
		finish.wait();
	}

	static void AssembleCBVHeap(ID3D12Device* pDevice)
	{
		// Create SRV heap
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = Texture2D::Texture2DDictionary().size();
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		srvHeapDesc.NodeMask = 0;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&srvHeapDesc,
			IID_PPV_ARGS(Texture2D::DescriptorHeap.GetAddressOf())));
	
		// Create SRVs for every texture

		D3D12_CPU_DESCRIPTOR_HANDLE handle = Texture2D::DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		Texture2D::cbvSrvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		int i = 0;
		for (const auto& entry : Texture2D::Texture2DDictionary())
		{
			// Create SRV for this texture
			Texture2D* tex = entry.second.get();
			tex->SRVHeapIndex = i;

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = tex->pTexture->GetDesc().Format;
			srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = tex->pTexture->GetDesc().MipLevels;
			srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

			pDevice->CreateShaderResourceView(tex->pTexture.Get(), &srvDesc, handle);

			handle.ptr += Texture2D::cbvSrvDescriptorSize;
			i++;
		}
	}
};

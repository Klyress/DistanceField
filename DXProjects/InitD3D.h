#pragma once
#include "D3D11.h"
#define DOWN_SAMPLE 1

struct D3DContext
{
	D3D_DRIVER_TYPE         m_driverType;
	std::vector<ID3D11Device*>  m_pd3dDevices;
	D3D_FEATURE_LEVEL       m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	std::vector<ID3D11DeviceContext*>    m_pImmediateContext;
	IDXGISwapChain*         m_pSwapChain = nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
	ID3D11Texture2D*		m_pBackBuffer;
	ID3D11Texture2D*		m_pWorkBuffer[2];
	ID3D11Texture2D*		m_pMiniWorkBuffer[2];
	ID3D11Texture2D*		m_pRNGStateBuffer[2];
	ID3D11Texture2D*		m_pDepthBuffer[2];

	std::vector<IDXGIAdapter1*>	m_pAdapters;

	UINT viewportWidth;
	UINT viewportHeight;

	D3DContext() {   };
	// this function initializes and prepares Direct3D for use
	HRESULT InitD3D(HWND hWnd)
	{
		HRESULT hr = S_OK;

		RECT rc;
		GetClientRect(hWnd, &rc);
		UINT width = rc.right - rc.left;
		UINT height = rc.bottom - rc.top;

		UINT createDeviceFlags = 0;
#ifdef _DEBUG
		//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		IDXGIFactory1 * pFactory = NULL;

		CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory);

		UINT adapterIndex = 0;
		IDXGIAdapter1* pAdapter;
		while (pFactory->EnumAdapters1(adapterIndex, &pAdapter) == S_OK)
		{
			m_pAdapters.push_back(pAdapter);
			adapterIndex++;
		}
#ifdef _DEBUG
		if (m_pAdapters.size() > 1)
		{
			wprintf(L"Multi adapters found.\n");
			for_each(m_pAdapters.cbegin(), m_pAdapters.cend(), 
				[=](IDXGIAdapter1* a)
				{
					DXGI_ADAPTER_DESC1 desc;
					a->GetDesc1(&desc);
					wprintf(L"%s\n", desc.Description);
				}
			);
		}
#endif

		D3D_FEATURE_LEVEL featureLevels [] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		UINT numFeatureLevels = ARRAYSIZE(featureLevels);
		
		int num = 1;
		for each(auto adapter in m_pAdapters)
		{
			ID3D11Device* device;
			HRESULT hr;

			ID3D11DeviceContext* context;
			hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
				D3D11_SDK_VERSION, &device, &m_featureLevel, &context);

			if (hr == E_INVALIDARG)
			{
				// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
				hr = D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
					D3D11_SDK_VERSION, &device, &m_featureLevel, &context);
			}

			m_pImmediateContext.push_back(context);
			if (device == nullptr)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				WCHAR msg[256];
				wsprintf(msg, L"Failed to create device upon adapter %s %d", desc.Description, num);
				MessageBoxW(hWnd, msg, L"Warning", MB_OK);
				exit(0);
			}
			else
			{
				num++;
				m_pd3dDevices.push_back(device);
			}

		}
		{
		// DirectX 11.0 systems
			DXGI_SWAP_CHAIN_DESC sd;
			ZeroMemory(&sd, sizeof(sd));
			sd.BufferCount = 1;
			sd.BufferDesc.Width = width;
			sd.BufferDesc.Height = height;
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferDesc.RefreshRate.Numerator = 60;
			sd.BufferDesc.RefreshRate.Denominator = 1;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
			sd.OutputWindow = hWnd;
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			sd.Windowed = TRUE;

			// choose GPU 0 as master GPU
			hr = pFactory->CreateSwapChain(m_pd3dDevices[0], &sd, &m_pSwapChain);
		}

		// Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
		pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

		pFactory->Release();

		if (FAILED(hr))
			return hr;

		// Create a render target view
		m_pBackBuffer = nullptr;

		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pBackBuffer));
		if (FAILED(hr))
			return hr;

		hr = m_pd3dDevices[0]->CreateRenderTargetView(m_pBackBuffer, nullptr, &m_pRenderTargetView);
		if (FAILED(hr))
			return hr;

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = width*DOWN_SAMPLE;
		desc.Height = height*DOWN_SAMPLE;
		desc.ArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		desc.BindFlags = DXGI_USAGE_RENDER_TARGET_OUTPUT | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count = 1;
		m_pd3dDevices[0]->CreateTexture2D(&desc, NULL, &m_pWorkBuffer[0]);
		m_pd3dDevices[0]->CreateTexture2D(&desc, NULL, &m_pWorkBuffer[1]);


		// create two r/w texture for storing RNG states
		unsigned int* initialData = (unsigned int*)malloc(width*height*4 *4);

		desc.Width = width*DOWN_SAMPLE;
		desc.Height = height*DOWN_SAMPLE;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32_UINT;
		desc.BindFlags = DXGI_USAGE_RENDER_TARGET_OUTPUT | D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.SampleDesc.Count = 1;
		for (unsigned int i = 0; i < desc.Width*desc.Height; i++)
		{
			rand_s(&initialData[i]);
		}
		D3D11_SUBRESOURCE_DATA data[1];
		data[0].pSysMem = initialData;
		data[0].SysMemPitch = desc.Width * 4;
		data[0].SysMemSlicePitch = desc.Width*desc.Height *4;
		m_pd3dDevices[0]->CreateTexture2D(&desc, data, &m_pRNGStateBuffer[0]);

		for (unsigned int i = 0; i < desc.Width*desc.Height; i++)
		{
			rand_s(&initialData[i]);
		}
		m_pd3dDevices[0]->CreateTexture2D(&desc, data, &m_pRNGStateBuffer[1]);

		for (unsigned int i = 0; i < desc.Width*desc.Height; i++)
		{
			initialData[i] = D3D10_FLOAT32_MAX;
		}
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		m_pd3dDevices[0]->CreateTexture2D(&desc, data, &m_pDepthBuffer[0]);
		m_pd3dDevices[0]->CreateTexture2D(&desc, data, &m_pDepthBuffer[1]);

		desc.Width = width*DOWN_SAMPLE >> 1;
		desc.Height = height*DOWN_SAMPLE >> 1;
		m_pd3dDevices[0]->CreateTexture2D(&desc, NULL, &m_pMiniWorkBuffer[0]);
		m_pd3dDevices[0]->CreateTexture2D(&desc, NULL, &m_pMiniWorkBuffer[1]);

		//m_pd3dDevice->CreateTexture2D(&desc, NULL, &m_pRNGStateBuffer[0]);
		//m_pd3dDevice->CreateTexture2D(&desc, NULL, &m_pRNGStateBuffer[1]);

		free(initialData);

		m_pImmediateContext[0]->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);

		// Setup the viewport
		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT) width;
		vp.Height = (FLOAT) height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_pImmediateContext[0]->RSSetViewports(1, &vp);

		viewportWidth = width;
		viewportHeight = height;

		return hr;
	};

	void CleanContext()
	{
		if (m_pRenderTargetView) m_pRenderTargetView->Release();
		if (m_pSwapChain) m_pSwapChain->Release();
		for_each(m_pd3dDevices.cbegin(), m_pd3dDevices.cend(), [=](ID3D11Device* device) {if (device) device->Release(); });
		for_each(m_pImmediateContext.cbegin(), m_pImmediateContext.cend(), [=](ID3D11DeviceContext* context) {context->ClearState(); context->Release(); });
	};

	ID3D11Texture2D* m_pTexture;
	void CreateTexture()
	{
		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = viewportWidth;
		desc.Height = viewportHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

	}
};
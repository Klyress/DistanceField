#pragma once
#include <vector>
#include "D3D11.h"
#include <algorithm>
#include <amp.h>
#include <amp_graphics.h>
#include <amp_math.h>

using namespace concurrency;
using namespace concurrency::graphics;
using namespace concurrency::graphics::direct3d;
using namespace concurrency::direct3d;

class AMPRayMatcherRenderer
{
public:
	AMPRayMatcherRenderer(std::vector<ID3D11Device*>& devices)
	{
		m_devices = devices;
	}
	void Init() 
	{
		for_each(m_devices.cbegin(), m_devices.cend(), 
			[=](ID3D11Device* device)
			{
				accelerator_view dxView = create_accelerator_view(device);
				m_accViews.push_back(dxView);
			}
		);
		
	}
	void Render() 
	{

	}

private:

	std::vector<ID3D11Device*> m_devices;
	std::vector<accelerator_view>  m_accViews;
};
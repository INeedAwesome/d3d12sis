#include "sspch.h"
#include "HelloTriangle.h"

namespace SS {

	HelloTriangle::HelloTriangle()
	{
	}

	bool HelloTriangle::Start()
	{
		if (!Init())
			return false;
		Running = true;
		while (Running)
		{
			while (!glfwWindowShouldClose(m_glfwWindow))
			{
				Update();
				Render();
			}
		}
	}

	bool HelloTriangle::Init()
	{
		printf("Hello World!\n");

		if (!glfwInit())
			return 1;

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		std::string windowTitle = "D3D12: SIS";
		m_glfwWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle.c_str(), 0, 0);
		if (!m_glfwWindow)
			return 2;

		m_hwnd = glfwGetWin32Window(m_glfwWindow);
		glfwMakeContextCurrent(m_glfwWindow);

		if (!InitializeDirectX12Pipeline())
			return 3;

	}

	void HelloTriangle::Update()
	{
		glfwPollEvents();
	}

	void HelloTriangle::Render()
	{
		glfwSwapBuffers(m_glfwWindow);
	}

	void HelloTriangle::Terminate()
	{
		glfwTerminate();
	}

	bool HelloTriangle::InitializeDirectX12Pipeline()
	{
		using namespace Microsoft::WRL;
		uint32_t dxgiFactoryFlags = 0;

#ifdef _DEBUG
		ComPtr<ID3D12Debug> debugController;
		if (D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif // _DEBUG

		ComPtr<IDXGIFactory4> factory;
		CreateDXGIFactory(IID_PPV_ARGS(&factory));

		// enable warp
		// https://learn.microsoft.com/en-us/windows/win32/direct3darticles/directx-warp

		ComPtr<IDXGIDevice> warpAdapter;
		factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));

		// creating device with warp adapter and feature lvl 12
		D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device));

		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
		{
			commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			commandQueueDesc.NodeMask = 0; // only one gpu
		}

		m_Device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_CommandQueue));

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		{
			swapChainDesc.BufferCount = m_frameCount;
			swapChainDesc.Width = WINDOW_WIDTH;
			swapChainDesc.Height = WINDOW_HEIGHT;
			swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.SampleDesc.Count = 1;
		}

		ComPtr<IDXGISwapChain1> swapChain;
		factory->CreateSwapChainForHwnd(m_CommandQueue.Get(), m_hwnd, &swapChainDesc, nullptr, nullptr, &swapChain);

		factory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER);

		swapChain.As(&m_swapChain);
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		{
			// Describe and create a render target view (RTV) descriptor heap.
			D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			rtvHeapDesc.NumDescriptors = m_frameCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));

			m_rtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}

	}

}
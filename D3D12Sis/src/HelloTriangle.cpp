#include "sspch.h"
#include "HelloTriangle.h"
#include "d3dx12.h"
#include <comdef.h>

namespace SS {

	HelloTriangle::HelloTriangle()
	{
	}

	bool HelloTriangle::Start()
	{
		if (!Init())
		{
			printf("Could not initialize p:SIS!");
			return false;
		}
		
		m_Running = true;
		while (m_Running)
		{
			Update();
			Render();

			if (glfwWindowShouldClose(m_glfwWindow))
				m_Running = false;
		
		}
		Terminate();
	}

	bool HelloTriangle::Init()
	{
		printf("Hello World!\n");

		if (!glfwInit())
		{
			printf("Could not initialize GLFW!\n");
			return false;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		std::string windowTitle = "D3D12: SIS";
		m_glfwWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle.c_str(), 0, 0);
		if (!m_glfwWindow)
		{
			printf("Could not initialize GLFW window!\n");
			return false;
		}

		m_hwnd = glfwGetWin32Window(m_glfwWindow);
		glfwMakeContextCurrent(m_glfwWindow);

		if (!InitializePipeline())
		{
			printf("Could not initialize DirectX12 Pipeline!\n");
			return false;
		}
		if (!InitializeAssets())
		{
			printf("Could not initialize DirectX12 Assets!\n");
			return false;
		}
		return true;

	}

	void HelloTriangle::Update()
	{
		glfwPollEvents();
	}

	void HelloTriangle::Render()
	{
		PopulateCommandList();

		ID3D12CommandList* pCommandList[] = { m_CommandList.Get() };
			
		m_CommandQueue->ExecuteCommandLists(_countof(pCommandList), pCommandList);

		m_SwapChain->Present(1, 0);
	
		glfwSwapBuffers(m_glfwWindow);
	}

	void HelloTriangle::Terminate()
	{
		WaitForPreviousFrame();
		CloseHandle(m_FenceEvent);

		glfwTerminate();
	}

	bool HelloTriangle::InitializePipeline()
	{
		using namespace Microsoft::WRL;
		uint32_t dxgiFactoryFlags = 0;

#ifdef SS_DEBUG
		ComPtr<ID3D12Debug> debugController;
		if (D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))
		{
			debugController->EnableDebugLayer();
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif // SS_DEBUG

		ComPtr<IDXGIFactory4> factory;
		CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));

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
			swapChainDesc.BufferCount = m_FrameCount;
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

		swapChain.As(&m_SwapChain);
		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		{
			rtvHeapDesc.NumDescriptors = m_FrameCount;
			rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		}
		m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvHeap));
		m_rtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// Create frame resources.
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
			
			// Create a RTV for each frame.
			for (UINT i = 0; i < m_FrameCount; i++)
			{
				m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
				m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);
				rtvHandle.Offset(1, m_rtvDescriptorSize);
			}
		}

		m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));

		return true;

	}

	bool HelloTriangle::InitializeAssets()
	{
		{
			CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
			rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;
			D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
			m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		}

		m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList));

		m_CommandList->Close();

		// Create synchronization objects.
		{
			m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
			m_FenceValue = 1;

			// Create an event handle to use for frame synchronization.
			m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			if (m_FenceEvent == nullptr)
			{
				HRESULT_FROM_WIN32(GetLastError());
			}
		}

		return true;
	}

	void HelloTriangle::PopulateCommandList()
	{
		m_CommandAllocator->Reset();
		this->m_CommandList->Reset(m_CommandAllocator.Get(), m_PipelineState.Get());
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_rtvDescriptorSize);


		const float clearColor[] = {0.1f, 0.2f, 0.5f, 1.0f};
		m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, 0);

		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			m_RenderTargets[m_FrameIndex].Get(), 
			D3D12_RESOURCE_STATE_PRESENT, 
			D3D12_RESOURCE_STATE_RENDER_TARGET));
		m_CommandList->Close();
	}

	void HelloTriangle::WaitForPreviousFrame()
	{
		const UINT64 fence = m_FenceValue;
		m_CommandQueue->Signal(m_Fence.Get(), fence);
		m_FenceValue++;

		// Wait until the previous frame is finished.
		if (m_Fence->GetCompletedValue() < fence)
		{
			m_Fence->SetEventOnCompletion(fence, m_FenceEvent);
			WaitForSingleObject(m_FenceEvent, INFINITE);
		}

		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
	}

}
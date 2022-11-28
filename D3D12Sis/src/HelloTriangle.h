#pragma once
#include "sspch.h"

using Microsoft::WRL::ComPtr;

namespace SS {

	class HelloTriangle
	{
	public:
		HelloTriangle();
		
		bool Start();
	private:
		bool Init();
		void Update();
		void Render();
		void Terminate();

		bool InitializePipeline();
		bool InitializeAssets();

		void PopulateCommandList();
		void WaitForPreviousFrame();

		int WINDOW_WIDTH = 1280;
		int WINDOW_HEIGHT = 720;
		bool m_Running = false;

		static const uint32_t m_FrameCount = 2;

		GLFWwindow* m_glfwWindow;
		HWND m_hwnd;

		ComPtr<ID3D12Device> m_Device;
		ComPtr<IDXGISwapChain3> m_SwapChain;
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12Resource> m_RenderTargets[m_FrameCount];
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;

		ComPtr<ID3D12RootSignature> m_RootSignature;
		ComPtr<ID3D12PipelineState> m_PipelineState;

		ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
		uint32_t m_rtvDescriptorSize;

		// sync objects
		uint32_t m_FrameIndex;
		HANDLE m_FenceEvent;
		ComPtr<ID3D12Fence> m_Fence;
		uint64_t m_FenceValue;
	};
}

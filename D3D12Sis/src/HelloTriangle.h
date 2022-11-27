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

		bool InitializeDirectX12Pipeline();

		int WINDOW_WIDTH = 1280;
		int WINDOW_HEIGHT = 720;
		bool Running = false;

		uint32_t m_frameCount = 2;

		GLFWwindow* m_glfwWindow;
		HWND m_hwnd;

		ComPtr<ID3D12Device> m_Device;
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<IDXGISwapChain3> m_swapChain;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;

		uint32_t m_rtvDescriptorSize;

		// sync objects
		uint32_t m_frameIndex;
		HANDLE m_fenceEvent;
		ComPtr<ID3D12Fence> m_fence;
		uint64_t m_fenceValue;
	};
}

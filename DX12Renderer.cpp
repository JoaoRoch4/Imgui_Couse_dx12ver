////*********************************************************
////
//// Copyright (c) Microsoft. All rights reserved.
//// This code is licensed under the MIT License (MIT).
//// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
//// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
//// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
////
////*********************************************************
//
//#include "pch.hpp"
//#include "DX12Renderer.hpp"
//#include "Classes.hpp"
//
//extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 618; }
//extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\"; }
//
//DX12Renderer::DX12Renderer(UINT width, UINT height, std::wstring name) :
//    DXSample(width, height, name),
//    m_frameIndex(0),
//    m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
//    m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
//    m_rtvDescriptorSize(0)
//{
//}
//
//void DX12Renderer::OnInit()
//{
//    //LoadAssets();
//}
//
//// Load the rendering pipeline dependencies.
//bool DX12Renderer::LoadPipeline(WindowManager *window)
//{
//   
//    UINT dxgiFactoryFlags = 0;
//
//#if defined(_DEBUG)
//    // Enable the debug layer (requires the Graphics Tools "optional feature").
//    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
//    {
//        ComPtr<ID3D12Debug> debugController;
//        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
//        {
//            debugController->EnableDebugLayer();
//
//            // Enable additional debug layers.
//            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
//        }
//    }
//#endif
//
//    ComPtr<IDXGIFactory4> factory;
//    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
//
//    if (m_useWarpDevice)
//    {
//        ComPtr<IDXGIAdapter> warpAdapter;
//        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
//
//        ThrowIfFailed(D3D12CreateDevice(
//            warpAdapter.get(),
//            D3D_FEATURE_LEVEL_11_0,
//            IID_PPV_ARGS(&m_device)
//            ));
//    }
//    else
//    {
//        ComPtr<IDXGIAdapter1> hardwareAdapter;
//        GetHardwareAdapter(factory.get(), hardwareAdapter.put());
//
//        ThrowIfFailed(D3D12CreateDevice(
//            hardwareAdapter.get(),
//            D3D_FEATURE_LEVEL_11_0,
//            IID_PPV_ARGS(&m_device)
//            ));
//    }
//
//    // Describe and create the command queue.
//    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
//    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//
//    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));
//
//    // Describe and create the swap chain.
//    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
//    swapChainDesc.BufferCount = FrameCount;
//    swapChainDesc.Width = m_width;
//    swapChainDesc.Height = m_height;
//    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//    swapChainDesc.SampleDesc.Count = 1;
//
//    ComPtr<IDXGISwapChain1> swapChain;
//    ThrowIfFailed(factory->CreateSwapChainForHwnd(
//        m_commandQueue.get(),        // Swap chain needs the queue so that it can force a flush on it.
//        window->GetHWND(),
//        &swapChainDesc,
//        nullptr,
//        nullptr,
//        swapChain.put()
//        ));
//
//    // This sample does not support fullscreen transitions.
//    ThrowIfFailed(factory->MakeWindowAssociation(window->GetHWND(), DXGI_MWA_NO_ALT_ENTER));
//
//
//    ThrowIfFailed(swapChain.try_as(__uuidof(m_swapChain.get())));
//    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
//
//    // Create descriptor heaps.
//    {
//        // Describe and create a render target view (RTV) descriptor heap.
//        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
//        rtvHeapDesc.NumDescriptors = FrameCount;
//        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//        ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
//
//        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//    }
//
//    // Create frame resources.
//    {
//        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
//
//        // Create a RTV for each frame.
//        for (UINT n = 0; n < FrameCount; n++)
//        {
//            ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
//            m_device->CreateRenderTargetView(m_renderTargets[n].get(), nullptr, rtvHandle);
//            rtvHandle.Offset(1, m_rtvDescriptorSize);
//        }
//    }
//
//    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
//        return 0;
//
//}
//
//
//// Update frame-based values.
//void DX12Renderer::OnUpdate()
//{
//}
//
//// Render the scene.
//void DX12Renderer::OnRender()
//{
//    // Record all the commands we need to render the scene into the command list.
//    PopulateCommandList();
//
//    // Execute the command list.
//    ID3D12CommandList* ppCommandLists[] = { m_commandList.get() };
//    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
//
//    // Present the frame.
//    ThrowIfFailed(m_swapChain->Present(1, 0));
//
//    WaitForPreviousFrame();
//}
//
//void DX12Renderer::OnDestroy()
//{
//    // Ensure that the GPU is no longer referencing resources that are about to be
//    // cleaned up by the destructor.
//    WaitForPreviousFrame();
//
//    CloseHandle(m_fenceEvent);
//}
//
//void DX12Renderer::PopulateCommandList()
//{
//    // Command list allocators can only be reset when the associated 
//    // command lists have finished execution on the GPU; apps should use 
//    // fences to determine GPU execution progress.
//    ThrowIfFailed(m_commandAllocator->Reset());
//
//    // However, when ExecuteCommandList() is called on a particular command 
//    // list, that command list can then be reset at any time and must be before 
//    // re-recording.
//    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.get(), m_pipelineState.get()));
//
//    // Set necessary state.
//    m_commandList->SetGraphicsRootSignature(m_rootSignature.get());
//    m_commandList->RSSetViewports(1, &m_viewport);
//    m_commandList->RSSetScissorRects(1, &m_scissorRect);
//
//    // Indicate that the back buffer will be used as a render target.
//    m_commandList->ResourceBarrier(1, CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
//
//    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
//    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
//
//    // Record commands.
//    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
//    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
//    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//    m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
//    m_commandList->DrawInstanced(3, 1, 0, 0);
//
//    // Indicate that the back buffer will now be used to present.
//    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
//
//    ThrowIfFailed(m_commandList->Close());
//}
//
//void DX12Renderer::WaitForPreviousFrame()
//{
//    // WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
//    // This is code implemented as such for simplicity. The D3D12HelloFrameBuffering
//    // sample illustrates how to use fences for efficient resource usage and to
//    // maximize GPU utilization.
//
//    // Signal and increment the fence value.
//    const UINT64 fence = m_frameIndex;
//    ThrowIfFailed(m_commandQueue->Signal(m_fence.get(), fence));
//    m_frameIndex++;
//
//    // Wait until the previous frame is finished.
//    if (m_fence->GetCompletedValue() < fence)
//    {
//        ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
//        WaitForSingleObject(m_fenceEvent, INFINITE);
//    }
//
//    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
//}

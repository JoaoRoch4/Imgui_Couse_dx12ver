#include "PCH.hpp"
#include "Main.hpp"
#include "MainWindow.hpp"
   std::unique_ptr< MainWindow> mw;


// 1. Função de procedimento de janela (lida com mensagens da janela)
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
     if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;


    switch (msg)
    {
    case WM_SIZE:
        if (mw->m_Device != nullptr && wParam != SIZE_MINIMIZED)
        {
            mw->CleanupRenderTarget();
            DXGI_SWAP_CHAIN_DESC1 desc = {};
           mw->m_pSwapChain->GetDesc1(&desc);
            HRESULT result =
                mw->m_pSwapChain->ResizeBuffers(0,
                                            static_cast<UINT> LOWORD(lParam),
                                            static_cast<UINT> HIWORD(lParam),
                                            desc.Format,
                                            desc.Flags);
            IM_ASSERT(SUCCEEDED(result) && "Failed to resize swapchain.");
            mw->CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

// 2. O ponto de entrada principal do programa Windows
_Use_decl_annotations_ int WINAPI wWinMain(_In_ HINSTANCE hInstance,
                                           _In_opt_ HINSTANCE hPrevInstance,
                                           _In_ LPWSTR lpCmdLine,
                                           _In_ int nCmdShow){
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);
    mw = std::make_unique<MainWindow>();
    return mw->launch(hInstance);
   
}


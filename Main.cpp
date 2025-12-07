#include "PCH.hpp"
#include "Main.hpp"

// 1. Função de procedimento de janela (lida com mensagens da janela)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            // Quando a janela é fechada, postamos uma mensagem para sair do loop principal
            PostQuitMessage(0);
            return 0;

        case WM_PAINT: {
            // Quando a janela precisa ser pintada (desenhada)
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Preenche o fundo da janela
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            // Escreve um texto simples no centro
            const TCHAR greeting[] = _T("Usando wWinMain!");
            TextOut(hdc,
                100, 100, // Posição X, Y
                greeting, _tcslen(greeting));

            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    // Lida com quaisquer mensagens não tratadas por nós
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// 2. O ponto de entrada principal do programa Windows
_Use_decl_annotations_ int WINAPI wWinMain(_In_ HINSTANCE hInstance,
                                           _In_opt_ HINSTANCE hPrevInstance,
                                           _In_ LPWSTR lpCmdLine,
                                           _In_ int nCmdShow){
    // Registro da classe de janela
    const TCHAR CLASS_NAME[] = _T("SampleWindowClass");

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // Criação da janela
    HWND hwnd = CreateWindowEx(
        0,                      // Estilo de janela estendido
        CLASS_NAME,             // Nome da classe que acabamos de registrar
        _T("Exemplo wWinMain"), // Título da barra de título
        WS_OVERLAPPEDWINDOW,    // Estilo de janela padrão

        // Posição e Tamanho (usar padrão do sistema)
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Janela pai
        NULL,       // Menu
        hInstance,  // Handle da instância do aplicativo
        NULL        // Parâmetros de criação adicionais
        );

    if (hwnd == NULL) {
        return 0;
    }

    // Mostrar a janela na tela
    ShowWindow(hwnd, nCmdShow);

    // 3. O loop de mensagens principal
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Retorna o valor de saída quando PostQuitMessage é chamado
    return msg.wParam;
}

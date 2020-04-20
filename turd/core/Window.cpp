#include "pch.hpp"
#include "Window.hpp"
#include "Settings.hpp"
#include "Environment.hpp"

#include <strsafe.h>

namespace turd
{
    CHAR szWindowClass[] = "TurdApp";

    void ErrorExit(LPTSTR lpszFunction)
    {
        LPVOID lpMsgBuf;
        LPVOID lpDisplayBuf;
        DWORD dw = GetLastError();

        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                      dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

        // Display the error message and exit the process

        lpDisplayBuf = (LPVOID)LocalAlloc(
            LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
        StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                        TEXT("%s failed with error %d: %s"), lpszFunction, dw, lpMsgBuf);
        MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

        LocalFree(lpMsgBuf);
        LocalFree(lpDisplayBuf);
        ExitProcess(dw);
    }

    LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        auto window = reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        return window->WndProc(hWnd, msg, wParam, lParam);
    }

    ATOM AppRegisterClass(HINSTANCE hInstance)
    {
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = 0; // LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = 0; // MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
        wcex.lpszClassName = szWindowClass;
        wcex.hIconSm = 0; // LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

        return RegisterClassEx(&wcex);
    }

    Window::Window(HINSTANCE hInstance) : mInstance(hInstance), mHandle(0)
    {
        auto &settings = GetSettings();
        AppRegisterClass(mInstance);

        mHandle = CreateWindowA(szWindowClass, settings.WindowSettings().Title.c_str(), WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, 0, settings.WindowSettings().Width, settings.WindowSettings().Height,
                                nullptr, nullptr, mInstance, nullptr);

        if (!mHandle)
        {
            ErrorExit("CreateWindow");
        }

        SetWindowLongPtr(mHandle, GWLP_USERDATA, LONG_PTR(this));

        // ShowWindow(mHandle, SW_SHOW);
        UpdateWindow(mHandle);
    }

    Window::~Window() { DestroyWindow(mHandle); }

    void Window::Show() { ShowWindow(mHandle, SW_SHOW); }

    LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_SIZE:
            GetEnvironment().gEventBus->Resize({LOWORD(lParam), HIWORD(lParam)});
            break;
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
        }
        return 0;
    }
} // namespace turd

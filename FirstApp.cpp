#include <iostream>
#include <Windows.h>
#include <windowsx.h>
#include <time.h>

int N = 3;
HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 255));

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {

	srand(time(0));

	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(NULL, IDI_QUESTION), L"MainWndClass", SoftwareMainProcedure);

	if (!RegisterClassW(&SoftwareMainClass)) { return -1; }
	MSG SoftwateMainMessage = { 0 };

	AllocConsole();
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD written;
	WriteConsole(hStdout, L"Введите желаемое количество клеток (при некорректном вводе будет использовано значение по умолчанию): ", 101, &written, NULL);
	char buffer[256];
	DWORD read;
	ReadConsole(hStdin, buffer, sizeof(buffer), &read, NULL);
	if (atoi(buffer) > 0) N = atoi(buffer);
	FreeConsole();

	Sleep(30);

	CreateWindow(L"MainWndClass", L"Fitst c++ window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 300, 300, 320, 240, NULL, NULL, NULL, NULL);

	while (GetMessage(&SoftwateMainMessage, NULL, NULL, NULL)) {
		TranslateMessage(&SoftwateMainMessage);
		DispatchMessage(&SoftwateMainMessage);
	}
	return 0;
}

WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure) {
	WNDCLASS NWC = { 0 };

	NWC.hIcon = Icon;
	NWC.hCursor = Cursor;
	NWC.hInstance = hInst;
	NWC.lpszClassName = Name;
	NWC.hbrBackground = BGColor;
	NWC.lpfnWndProc = Procedure;

	return NWC;
}

LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	static int currentColor = 0;
	static int currentColor2 = 255;
	static int deltaColor = 5;

	RECT rect;
	GetWindowRect(hWnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	static int cellSize = min(width / N, height / N);
	static int* cells;

	switch (msg)
	{
	case WM_CREATE:
		cells = new int[N * N];
		memset(cells, 0, N * N * sizeof(int));
		break;
	case WM_KEYDOWN:
		switch (wp)
		{
		case VK_RETURN:
		{
			DeleteObject(hBrush);
			hBrush = CreateSolidBrush(RGB(rand() % 256, rand() % 256, rand() % 256));
			SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		case VK_ESCAPE: PostQuitMessage(0);
		case 'Q':
			if (GetAsyncKeyState(VK_CONTROL) < 0) {
				PostQuitMessage(0);
			}
		case 'C':
			if (GetAsyncKeyState(VK_SHIFT) < 0) {
				HANDLE hProcess = NULL;
				HANDLE hThread = NULL;
				LPCWSTR notepadPath = L"C:\\Windows\\notepad.exe";
				STARTUPINFO si;
				PROCESS_INFORMATION pi;
				DWORD dwProcessId = 0;
				DWORD dwThreadId = 0;
				ZeroMemory(&si, sizeof(si));
				ZeroMemory(&pi, sizeof(pi));
				BOOL bCreateProcess = NULL;
				bCreateProcess = CreateProcess(notepadPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
		default:
			break;
		}
	case WM_MOUSEWHEEL:
	{
		int delta = GET_WHEEL_DELTA_WPARAM(wp);
		currentColor += deltaColor * (delta > 0 ? 1 : -1);
		currentColor2 += deltaColor * (delta > 0 ? -1 : 1);

		if (currentColor < 0)
			currentColor = 0;
		else if (currentColor > 255)
			currentColor = 255;
		if (currentColor2 < 0)
			currentColor2 = 0;
		else if (currentColor2 > 255)
			currentColor2 = 255;

		COLORREF newColor = RGB(currentColor2, currentColor, currentColor);
		SetDCPenColor(GetDC(hWnd), newColor);
		RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		break;
	}
	case WM_SIZE:
	{
		cellSize = min(width / N, height / N);

		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		FillRect(hdc, &ps.rcPaint, hBrush);

		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(currentColor2, currentColor, currentColor));
		SelectObject(hdc, hPen);
		for (int x = 0; x < ps.rcPaint.right; x += (ps.rcPaint.right / N))
		{
			MoveToEx(hdc, x, 0, NULL);
			LineTo(hdc, x, ps.rcPaint.bottom);
		}
		for (int y = 0; y < ps.rcPaint.bottom; y += (ps.rcPaint.bottom / N))
		{
			MoveToEx(hdc, 0, y, NULL);
			LineTo(hdc, ps.rcPaint.right, y);
		}

		for (int i = 0; i < N * N; ++i) {
			int x = (ps.rcPaint.right / N) * (i % N);
			int y = (ps.rcPaint.bottom / N) * (i / N);
			if (cells[i] == 1) {
				HBRUSH eBrush = CreateSolidBrush(RGB(255, 0, 0));
				HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, eBrush);
				Ellipse(hdc, x, y, x + (ps.rcPaint.right / N), y + (ps.rcPaint.bottom / N));
				SelectObject(hdc, hOldBrush);
				DeleteObject(eBrush);
			}
			else if (cells[i] == 2) {
				HPEN cPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
				SelectObject(hdc, cPen);
				MoveToEx(hdc, x, y, NULL);
				LineTo(hdc, x + (ps.rcPaint.right / N), y + (ps.rcPaint.bottom / N));
				MoveToEx(hdc, x + (ps.rcPaint.right / N), y, NULL);
				LineTo(hdc, x, y + (ps.rcPaint.bottom / N));
				DeleteObject(cPen);
			}
		}
		DeleteObject(hPen);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		int x = LOWORD(lp);
		int y = HIWORD(lp);
		int xCell = x / (width / N);
		int yCell = y / (height / N);
		if (cells[yCell * N + xCell] != 2) cells[yCell * N + xCell] = 1;
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		int x = LOWORD(lp);
		int y = HIWORD(lp);
		int xCell = x / (width / N);
		int yCell = y / (height / N);
		if (cells[yCell * N + xCell] != 1) cells[yCell * N + xCell] = 2;
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		break;
	}
	case WM_DESTROY:
		DeleteObject(hBrush);
		delete[] cells;
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
		break;
	}
}

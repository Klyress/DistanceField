// DXProjects.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "DXProjects.h"

#include "quaternion.h"
#include "vector4.h"
#include <amp.h>
#include <amp_graphics.h>
#include <amp_math.h>
using namespace xLib;
using namespace concurrency;
using namespace concurrency::graphics;
using namespace concurrency::graphics::direct3d;
using namespace concurrency::direct3d;
using namespace concurrency::fast_math;

#include "InitD3D.h"
#include "Camera.h"

//#include "Sea.h"
#include "Menger.h"
#include "RayMatchFunc.h"
#define MAX_LOADSTRING 100
#define PI 3.1415926535897932f

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

												// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

D3DContext context;
RTCamera<float>* g_Camera;

bool shadowOn = true;


float Cube4DDE(Vector3<float> point) restrict(amp)
{
	// hyper cude has 16 vertices
	Vector4<float> pt[16] = {
		Vector4<float>(1.0f, 1.0f, 1.0f, 0.0f),
		Vector4<float>(-1.0f, 1.0f, 1.0f, 0.0f),
		Vector4<float>(-1.0f, -1.0f, 1.0f, 0.0f),
		Vector4<float>(1.0f, -1.0f, 1.0f, 0.0f),
		Vector4<float>(1.0f, 1.0f, -1.0f, 0.0f),
		Vector4<float>(-1.0f, 1.0f, -1.0f, 0.0f),
		Vector4<float>(-1.0f, -1.0f, -1.0f, 0.0f),
		Vector4<float>(1.0f, -1.0f, -1.0f, 0.0f),
		Vector4<float>(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4<float>(-1.0f, 1.0f, 1.0f, 1.0f),
		Vector4<float>(-1.0f, -1.0f, 1.0f, 1.0f),
		Vector4<float>(1.0f, -1.0f, 1.0f, 1.0f),
		Vector4<float>(1.0f, 1.0f, -1.0f, 1.0f),
		Vector4<float>(-1.0f, 1.0f, -1.0f, 1.0f),
		Vector4<float>(-1.0f, -1.0f, -1.0f, 1.0f),
		Vector4<float>(1.0f, -1.0f, -1.0f, 1.0f)
	};

	// and 32 lines
	int line[32][2] = {
		{ 0, 1 },{ 1, 2 },{ 2, 3 },{ 3, 0 },
		{ 4, 5 },{ 5, 6 },{ 6, 7 },{ 7, 4 },
		{ 0, 4 },{ 1, 5 },{ 2, 6 },{ 3, 7 },

		{ 0 + 8, 1 + 8 },{ 1 + 8, 2 + 8 },{ 2 + 8, 3 + 8 },{ 3 + 8, 0 + 8 },
		{ 4 + 8, 5 + 8 },{ 5 + 8, 6 + 8 },{ 6 + 8, 7 + 8 },{ 7 + 8, 4 + 8 },
		{ 0 + 8, 4 + 8 },{ 1 + 8, 5 + 8 },{ 2 + 8, 6 + 8 },{ 3 + 8, 7 + 8 },

		{ 0, 8 },{ 1, 9 },{ 2, 10 },{ 3, 11 },
		{ 4, 12 },{ 5, 13 },{ 6, 14 },{ 7, 15 },
	};

	// inverse stereographic projection of point to S3
	float r = point.Length();
	Vector4<float> z4 = Vector4<float>(2.0f*point.x, 2.0f*point.y, 2.0f*point.z, r*r - 1.0f)* (1.0f / (r*r + 1.0f));

	Vector4<float> t;
	Vector4<float> tempt;
	// distance to point
	//float distancePt = D3D10_FLOAT32_MAX;
	//for (int i = 0; i < 16; i++)
	//{
	//	distancePt = min((pt[i] - z4).Length(), distancePt);
	//}
	//// points were displayed as sphere which radius is 0.3f
	//distancePt = distancePt - 0.2f;
	//if (distancePt < 0.0f) distancePt = 0.0f;

	// distance to edge
	//bool isVertexOneLoop = false;
	//bool isVertexFinal;
	//float distanceEdge = D3D10_FLOAT32_MAX;
	//for (int i = 0; i < 32; i++)
	//{
	//	Vector4<float> A = pt[line[i][0]];
	//	Vector4<float> B = pt[line[i][1]];

	//	Vector4<float> AP = (z4 - A);
	//	Vector4<float> lineDir = (B - A).Normalize();
	//	float APPerpendicular = AP * lineDir;

	//	float distance;
	//	if (APPerpendicular > 0.0f && APPerpendicular < (A - B).Length())
	//	{
	//		isVertexOneLoop = false;
	//		Vector4<float> tempt = A + lineDir * APPerpendicular;
	//		distance = (tempt - z4).Length();
	//	}
	//	else
	//	{
	//		isVertexOneLoop = true;
	//		float distanceA = (A - z4).Length();
	//		float distanceB = (B - z4).Length();
	//		if (distanceA < distanceB)
	//		{
	//			distance = distanceA;
	//			tempt = A;
	//		}
	//		else
	//		{
	//			distanceB = distanceB;
	//			tempt = B;
	//		}
	//		distance = min(distanceA, distanceB);
	//	}

	//	if (distance < distanceEdge)
	//	{
	//		isVertexFinal = isVertexOneLoop;
	//		distanceEdge = distance;
	//		t = tempt;
	//	}
	//}

	float distancePt = D3D10_FLOAT32_MAX;
	Vector4<float> v;
	for (int i = 0; i < 16; i++)
	{
		if ((pt[i] - z4).Length() < distancePt)
		{
			distancePt = (pt[i] - z4).Length();
			v = pt[i];
		}
	}
	// map t to 3-dimension space
	Vector3<float> k(v.x / (1.0f - v.w), v.y / (1.0f - v.w), v.z / (1.0f - v.w));
	float estimateDistance = (k - point).Length();
	//if (isVertexFinal)
	//{
	estimateDistance = estimateDistance - 0.2f;
	if (estimateDistance < 0.0f)
	{
		estimateDistance = 0.0f;
	}
	//}
	//else
	//{
	//	estimateDistance = estimateDistance - 0.1f;
	//}

	return estimateDistance;
}

float CubeDE(Vector3<float> point) restrict(amp)
{
	Vector3<float> pt[8] = {
		Vector3<float>(1.0f, 1.0f, 1.0f),
		Vector3<float>(-1.0f, 1.0f, 1.0f),
		Vector3<float>(-1.0f, -1.0f, 1.0f),
		Vector3<float>(1.0f, -1.0f, 1.0f),
		Vector3<float>(1.0f, 1.0f, -1.0f),
		Vector3<float>(-1.0f, 1.0f, -1.0f),
		Vector3<float>(-1.0f, -1.0f, -1.0f),
		Vector3<float>(1.0f, -1.0f, -1.0f)
	};
	int line[12][2] = {
		{ 0, 1 },{ 1, 2 },{ 2, 3 },{ 3, 0 },
		{ 4, 5 },{ 5, 6 },{ 6, 7 },{ 7, 4 },
		{ 0, 4 },{ 1, 5 },{ 2, 6 },{ 3, 7 }
	};

	// distance to point
	float distancePt = D3D10_FLOAT32_MAX;
	for (int i = 0; i < 8; i++)
	{
		distancePt = min((pt[i] - point).Length(), distancePt);
	}
	// points were displayed as sphere which radius is 0.3f
	distancePt = distancePt - 0.2f;
	if (distancePt < 0.0f) distancePt = 0.0f;

	// distance to edge
	float distanceEdge = D3D10_FLOAT32_MAX;
	for (int i = 0; i < 12; i++)
	{
		Vector3<float> A = pt[line[i][0]];
		Vector3<float> B = pt[line[i][1]];

		Vector3<float> AP = (point - A);
		Vector3<float> lineDir = (B - A).Normalize();
		float APPerpendicular = AP * lineDir;

		float distance;
		if (APPerpendicular > 0.0f && APPerpendicular < (A - B).Length())
		{
			distance = (point - (A + lineDir * APPerpendicular)).Length();
		}
		else
		{
			float distanceA = (A - point).Length();
			float distanceB = (B - point).Length();
			distance = min(distanceA, distanceB);
		}

		distanceEdge = min(distanceEdge, distance);
	}

	distanceEdge = distanceEdge - 0.1f;
	if (distanceEdge < 0.0f) distanceEdge = 0.0f;

	return min(distancePt, distanceEdge);
}




// for debug
float SphereDistanceEstimate(Vector3<float> point, Vector3<float>& normal) restrict(amp)
{
	// sphere for debug
	float r = 1.0f;
	float distance = point.Length();
	if (distance < r)
	{
		return 0;
	}
	else
	{
		if (distance - r < point.y + 1.0f)
		{
			normal = point;
			return distance - r;
		}
		else
		{
			normal = Vector3<float>(0.0f, 1.0f, 0.0f);
			return point.y + 1.0f;
		}
	}
}


//// for quaternion specialization
//template<>
//float potential<quaternion<float>>(quaternion<float> pos, quaternion<float>& c) restrict(amp)
//{
//	quaternion<float> z = pos;
//	quaternion<float> a(0.5f, 0.5f, 0.0f, 0.0f);
//	//quaternion<float> c(1, -1, 1, 0);
//	int iterations = 10;
//	for (int i = 1; i < iterations; i++)
//	{	
//		z = z * z + c;
//		if (z.Length() > 1.0f) 
//			return log(z.Length()) / pow(2.0f, float(i));
//	}
//	return 0.0f;
//}
//
//template<>
//float DistanceEstimate<quaternion<float>>(quaternion<float> point, quaternion<float> c, quaternion<float>& normal)  restrict(amp)
//{
//	float EPS = 0.001f;
//	float pot = potential(point, c);
//
//	// this point is convengence, inside of the shape, return distance 0
//	if (pot == 0.0f) return 0.0f;
//
//	quaternion<float> gradient;
//	gradient.x = (potential(point + quaternion<float>(1.0f, 0.0f, 0.0f, 0.0f) * EPS, c) - pot)/ EPS;
//	gradient.y = (potential(point + quaternion<float>(0.0f, 1.0f, 0.0f, 0.0f) * EPS, c) - pot)/ EPS;
//	gradient.z = (potential(point + quaternion<float>(0.0f, 0.0f, 1.0f, 0.0f) * EPS, c) - pot)/ EPS;
//	gradient.w = (potential(point + quaternion<float>(0.0f, 0.0f, 0.0f, 1.0f) * EPS, c) - pot)/ EPS;
//
//	normal = gradient;
//
//	return (0.5f / exp(pot))*sinh(pot) / gradient.Length();
//	//return 0.5 * pot / gradient.Length();
//
//	//int Iterations = 50;
//	//quaternion<float> z = point;
//	//quaternion<float> s(1, 0, 0, 0);
//	//quaternion<float> c(0, 0.1f, 0, 0);
//	//for (int i = 1; i < Iterations; i++)
//	//{
//	//	s = z * s * 2.0f;
//	//	z = z * z + c;
//	//	if (z.Length() > 10.0f) break;
//	//}
//
//	//float normZ = z.Length();
//	//return  0.5 * normZ * log(normZ) / s.Length();
//};



void Render()
{
	static int pingPong = 0;
	static int frame = 0;

	// map 4-d quaternion w to time, in order to draw it in 3-d space
	static float time = 0.0f;

	static accelerator_view dxView = create_accelerator_view(context.m_pd3dDevices[0]);
	static texture< unorm_4, 2> ampTexture = make_texture< unorm_4, 2>(dxView, context.m_pBackBuffer);
	texture_view< unorm_4, 2> texView(ampTexture);

	texture<unorm_4, 2> source = make_texture<unorm_4, 2>(dxView, context.m_pWorkBuffer[pingPong]);
	texture<unorm_4, 2> dest = make_texture<unorm_4, 2>(dxView, context.m_pWorkBuffer[1 - pingPong]);
	texture<float, 2> depthSource = make_texture<float, 2>(dxView, context.m_pDepthBuffer[pingPong]);
	texture<float, 2> depthDest = make_texture<float, 2>(dxView, context.m_pDepthBuffer[1 - pingPong]);
	texture_view<const unorm_4, 2> sourceView(source);
	texture_view<unorm_4, 2> destView(dest);
	texture_view<const float, 2> depthSourceView(depthSource);
	texture_view<float, 2> depthDestView(depthDest);

	Vector3<float> eye = g_Camera->m_eye;
	Vector3<float> leftTopPoint = g_Camera->m_leftTopPoint;
	Vector3<float> xStep = g_Camera->m_xStep;
	Vector3<float> yStep = g_Camera->m_yStep;
	int width = g_Camera->m_viewportWidth;
	int height = g_Camera->m_viewportHeight;

	float w = time;
	float resolutionFactor = g_Camera->GetResolutionFactor();
	g_Camera->m_objDistance = DE(g_Camera->m_eye);

	float threshold = g_Camera->m_objDistance * resolutionFactor;
	if (threshold < 0.000001f)
	{
		threshold = 0.000001f;
	}

	// let say, ambient is 0.3, 0.3, 0.3 and we have a direction light from 1, 1, 1
	bool isShadowOn = shadowOn;
	parallel_for_each(dest.extent,
		[=](index<2> idx) restrict(amp)
	{

		int x = idx[1];
		int y = idx[0];

		Vector3<float> strideH = xStep * x;
		Vector3<float> strideV = yStep * y;

		Ray<float> r;
		r.from = eye;
		r.to = leftTopPoint - strideH - strideV;
		
		Vector3<float> color;
		color = Matching(r, threshold, isShadowOn);

		destView.set(idx, unorm4(color.x, color.y, color.z, 1.0f));
		depthDestView.set(idx, color.w);
	});


#if DOWN_SAMPLE == 2
	texture_view<const unorm_4, 2> renderView(dest);
	parallel_for_each(dest.extent,
		[=](index<2> idx) restrict(amp)
	{
		int x = idx[0];
		int y = idx[1];
		unorm_4 pixel = renderView.get(idx);
		unorm_4 quarter(0.25f, 0.25f, 0.25f, 1.0f);
		pixel *= quarter;
		unorm_4 a = renderView.get(index<2>(x + 1, y));
		a *= quarter;
		pixel += a;
		a = renderView.get(index<2>(x, y + 1));
		a *= quarter;
		pixel += a;
		a = renderView.get(index<2>(x + 1, y + 1));
		a *= quarter;
		pixel += a;

		texView.set(index<2>(x / 2, y / 2), pixel);
	});
#else
	texture_view<const unorm_4, 2> renderView(dest);
	parallel_for_each(dest.extent,
		[=](index<2> idx) restrict(amp)
	{
		unorm_4 pixel = renderView.get(idx);
		texView.set(idx, pixel);
	});
#endif
	pingPong = 1 - pingPong;
	//	time += 0.005f;
	// we got depth buffer, can do some post effect here

	context.m_pSwapChain->Present(0, 0);
	frame++;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	MSG msg = { 0 };
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DXPROJECTS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DXPROJECTS));
	UINT frames = 0;
	UINT time = 0;
	UINT elapseTime = 0;
	UINT lastUpdateTime = 0;
	UINT fps = 0;
	UINT totalFrames = 0;
	WCHAR windowStr[64];
	// Main message loop:
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		totalFrames++;
		Render();
		time = GetTickCount();
		elapseTime = time - lastUpdateTime;
		frames++;
		if (elapseTime >= 1000)
		{

			lastUpdateTime = time;
			fps = frames * 1000 / elapseTime;
			frames = 0;
			swprintf(windowStr, 64, L"FPS = %d, Frames = %d, Resolution = %f", fps, totalFrames, g_Camera->GetResolutionFactor() * g_Camera->m_objDistance);
			SetWindowText(msg.hwnd, windowStr);
		}
	}


	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DXPROJECTS));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_DXPROJECTS);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 800, 800, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	context.InitD3D(hWnd);

	RECT rc;
	GetClientRect(hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;
	g_Camera = new RTCamera<float>(Vector3<float>(10.0f, 10.0f, 0.f), Vector3<float>(0.0f, 0.0f, 0.0f), Vector3<float>(0.0f, 1.0f, 0.0f), 3.14159265f / 360.0f * 60.0f, width*DOWN_SAMPLE, height*DOWN_SAMPLE);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	int xPos = 0;
	int yPos = 0;
	int keyCode;
	float cameraSpeed = 0.01f;
	static int ox;
	static int oy;

	switch (message)
	{
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_KEYDOWN:
		keyCode = wParam;

		switch (keyCode)
		{
			//case 'A':
			//	g_Camera->m_eye = g_Camera->m_lookAt + xLib::quaternion<float>(g_Camera->m_up, 0.01f).Rotate(g_Camera->m_eye - g_Camera->m_lookAt); g_Camera->UpdateCamera(); break;
			//case 'D':
			//	g_Camera->m_eye = g_Camera->m_lookAt + xLib::quaternion<float>(g_Camera->m_up, -0.01f).Rotate(g_Camera->m_eye - g_Camera->m_lookAt); g_Camera->UpdateCamera(); break;
			//case 'W': 
			//	g_Camera->m_eye = g_Camera->m_lookAt + xLib::quaternion<float>(g_Camera->m_left, -0.01f).Rotate(g_Camera->m_eye - g_Camera->m_lookAt); g_Camera->UpdateCamera(); break;
			//case 'S':
			//	g_Camera->m_eye = g_Camera->m_lookAt + xLib::quaternion<float>(g_Camera->m_left, 0.01f).Rotate(g_Camera->m_eye - g_Camera->m_lookAt); g_Camera->UpdateCamera(); break;
		case 'W':g_Camera->ZoomCamera(); break;
		case 'S':g_Camera->ZoomCamera(-1.0f); break;
		case 'Q':g_Camera->RollCamera(1.0f);break;
		case 'E':g_Camera->RollCamera(-1.0f); break;
		case 'A':g_Camera->StrafeCamera(); break;
		case 'D':g_Camera->StrafeCamera(-1.0f); break;
		}


	case WM_LBUTTONUP:
		break;

	case WM_MOUSEMOVE:
		xPos = GET_X_LPARAM(lParam);
		yPos = GET_Y_LPARAM(lParam);
		g_Camera->RotateCamera(400 - xPos, yPos - 400);
		POINT pt; pt.x = 400; pt.y = 400;
		ClientToScreen(hWnd, &pt);
		SetCursorPos(pt.x, pt.y);
		break;

	case WM_RBUTTONUP:
		//g_Camera->ZoomOut(10.0);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		context.CleanContext();
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

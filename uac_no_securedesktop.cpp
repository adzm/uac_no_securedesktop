#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>
#include <conio.h>

#include <windows.h>

namespace UAC
{
	struct __declspec(uuid("e1ba41ad-4a1d-418f-aaba-3d1196b423d3")) SDChangeObj;

	struct __declspec(uuid("802c03cf-0243-4daf-bde5-a1a9071b79d8"))
	ISDChangeObj : IDispatch
	{
		virtual HRESULT __stdcall UseSecureDesktop(long UseSD) = 0;
	};

	HRESULT TestSDChange()
	{
		HRESULT hr = S_OK;

		IUnknown* pUnk = nullptr;
		ISDChangeObj* pSDChange = nullptr;

		// We could use ::CoCreateInstance, but that requires this process to be elevated for it to work.
		// Using ::CoGetObject allows the local server / out of process COM object to request elevation
		// on our behalf, without our process requiring elevation.
		static const bool useComElevation = true;
		if (!useComElevation) {
			hr = ::CoCreateInstance(__uuidof(SDChangeObj), nullptr, CLSCTX_LOCAL_SERVER, __uuidof(IUnknown), (void**)&pUnk);
		}
		else {

			wchar_t clsid[50];
			wchar_t moniker[300];

			::StringFromGUID2(__uuidof(SDChangeObj), clsid, _countof(clsid));
			wsprintf(moniker, L"Elevation:Administrator!new:%s", clsid);

			BIND_OPTS3 bo;
			memset(&bo, 0, sizeof(bo));
			bo.cbStruct = sizeof(bo);
			bo.hwnd = ::GetConsoleWindow();
			bo.dwClassContext = CLSCTX_LOCAL_SERVER;
			hr = ::CoGetObject(moniker, &bo, __uuidof(IUnknown), (void**)&pUnk);
		}

		if (!SUCCEEDED(hr) || !pUnk) {
			wprintf(L"Could not create SDChangeObj coclass (HRESULT 0x%08x)\r\n", hr);
		} else {
			hr = ::OleRun(pUnk);
			hr = pUnk->QueryInterface(__uuidof(ISDChangeObj), (void**)&pSDChange);

			pUnk->Release();

			if (!SUCCEEDED(hr) || !pSDChange) {
				wprintf(L"Could not QI for ISDChangeObj interface (HRESULT 0x%08x)\r\n", hr);
			} else {
				hr = pSDChange->UseSecureDesktop(0);	// no more UAC prompts on secure deskop
				if (SUCCEEDED(hr)) {
					wprintf(L"Secure Desktop disabled temporarily, go try a UAC prompt! Press any key to exit...\r\n");
					_getch();
					hr = pSDChange->UseSecureDesktop(1);	// back to normal
					// note this does not always seem to revert immediately, sometimes there is a delay
				}
				else {
					wprintf(L"UseSecureDesktop failed with (HRESULT 0x%08x)\r\n", hr);
				}

				pSDChange->Release();
			}
		}

		return hr;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	HRESULT hr = S_OK;
	
	hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	hr = UAC::TestSDChange();

	::CoUninitialize();
	
	return hr;
}


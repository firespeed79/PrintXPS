// PrintXPS - Command line XPS file printer.
// Copyright (C) 2010 Frogmore Computer Services Ltd
// www.frogmorecs.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "printxps.h"
#include "cominit.h"
#include "smarthandle.h"

#include <iostream>
#include <XpsPrint.h>
#include <xpsobjectmodel.h>

#include <atlbase.h>

int wmain(int argc, wchar_t** argv)
{
	try
	{
		if (argc < 3)
		{
			std::wcout << L"PrintXPS 1.1\n(C) 2010 Frogmore Computer Services Ltd\n\n";
			std::wcout << L"Command line usage:\n";
			std::wcout << L"printxps printername filename.xps\n";
			return 1;
		}

		ComInit ci;

		SmartHandle event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if (event.get() == 0)
		{
			::AtlThrow(::GetLastError());
		}

		CComPtr<IXpsOMObjectFactory> pXpsFactory;
		HRESULT hr = pXpsFactory.CoCreateInstance(CLSID_XpsOMObjectFactory);
		if (FAILED(hr))
		{
			::AtlThrow(hr);
		}

		CComPtr<IXpsPrintJobStream> pDocumentStream;
		hr = ::StartXpsPrintJob(argv[1], argv[2], NULL, NULL, event.get(), NULL, NULL, NULL, &pDocumentStream, NULL);
		if (FAILED(hr))
		{
			::AtlThrow(hr);
		}

		CComPtr<IXpsOMPackage> pXpsOMPackage;

		hr = pXpsFactory->CreatePackageFromFile(argv[2], false, &pXpsOMPackage);
		if (FAILED(hr))
		{
			::AtlThrow(hr);
		}

		hr = pXpsOMPackage->WriteToStream(pDocumentStream, FALSE);
		if (FAILED(hr))
		{
			::AtlThrow(hr);
		}

		pDocumentStream->Close();

		if (::WaitForSingleObject(event.get(), INFINITE) != WAIT_OBJECT_0)
		{
			::AtlThrow(hr);
		}


		return 0;
	}
	catch(CAtlException e)
	{
		std::wcerr << L"Error " << e.m_hr << std::endl;

		return e.m_hr;
	}
	catch (...)
	{
		std::wcerr << L"Unknown error" << std::endl;

		return 1;
	}
}
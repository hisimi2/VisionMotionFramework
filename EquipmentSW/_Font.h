#pragma once

// 머신에서 Microsoft Visual C++를 사용하여 생성한 IDispatch 래퍼 클래스입니다.

// 참고: 이 파일의 콘텐츠를 수정하지 마세요. Microsoft Visual C++를 통해 이 클래스가 다시 생성될 경우 
// 수정 내용을 덮어씁니다.

class COleFont : public COleDispatchDriver
{
public:
	COleFont() {}
	COleFont(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	COleFont(const COleFont& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

	CString GetName()
	{
		CString result;
		GetProperty(0x0, VT_BSTR, (void*)&result);
		return result;
	}

	void SetName(LPCTSTR propVal)
	{
		SetProperty(0x0, VT_BSTR, propVal);
	}

	CY GetSize()
	{
		CY result;
		GetProperty(0x2, VT_CY, (void*)&result);
		return result;
	}

	void SetSize(const CY& propVal)
	{
		SetProperty(0x2, VT_CY, &propVal);
	}

	BOOL GetBold()
	{
		BOOL result;
		GetProperty(0x3, VT_BOOL, (void*)&result);
		return result;
	}

	void SetBold(BOOL propVal)
	{
		SetProperty(0x3, VT_BOOL, propVal);
	}

	BOOL GetItalic()
	{
		BOOL result;
		GetProperty(0x4, VT_BOOL, (void*)&result);
		return result;
	}

	void SetItalic(BOOL propVal)
	{
		SetProperty(0x4, VT_BOOL, propVal);
	}

	BOOL GetUnderline()
	{
		BOOL result;
		GetProperty(0x5, VT_BOOL, (void*)&result);
		return result;
	}

	void SetUnderline(BOOL propVal)
	{
		SetProperty(0x5, VT_BOOL, propVal);
	}

	BOOL GetStrikethrough()
	{
		BOOL result;
		GetProperty(0x6, VT_BOOL, (void*)&result);
		return result;
	}

	void SetStrikethrough(BOOL propVal)
	{
		SetProperty(0x6, VT_BOOL, propVal);
	}

	short GetWeight()
	{
		short result;
		GetProperty(0x7, VT_I2, (void*)&result);
		return result;
	}

	void SetWeight(short propVal)
	{
		SetProperty(0x7, VT_I2, propVal);
	}

	short GetCharset()
	{
		short result;
		GetProperty(0x8, VT_I2, (void*)&result);
		return result;
	}

	void SetCharset(short propVal)
	{
		SetProperty(0x8, VT_I2, propVal);
	}
};

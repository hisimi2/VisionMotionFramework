#pragma once
#include "_Picture.h"
#include "_Font.h"

// 컴퓨터에서 Microsoft Visual C++를 사용하여 생성한 IDispatch 래퍼 클래스입니다.

// 참고: 이 파일의 내용을 수정하지 마십시오. Microsoft Visual C++에서
//  이 클래스를 다시 생성할 때 수정한 내용을 덮어씁니다.

/////////////////////////////////////////////////////////////////////////////
// CSpreadsheet_legend 래퍼 클래스입니다.

class CSpreadsheet_legend : public CWnd
{
protected:
	DECLARE_DYNCREATE(CSpreadsheet_legend)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x41F841C1, 0xAE16, 0x11D5, { 0x88, 0x17, 0x0, 0x50, 0xDA, 0x6E, 0xF5, 0xE5 } };
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
						const RECT& rect, CWnd* pParentWnd, UINT nID, 
						CCreateContext* pContext = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID); 
	}

    BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID, CFile* pPersist = NULL, BOOL bStorage = FALSE,
				BSTR bstrLicKey = NULL)
	{ 
		return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); 
	}

// 특성입니다.
public:
enum
{
    SpreadHeader = -1000
}CoordConstants;
enum
{
    BorderStyleNone = 0,
    BorderStyleFixedSingle = 1
}BorderStyleConstants;
enum
{
    DispBlank = 0,
    DispNumbers = 1,
    DispLetters = 2
}HeaderDisplayConstants;
enum
{
    EditEnterActionNone = 0,
    EditEnterActionUp = 1,
    EditEnterActionDown = 2,
    EditEnterActionLeft = 3,
    EditEnterActionRight = 4,
    EditEnterActionNext = 5,
    EditEnterActionPrevious = 6,
    EditEnterActionSame = 7,
    EditEnterActionNextRow = 8
}EditEnterActionConstants;
enum
{
    OperationModeNormal = 0,
    OperationModeRead = 1,
    OperationModeRow = 2,
    OperationModeSingle = 3,
    OperationModeMulti = 4,
    OperationModeExtended = 5
}OperationModeConstants;
enum
{
    PositionUpperLeft = 0,
    PositionUpperCenter = 1,
    PositionUpperRight = 2,
    PositionCenterLeft = 3,
    PositionCenter = 4,
    PositionCenterRight = 5,
    PositionBottomLeft = 6,
    PositionBottomCenter = 7,
    PositionBottomRight = 8
}PositionConstants;
enum
{
    ScrollBarsNone = 0,
    ScrollBarsHorizontal = 1,
    ScrollBarsVertical = 2,
    ScrollBarsBoth = 3
}ScrollBarsConstants;
enum
{
    UnitTypeNormal = 0,
    UnitTypeVGABase = 1,
    UnitTypeTwips = 2
}UnitTypeConstants;
enum
{
    BackColorStyleOverGrid = 0,
    BackColorStyleUnderGrid = 1,
    BackColorStyleOverHorzGridOnly = 2,
    BackColorStyleOverVertGridOnly = 3
}BackColorStyleConstants;
enum
{
    DAutoSizeColsNone = 0,
    DAutoSizeColsMax = 1,
    DAutoSizeColsBest = 2
}DAutoSizeColsConstants;
enum
{
    ActionActiveCell = 0,
    ActionGotoCell = 1,
    ActionSelectBlock = 2,
    ActionClear = 3,
    ActionDeleteCol = 4,
    ActionDeleteRow = 5,
    ActionInsertCol = 6,
    ActionInsertRow = 7,
    ActionReCalc = 11,
    ActionClearText = 12,
    ActionPrint = 13,
    ActionDeselectBlock = 14,
    ActionDSave = 15,
    ActionSetCellBorder = 16,
    ActionAddMultiSelBlock = 17,
    ActionGetMultiSelection = 18,
    ActionCopyRange = 19,
    ActionMoveRange = 20,
    ActionSwapRange = 21,
    ActionClipboardCopy = 22,
    ActionClipboardCut = 23,
    ActionClipboardPaste = 24,
    ActionSort = 25,
    ActionComboClear = 26,
    ActionComboRemove = 27,
    ActionReset = 28,
    ActionSelModeClear = 29,
    ActionVModeRefresh = 30,
    ActionSmartPrint = 32
}ActionConstants;
enum
{
    CellBorderStyleDefault = 0,
    CellBorderStyleSolid = 1,
    CellBorderStyleDash = 2,
    CellBorderStyleDot = 3,
    CellBorderStyleDashDot = 4,
    CellBorderStyleDashDotDot = 5,
    CellBorderStyleBlank = 6,
    CellBorderStyleFineSolid = 11,
    CellBorderStyleFineDash = 12,
    CellBorderStyleFineDot = 13,
    CellBorderStyleFineDashDot = 14,
    CellBorderStyleFineDashDotDot = 15
}CellBorderStyleConstants;
enum
{
    CellTypeDate = 0,
    CellTypeEdit = 1,
    CellTypeFloat = 2,
    CellTypeInteger = 3,
    CellTypePic = 4,
    CellTypeStaticText = 5,
    CellTypeTime = 6,
    CellTypeButton = 7,
    CellTypeComboBox = 8,
    CellTypePicture = 9,
    CellTypeCheckBox = 10,
    CellTypeOwnerDrawn = 11,
    CellTypeCurrency = 12,
    CellTypeNumber = 13,
    CellTypePercent = 14
}CellTypeConstants;
enum
{
    CursorStyleUserDefined = 0,
    CursorStyleDefault = 1,
    CursorStyleArrow = 2,
    CursorStyleDefColResize = 3,
    CursorStyleDefRowResize = 4
}CursorStyleConstants;
enum
{
    CursorTypeDefault = 0,
    CursorTypeColResize = 1,
    CursorTypeRowResize = 2,
    CursorTypeButton = 3,
    CursorTypeGrayArea = 4,
    CursorTypeLockedCell = 5,
    CursorTypeColHeader = 6,
    CursorTypeRowHeader = 7,
    CursorTypeDragDropArea = 8,
    CursorTypeDragDrop = 9
}CursorTypeConstants;
enum
{
    PrintOrientationDefault = 0,
    PrintOrientationPortrait = 1,
    PrintOrientationLandscape = 2
}PrintOrientationConstants;
enum
{
    PrintTypeAll = 0,
    PrintTypeCellRange = 1,
    PrintTypeCurrentPage = 2,
    PrintTypePageRange = 3
}PrintTypeConstants;
enum
{
    SortByRow = 0,
    SortByCol = 1
}SortByConstants;
enum
{
    SortKeyOrderNone = 0,
    SortKeyOrderAscending = 1,
    SortKeyOrderDescending = 2
}SortKeyOrderConstants;
enum
{
    TypeButtonAlignBottom = 0,
    TypeButtonAlignTop = 1,
    TypeButtonAlignLeft = 2,
    TypeButtonAlignRight = 3
}TypeButtonAlignConstants;
enum
{
    TypeButtonTypeNormal = 0,
    TypeButtonTypeTwoState = 1
}TypeButtonTypeConstants;
enum
{
    TypeCheckTextAlignLeft = 0,
    TypeCheckTextAlignRight = 1
}TypeCheckTextAlignConstants;
enum
{
    TypeCheckTypeNormal = 0,
    TypeCheckTypeThreeState = 1
}TypeCheckTypeConstants;
enum
{
    TypeDateFormatDDMONYY = 0,
    TypeDateFormatDDMMYY = 1,
    TypeDateFormatMMDDYY = 2,
    TypeDateFormatYYMMDD = 3,
    TypeDateFormatDefault = 99
}TypeDateFormatConstants;
enum
{
    TypeEditCharCaseSetLower = 0,
    TypeEditCharCaseSetNone = 1,
    TypeEditCharCaseSetUpper = 2
}TypeEditCharCaseConstants;
enum
{
    TypeEditCharSetASCII = 0,
    TypeEditCharSetAlpha = 1,
    TypeEditCharSetAlphanumeric = 2,
    TypeEditCharSetNumeric = 3
}TypeEditCharSetConstants;
enum
{
    TypeHAlignLeft = 0,
    TypeHAlignRight = 1,
    TypeHAlignCenter = 2
}TypeHAlignConstants;
enum
{
    TypeTextAlignVertBottom = 0,
    TypeTextAlignVertCenter = 1,
    TypeTextAlignVertTop = 2
}TypeTextAlignVertConstants;
enum
{
    TypeTime24Hour12HourClock = 0,
    TypeTime24Hour24HourClock = 1,
    TypeTime24HourDefault = 2
}TypeTime24HourConstants;
enum
{
    UserResizeNone = 0,
    UserResizeColumns = 1,
    UserResizeRows = 2,
    UserResizeBoth = 3
}UserResizeConstants;
enum
{
    UserResizeDefault = 0,
    UserResizeOn = 1,
    UserResizeOff = 2
}UserResizeConstants2;
enum
{
    AppearanceFlat = 0,
    Appearance3D = 1,
    Appearance3DWithBorder = 2
}AppearanceConstants;
enum
{
    TextTipOff = 0,
    TextTipFixed = 1,
    TextTipFloating = 2,
    TextTipFixedFocusOnly = 3,
    TextTipFloatingFocusOnly = 4
}TextTipConstants;
enum
{
    ScrollBarTrackOff = 0,
    ScrollBarTrackVertical = 1,
    ScrollBarTrackHorizontal = 2,
    ScrollBarTrackBoth = 3
}ScrollBarTrackConstants;
enum
{
    Default = 0,
    Arrow = 1,
    Cross = 2,
    IBeam = 3,
    Icon = 4,
    Sizing = 5,
    SizeNESW = 6,
    SizeNS = 7,
    SizeNWSE = 8,
    SizeWE = 9,
    UpArrow = 10,
    Hourglass = 11,
    NoDrop = 12,
    ArrowHourglass = 13,
    ArrowQuestion = 14,
    SizeAll = 15,
    Custom = 99
}MousePointerConstants;
enum
{
    PageOrderAuto = 0,
    PageOrderDownThenOver = 1,
    PageOrderOverThenDown = 2
}PrintPageOrderConstants;
enum
{
    TypeVAlignTop = 0,
    TypeVAlignBottom = 1,
    TypeVAlignCenter = 2
}TypeVAlignConstants;
enum
{
    ScrollBarShow = 0,
    ScrollBarAuto = 1,
    ScrollBarHide = 2
}PVScrollBarConstants;
enum
{
    GrayAreaMarginTypeScaled = 0,
    GrayAreaMarginTypeActual = 1
}PVGrayAreaMarginTypeConstants;
enum
{
    PageViewTypeWholePage = 0,
    PageViewTypeNormalSize = 1,
    PageViewTypePercentage = 2,
    PageViewTypePageWidth = 3,
    PageViewTypePageHeight = 4,
    PageViewTypeMultiplePages = 5
}PVPageViewTypeConstants;
enum
{
    ZoomStateIndeterminate = 0,
    ZoomStateIn = 1,
    ZoomStateOut = 2,
    ZoomStateSwitch = 3
}PVZoomStateConstants;
enum
{
    ColUserSortIndicatorNone = 0,
    ColUserSortIndicatorAscending = 1,
    ColUserSortIndicatorDescending = 2,
    ColUserSortIndicatorDisabled = 3
}ColUserSortIndicatorConstants;
enum
{
    UserColActionDefault = 0,
    UserColActionSort = 1,
    UserColActionSortNoIndicator = 2
}UserColActionConstants;
enum
{
    ShowScrollTipsOff = 0,
    ShowScrollTipsVertical = 1,
    ShowScrollTipsHorizontal = 2,
    ShowScrollTipsBoth = 3
}ShowScrollTipsConstants;
enum
{
    CellNoteIndicatorShowAndFireEvent = 0,
    CellNoteIndicatorShowAndDoNotFireEvent = 1,
    CellNoteIndicatorDoNotShowAndFireEvent = 2,
    CellNoteIndicatorDoNotShowAndDoNotFireEvent = 3
}CellNoteIndicatorConstants;
enum
{
    ExportRangeToTextFileCreateNewFile = 1,
    ExportRangeToTextFileAppendToExistingFile = 2,
    ExportRangeToTextFileUnformattedData = 4,
    ExportRangeToTextFileColHeaders = 8,
    ExportRangeToTextFileRowHeaders = 16,
    ExportRangeToTextFileAllHeaders = 24
}ExportRangeToTextFileConstants;
enum
{
    ExportToTextFileCreateNewFile = 1,
    ExportToTextFileAppendToExistingFile = 2,
    ExportToTextFileUnformattedData = 4,
    ExportToTextFileColHeaders = 8,
    ExportToTextFileRowHeaders = 16
}ExportToTextFileConstants;
enum
{
    LoadTextFileNoHeaders = 0,
    LoadTextFileColHeaders = 1,
    LoadTextFileRowHeaders = 2,
    LoadTextFileClearDataOnly = 4
}LoadTextFileConstants;
enum
{
    ExportToXMLFormattedData = 0,
    ExportToXMLUnFormattedData = 1
}ExportToXMLConstants;
enum
{
    PrintFlagsNone = 0,
    PrintFlagsShowCommonDialog = 1
}PrintFlagsConstants;
enum
{
    MergeNone = 0,
    MergeAlways = 1,
    MergeRestricted = 2
}MergeConstants;
enum
{
    TypeComboBoxAutoSearchNone = 0,
    TypeComboBoxAutoSearchSingleChar = 1,
    TypeComboBoxAutoSearchMultipleChar = 2,
    TypeComboBoxAutoSearchSingleCharGreater = 3
}TypeComboAutoSearchConstants;
enum
{
    SearchFlagsNone = 0,
    SearchFlagsGreaterOrEqual = 1,
    SearchFlagsPartialMatch = 2,
    SearchFlagsValue = 4,
    SearchFlagsCaseSensitive = 8,
    SearchFlagsSortedAscending = 16,
    SearchFlagsSortedDescending = 32
}SearchFlagsConstants;
enum
{
    TypeLeadingZeroIntl = 0,
    TypeLeadingZeroNo = 1,
    TypeLeadingZeroYes = 2
}TypeLeadingZeroConstants;
enum
{
    TypeCurrencyNegStyleIntl = 0,
    TypeCurrencyNegStyle1 = 1,
    TypeCurrencyNegStyle2 = 2,
    TypeCurrencyNegStyle3 = 3,
    TypeCurrencyNegStyle4 = 4,
    TypeCurrencyNegStyle5 = 5,
    TypeCurrencyNegStyle6 = 6,
    TypeCurrencyNegStyle7 = 7,
    TypeCurrencyNegStyle8 = 8,
    TypeCurrencyNegStyle9 = 9,
    TypeCurrencyNegStyle10 = 10,
    TypeCurrencyNegStyle11 = 11,
    TypeCurrencyNegStyle12 = 12,
    TypeCurrencyNegStyle13 = 13,
    TypeCurrencyNegStyle14 = 14,
    TypeCurrencyNegStyle15 = 15,
    TypeCurrencyNegStyle16 = 16
}TypeCurrencyNegStyleConstants;
enum
{
    TypeCurrencyPosStyleIntl = 0,
    TypeCurrencyPosStyle1 = 1,
    TypeCurrencyPosStyle2 = 2,
    TypeCurrencyPosStyle3 = 3,
    TypeCurrencyPosStyle4 = 4
}TypeCurrencyPosStyleConstants;
enum
{
    TypeNumberNegStyleIntl = 0,
    TypeNumberNegStyle1 = 1,
    TypeNumberNegStyle2 = 2,
    TypeNumberNegStyle3 = 3,
    TypeNumberNegStyle4 = 4,
    TypeNumberNegStyle5 = 5
}TypeNumberNegStyleConstants;
enum
{
    TypePercentNegStyleIntl = 0,
    TypePercentNegStyle1 = 1,
    TypePercentNegStyle2 = 2,
    TypePercentNegStyle3 = 3,
    TypePercentNegStyle4 = 4,
    TypePercentNegStyle5 = 5,
    TypePercentNegStyle6 = 6,
    TypePercentNegStyle7 = 7,
    TypePercentNegStyle8 = 8
}TypePercentNegStyleConstants;
enum
{
    TypeTextOrientHorizontal = 0,
    TypeTextOrientVerticalLTR = 1,
    TypeTextOrientDown = 2,
    TypeTextOrientUp = 3,
    TypeTextOrientInvert = 4,
    TypeTextOrientVerticalRTL = 5
}TypeTextOrientConstants;
enum
{
    GetCellSpanNo = 0,
    GetCellSpanYes = 1,
    GetCellSpanAnchor = 2
}GetCellSpanConstants;
enum
{
    EventAllEvents = 0,
    EventAdvance = 1,
    EventAfterUserSort = 2,
    EventBeforeUserSort = 3,
    EventBlockSelected = 4,
    EventButtonClicked = 5,
    EventChange = 6,
    EventClick = 7,
    EventColWidthChange = 8,
    EventComboCloseUp = 9,
    EventComboDropDown = 10,
    EventComboSelChange = 11,
    EventCustomFunction = 12,
    EventDataColConfig = 13,
    EventDataFill = 14,
    EventDblClick = 15,
    EventDragDropBlock = 16,
    EventDrawItem = 17,
    EventEditChange = 18,
    EventEditError = 19,
    EventEditMode = 20,
    EventEnterRow = 21,
    EventKeyDown = 22,
    EventKeyPress = 23,
    EventKeyUp = 24,
    EventLeaveCell = 25,
    EventLeaveRow = 26,
    EventOLECompleteDrag = 27,
    EventOLEDragDrop = 28,
    EventOLEDragOver = 29,
    EventOLEGiveFeedback = 30,
    EventOLESetData = 31,
    EventOLEStartDrag = 32,
    EventPrintAbort = 33,
    EventPrintMsgBox = 34,
    EventQueryAdvance = 35,
    EventQueryData = 36,
    EventRightClick = 37,
    EventRowHeightChange = 38,
    EventSelChange = 39,
    EventTextTipFetch = 40,
    EventTopLeftChange = 41,
    EventUserFormulaEntered = 42,
    EventVirtualClearData = 43
}EVENTENABLEDConstants;
enum
{
    fpOLEDropNone = 0,
    fpOLEDropManual = 1
}OLEDropModeConstants;
enum
{
    fpEnter = 0,
    fpLeave = 1,
    fpOver = 2
}OLEDragOverConstants;
enum
{
    fpCFText = 1,
    fpCFBitmap = 2,
    fpCFMetafile = 3,
    fpCFDIB = 8,
    fpCFPalette = 9,
    fpCFEMetafile = 14,
    fpCFFiles = 15,
    fpCFRTF = -16639
}ClipFormatConstants;
enum
{
    fpOLEDropEffectNone = 0,
    fpOLEDropEffectCopy = 1,
    fpOLEDropEffectMove = 2,
    fpOLEDropEffectScroll = static_cast<int>(0x80000000)
}OLEDropModeEffectConstants;


// 작업입니다.
public:

// _DSpreadSheet

// Functions
//

	double get_ColWidth(long lCol)
	{
		double result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xd9, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, parms, lCol);
		return result;
	}
	void put_ColWidth(long lCol, double newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_R8 ;
		InvokeHelper(0xd9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, lCol, newValue);
	}
	double get_MaxTextColWidth(long lCol)
	{
		double result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xda, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, parms, lCol);
		return result;
	}
	void put_MaxTextColWidth(long lCol, double newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_R8 ;
		InvokeHelper(0xda, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, lCol, newValue);
	}
	double get_MaxTextRowHeight(long lRow)
	{
		double result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xdb, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, parms, lRow);
		return result;
	}
	void put_MaxTextRowHeight(long lRow, double newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_R8 ;
		InvokeHelper(0xdb, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, lRow, newValue);
	}
	double get_RowHeight(long lRow)
	{
		double result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xdc, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, parms, lRow);
		return result;
	}
	void put_RowHeight(long lRow, double newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_R8 ;
		InvokeHelper(0xdc, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, lRow, newValue);
	}
	long get_SortKey(short nIndex)
	{
		long result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xdd, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, nIndex);
		return result;
	}
	void put_SortKey(short nIndex, long newValue)
	{
		static BYTE parms[] = VTS_I2 VTS_I4 ;
		InvokeHelper(0xdd, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, nIndex, newValue);
	}
	long get_SortKeyOrder(short nIndex)
	{
		long result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xde, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, nIndex);
		return result;
	}
	void put_SortKeyOrder(short nIndex, long newValue)
	{
		static BYTE parms[] = VTS_I2 VTS_I4 ;
		InvokeHelper(0xde, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, nIndex, newValue);
	}
	LPDISPATCH get_TypeCheckPicture(short Index)
	{
		LPDISPATCH result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xdf, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, parms, Index);
		return result;
	}
	void put_TypeCheckPicture(short Index, LPDISPATCH newValue)
	{
		static BYTE parms[] = VTS_I2 VTS_DISPATCH ;
		InvokeHelper(0xdf, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Index, newValue);
	}
	BOOL AddCustomFunction(LPCTSTR FunctionName, short ParameterCnt)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_I2 ;
		InvokeHelper(0xe0, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FunctionName, ParameterCnt);
		return result;
	}
	BOOL AddCustomFunctionExt(LPCTSTR FunctionName, short MinParamCnt, short MaxParamCnt, long Flags)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_I2 VTS_I2 VTS_I4 ;
		InvokeHelper(0xe1, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FunctionName, MinParamCnt, MaxParamCnt, Flags);
		return result;
	}
	void CFGetCellParam(short Param, long * Col, long * Row)
	{
		static BYTE parms[] = VTS_I2 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0xe2, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Param, Col, Row);
	}
	double CFGetDoubleParam(short Param)
	{
		double result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xe3, DISPATCH_METHOD, VT_R8, (void*)&result, parms, Param);
		return result;
	}
	double CFGetDoubleParamExt(short Param, double * ParamValue)
	{
		double result;
		static BYTE parms[] = VTS_I2 VTS_PR8 ;
		InvokeHelper(0xe4, DISPATCH_METHOD, VT_R8, (void*)&result, parms, Param, ParamValue);
		return result;
	}
	long CFGetLongParam(short Param)
	{
		long result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xe5, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Param);
		return result;
	}
	BOOL CFGetParamInfo(short Param, short * Type, short * Status)
	{
		BOOL result;
		static BYTE parms[] = VTS_I2 VTS_PI2 VTS_PI2 ;
		InvokeHelper(0xe6, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Param, Type, Status);
		return result;
	}
	void CFGetRangeParam(short Param, long * Col, long * Row, long * Col2, long * Row2)
	{
		static BYTE parms[] = VTS_I2 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0xe7, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Param, Col, Row, Col2, Row2);
	}
	CString CFGetStringParam(short Param)
	{
		CString result;
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0xe8, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, Param);
		return result;
	}
	void CFSetResult(VARIANT Var)
	{
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0xe9, DISPATCH_METHOD, VT_EMPTY, NULL, parms, &Var);
	}
	CString ColNumberToLetter(long HeaderNumber)
	{
		CString result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xea, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, HeaderNumber);
		return result;
	}
	void ColWidthToTwips(float Width, long * Twips)
	{
		static BYTE parms[] = VTS_R4 VTS_PI4 ;
		InvokeHelper(0xeb, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Width, Twips);
	}
	void GetBottomRightCell(long * Col, long * Row)
	{
		static BYTE parms[] = VTS_PI4 VTS_PI4 ;
		InvokeHelper(0xec, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Col, Row);
	}
	BOOL GetCellDirtyFlag(long Col, long Row)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0xed, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row);
		return result;
	}
	void GetCellFromScreenCoord(long * Col, long * Row, long x, long y)
	{
		static BYTE parms[] = VTS_PI4 VTS_PI4 VTS_I4 VTS_I4 ;
		InvokeHelper(0xee, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Col, Row, x, y);
	}
	BOOL GetCellPos(long Col, long Row, long * x, long * y, long * Width, long * Height)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0xef, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, x, y, Width, Height);
		return result;
	}
	void GetClientArea(long * Width, long * Height)
	{
		static BYTE parms[] = VTS_PI4 VTS_PI4 ;
		InvokeHelper(0xf0, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Width, Height);
	}
	long GetColItemData(long Col)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xf1, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Col);
		return result;
	}
	CString QueryCustomName(LPCTSTR Name)
	{
		CString result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xf2, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, Name);
		return result;
	}
	CString GetCustomName(LPCTSTR Name)
	{
		CString result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xf3, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, Name);
		return result;
	}
	BOOL GetDataFillData(VARIANT * Var, short VarType)
	{
		BOOL result;
		static BYTE parms[] = VTS_PVARIANT VTS_I2 ;
		InvokeHelper(0xf4, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Var, VarType);
		return result;
	}
	void GetFirstValidCell(long * Col, long * Row)
	{
		static BYTE parms[] = VTS_PI4 VTS_PI4 ;
		InvokeHelper(0xf5, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Col, Row);
	}
	long GetItemData()
	{
		long result;
		InvokeHelper(0xf6, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
		return result;
	}
	BOOL GetIteration(short * MaxIterations, double * MaxChange)
	{
		BOOL result;
		static BYTE parms[] = VTS_PI2 VTS_PR8 ;
		InvokeHelper(0xf7, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, MaxIterations, MaxChange);
		return result;
	}
	void GetLastValidCell(long * Col, long * Row)
	{
		static BYTE parms[] = VTS_PI4 VTS_PI4 ;
		InvokeHelper(0xf8, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Col, Row);
	}
	long GetMultiSelItem(long SelPrev)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xf9, DISPATCH_METHOD, VT_I4, (void*)&result, parms, SelPrev);
		return result;
	}
	short GetRefStyle()
	{
		short result;
		InvokeHelper(0xfa, DISPATCH_METHOD, VT_I2, (void*)&result, NULL);
		return result;
	}
	long GetRowItemData(long Row)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0xfb, DISPATCH_METHOD, VT_I4, (void*)&result, parms, Row);
		return result;
	}
	BOOL GetText(long Col, long Row, VARIANT * Var)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PVARIANT ;
		InvokeHelper(0xfc, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Var);
		return result;
	}
	BOOL IsCellSelected(long Col, long Row)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0xfd, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row);
		return result;
	}
	BOOL IsFormulaValid(LPCTSTR Formula)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0xfe, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Formula);
		return result;
	}
	BOOL IsVisible(long Col, long Row, BOOL Partial)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_BOOL ;
		InvokeHelper(0xff, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Partial);
		return result;
	}
	BOOL LoadFromFile(LPCTSTR FileName)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x100, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName);
		return result;
	}
	BOOL LoadTabFile(LPCTSTR FileName)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x101, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName);
		return result;
	}
	void RowHeightToTwips(long Row, float Height, long * Twips)
	{
		static BYTE parms[] = VTS_I4 VTS_R4 VTS_PI4 ;
		InvokeHelper(0x102, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Row, Height, Twips);
	}
	BOOL SaveTabFile(LPCTSTR FileName)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x103, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName);
		return result;
	}
	BOOL SaveToFile(LPCTSTR FileName, BOOL DataOnly)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BOOL ;
		InvokeHelper(0x104, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName, DataOnly);
		return result;
	}
	BOOL SetCellDirtyFlag(long Col, long Row, BOOL Dirty)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_BOOL ;
		InvokeHelper(0x105, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Dirty);
		return result;
	}
	void SetColItemData(long Col, long Value)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x106, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Col, Value);
	}
	BOOL SetCustomName(LPCTSTR Name, LPCTSTR Value)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x107, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Name, Value);
		return result;
	}
	BOOL SetDataFillData(VARIANT Var)
	{
		BOOL result;
		static BYTE parms[] = VTS_VARIANT ;
		InvokeHelper(0x108, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, &Var);
		return result;
	}
	void SetItemData(long Value)
	{
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x109, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Value);
	}
	void SetIteration(BOOL Iteration, short MaxIterations, double MaxChange)
	{
		static BYTE parms[] = VTS_BOOL VTS_I2 VTS_R8 ;
		InvokeHelper(0x10a, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Iteration, MaxIterations, MaxChange);
	}
	void SetRefStyle(short RefStyle)
	{
		static BYTE parms[] = VTS_I2 ;
		InvokeHelper(0x10b, DISPATCH_METHOD, VT_EMPTY, NULL, parms, RefStyle);
	}
	void SetRowItemData(long Row, long Value)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x10c, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Row, Value);
	}
	void SetText(long Col, long Row, VARIANT Var)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_VARIANT ;
		InvokeHelper(0x10d, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Col, Row, &Var);
	}
	void TwipsToColWidth(long Twips, float * ColWidth)
	{
		static BYTE parms[] = VTS_I4 VTS_PR4 ;
		InvokeHelper(0x10e, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Twips, ColWidth);
	}
	void TwipsToRowHeight(long Row, long Twips, float * RowHeight)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PR4 ;
		InvokeHelper(0x10f, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Row, Twips, RowHeight);
	}
	BOOL GetActionKey(short Action, BOOL * fShift, BOOL * fCtrl, short * Key)
	{
		BOOL result;
		static BYTE parms[] = VTS_I2 VTS_PBOOL VTS_PBOOL VTS_PI2 ;
		InvokeHelper(0x124, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Action, fShift, fCtrl, Key);
		return result;
	}
	BOOL SetActionKey(short Action, BOOL fShift, BOOL fCtrl, short Key)
	{
		BOOL result;
		static BYTE parms[] = VTS_I2 VTS_BOOL VTS_BOOL VTS_I2 ;
		InvokeHelper(0x125, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Action, fShift, fCtrl, Key);
		return result;
	}
	BOOL GetOddEvenRowColor(long * clrBackOdd, long * clrForeOdd, long * clrBackEven, long * clrForeEven)
	{
		BOOL result;
		static BYTE parms[] = VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x126, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, clrBackOdd, clrForeOdd, clrBackEven, clrForeEven);
		return result;
	}
	BOOL SetOddEvenRowColor(long clrBackOdd, long clrForeOdd, long clrBackEven, long clrForeEven)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x127, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, clrBackOdd, clrForeOdd, clrBackEven, clrForeEven);
		return result;
	}
	BOOL GetFloat(long Col, long Row, double * Value)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PR8 ;
		InvokeHelper(0x128, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Value);
		return result;
	}
	BOOL SetFloat(long Col, long Row, double Value)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_R8 ;
		InvokeHelper(0x129, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Value);
		return result;
	}
	BOOL GetInteger(long Col, long Row, long * Value)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PI4 ;
		InvokeHelper(0x12a, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Value);
		return result;
	}
	BOOL SetInteger(long Col, long Row, long Value)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x12b, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Value);
		return result;
	}
	void SetCalText(LPCTSTR ShortDays, LPCTSTR LongDays, LPCTSTR ShortMonths, LPCTSTR LongMonths, LPCTSTR OkText, LPCTSTR CancelText)
	{
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x12c, DISPATCH_METHOD, VT_EMPTY, NULL, parms, ShortDays, LongDays, ShortMonths, LongMonths, OkText, CancelText);
	}
	BOOL GetTextTipAppearance(BSTR * FontName, short * FontSize, BOOL * FontBold, BOOL * FontItalic, long * BackColor, long * ForeColor)
	{
		BOOL result;
		static BYTE parms[] = VTS_PBSTR VTS_PI2 VTS_PBOOL VTS_PBOOL VTS_PI4 VTS_PI4 ;
		InvokeHelper(0x12d, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FontName, FontSize, FontBold, FontItalic, BackColor, ForeColor);
		return result;
	}
	BOOL SetTextTipAppearance(LPCTSTR FontName, short FontSize, BOOL FontBold, BOOL FontItalic, long BackColor, long ForeColor)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_I2 VTS_BOOL VTS_BOOL VTS_I4 VTS_I4 ;
		InvokeHelper(0x12e, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FontName, FontSize, FontBold, FontItalic, BackColor, ForeColor);
		return result;
	}
	BOOL ExportToHTML(LPCTSTR FileName, BOOL AppendFlag, LPCTSTR LogFile)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BOOL VTS_BSTR ;
		InvokeHelper(0x12f, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName, AppendFlag, LogFile);
		return result;
	}
	BOOL ExportRangeToHTML(long Col, long Row, long Col2, long Row2, LPCTSTR FileName, BOOL AppendFlag, LPCTSTR LogFile)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BOOL VTS_BSTR ;
		InvokeHelper(0x130, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Col2, Row2, FileName, AppendFlag, LogFile);
		return result;
	}
	BOOL GetArray(long ColLeft, long RowTop, VARIANT VarArray)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_VARIANT ;
		InvokeHelper(0x131, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, ColLeft, RowTop, &VarArray);
		return result;
	}
	BOOL SetArray(long ColLeft, long RowTop, VARIANT VarArray)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_VARIANT ;
		InvokeHelper(0x132, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, ColLeft, RowTop, &VarArray);
		return result;
	}
	short IsExcelFile(LPCTSTR FileName)
	{
		short result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x133, DISPATCH_METHOD, VT_I2, (void*)&result, parms, FileName);
		return result;
	}
	BOOL GetExcelSheetList(LPCTSTR FileName, VARIANT VarArray, short * ListCount, LPCTSTR LogFileName, short * WorkbookHandle, BOOL Replace)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_VARIANT VTS_PI2 VTS_BSTR VTS_PI2 VTS_BOOL ;
		InvokeHelper(0x134, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName, &VarArray, ListCount, LogFileName, WorkbookHandle, Replace);
		return result;
	}
	BOOL ImportExcelSheet(short WorkbookHandle, VARIANT Sheet)
	{
		BOOL result;
		static BYTE parms[] = VTS_I2 VTS_VARIANT ;
		InvokeHelper(0x135, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, WorkbookHandle, &Sheet);
		return result;
	}
	BOOL ExportToExcel(LPCTSTR FileName, LPCTSTR SheetName, LPCTSTR LogFileName)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x136, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName, SheetName, LogFileName);
		return result;
	}
	BOOL EnumCustomFunction(LPCTSTR PrevFuncName, BSTR * FuncName)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_PBSTR ;
		InvokeHelper(0x137, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, PrevFuncName, FuncName);
		return result;
	}
	BOOL GetCustomFunction(LPCTSTR FuncName, short * MinArgs, short * MaxArgs, long * Flags)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_PI2 VTS_PI2 VTS_PI4 ;
		InvokeHelper(0x138, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FuncName, MinArgs, MaxArgs, Flags);
		return result;
	}
	BOOL RemoveCustomFunction(LPCTSTR FuncName)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x139, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FuncName);
		return result;
	}
	BOOL ReCalcCell(long Col, long Row)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x13a, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row);
		return result;
	}
	void OLEDrag()
	{
		InvokeHelper(0x13c, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	long get_ColUserSortIndicator(long lCol)
	{
		long result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x142, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms, lCol);
		return result;
	}
	void put_ColUserSortIndicator(long lCol, long newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x142, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, lCol, newValue);
	}
	BOOL IsFetchCellNote()
	{
		BOOL result;
		InvokeHelper(0x143, DISPATCH_METHOD, VT_BOOL, (void*)&result, NULL);
		return result;
	}
	BOOL ExportToXML(LPCTSTR FileName, LPCTSTR Root, LPCTSTR Collection, long Flags, LPCTSTR LogFile)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0x144, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName, Root, Collection, Flags, LogFile);
		return result;
	}
	BOOL ExportRangeToXML(long Col, long Row, long Col2, long Row2, LPCTSTR FileName, LPCTSTR Root, LPCTSTR Collection, long Flags, LPCTSTR LogFile)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0x145, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Col2, Row2, FileName, Root, Collection, Flags, LogFile);
		return result;
	}
	BOOL ExportToXMLBuffer(LPCTSTR Root, LPCTSTR Collection, BSTR * Buff, long Flags, LPCTSTR LogFile)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_PBSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0x146, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Root, Collection, Buff, Flags, LogFile);
		return result;
	}
	BOOL ExportRangeToXMLBuffer(long Col, long Row, long Col2, long Row2, LPCTSTR Root, LPCTSTR Collection, BSTR * Buff, long Flags, LPCTSTR LogFile)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_PBSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0x147, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Col2, Row2, Root, Collection, Buff, Flags, LogFile);
		return result;
	}
	BOOL OwnerPrintDraw(long hDC, long Left, long Top, long Right, long Bottom, short Page)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I2 ;
		InvokeHelper(0x148, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, hDC, Left, Top, Right, Bottom, Page);
		return result;
	}
	BOOL OwnerPrintPageCount(long hDC, long Left, long Top, long Right, long Bottom, short * PageCount)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PI2 ;
		InvokeHelper(0x149, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, hDC, Left, Top, Right, Bottom, PageCount);
		return result;
	}
	BOOL ExportRangeToTextFile(long Col, long Row, long Col2, long Row2, LPCTSTR FileName, LPCTSTR CellDelim, LPCTSTR ColDelim, LPCTSTR RowDelim, long Flags, LPCTSTR LogFile)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0x14b, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Col2, Row2, FileName, CellDelim, ColDelim, RowDelim, Flags, LogFile);
		return result;
	}
	BOOL ExportToTextFile(LPCTSTR FileName, LPCTSTR CellDelim, LPCTSTR ColDelim, LPCTSTR RowDelim, long Flags, LPCTSTR LogFile)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0x14c, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName, CellDelim, ColDelim, RowDelim, Flags, LogFile);
		return result;
	}
	BOOL LoadTextFile(LPCTSTR FileName, LPCTSTR CellDelim, LPCTSTR ColDelim, LPCTSTR RowDelim, long Flags, LPCTSTR LogFile)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0x14d, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName, CellDelim, ColDelim, RowDelim, Flags, LogFile);
		return result;
	}
	void SetCalTextOverride(LPCTSTR ShortDays, LPCTSTR LongDays, LPCTSTR ShortMonths, LPCTSTR LongMonths, LPCTSTR OkText, LPCTSTR CancelText)
	{
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR ;
		InvokeHelper(0x14e, DISPATCH_METHOD, VT_EMPTY, NULL, parms, ShortDays, LongDays, ShortMonths, LongMonths, OkText, CancelText);
	}
	void GetCalTextOverride(BSTR * ShortDays, short * LenShortDays, BSTR * LongDays, short * LenLongDays, BSTR * ShortMonths, short * LenShortMonths, BSTR * LongMonths, short * LenLongMonths, BSTR * OkText, short * LenOkText, BSTR * CancelText, short * LenCancelText)
	{
		static BYTE parms[] = VTS_PBSTR VTS_PI2 VTS_PBSTR VTS_PI2 VTS_PBSTR VTS_PI2 VTS_PBSTR VTS_PI2 VTS_PBSTR VTS_PI2 VTS_PBSTR VTS_PI2 ;
		InvokeHelper(0x14f, DISPATCH_METHOD, VT_EMPTY, NULL, parms, ShortDays, LenShortDays, LongDays, LenLongDays, ShortMonths, LenShortMonths, LongMonths, LenLongMonths, OkText, LenOkText, CancelText, LenCancelText);
	}
	void ScriptCFGetCellParam(short Param, VARIANT * Col, VARIANT * Row)
	{
		static BYTE parms[] = VTS_I2 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1b3, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Param, Col, Row);
	}
	double ScriptCFGetDoubleParamExt(short Param, VARIANT * ParamValue)
	{
		double result;
		static BYTE parms[] = VTS_I2 VTS_PVARIANT ;
		InvokeHelper(0x1b4, DISPATCH_METHOD, VT_R8, (void*)&result, parms, Param, ParamValue);
		return result;
	}
	BOOL ScriptCFGetParamInfo(short Param, VARIANT * Type, VARIANT * Status)
	{
		BOOL result;
		static BYTE parms[] = VTS_I2 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1b5, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Param, Type, Status);
		return result;
	}
	void ScriptCFGetRangeParam(short Param, VARIANT * Col, VARIANT * Row, VARIANT * Col2, VARIANT * Row2)
	{
		static BYTE parms[] = VTS_I2 VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1b6, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Param, Col, Row, Col2, Row2);
	}
	void ScriptColWidthToTwips(float Width, VARIANT * Twips)
	{
		static BYTE parms[] = VTS_R4 VTS_PVARIANT ;
		InvokeHelper(0x1b7, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Width, Twips);
	}
	void ScriptGetBottomRightCell(VARIANT * Col, VARIANT * Row)
	{
		static BYTE parms[] = VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1b8, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Col, Row);
	}
	void ScriptGetCellFromScreenCoord(VARIANT * Col, VARIANT * Row, long x, long y)
	{
		static BYTE parms[] = VTS_PVARIANT VTS_PVARIANT VTS_I4 VTS_I4 ;
		InvokeHelper(0x1b9, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Col, Row, x, y);
	}
	BOOL ScriptGetCellPos(long Col, long Row, VARIANT * x, VARIANT * y, VARIANT * Width, VARIANT * Height)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1ba, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, x, y, Width, Height);
		return result;
	}
	void ScriptGetClientArea(VARIANT * Width, VARIANT * Height)
	{
		static BYTE parms[] = VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1bb, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Width, Height);
	}
	void ScriptGetFirstValidCell(VARIANT * Col, VARIANT * Row)
	{
		static BYTE parms[] = VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1bc, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Col, Row);
	}
	BOOL ScriptGetIteration(VARIANT * MaxIterations, VARIANT * MaxChange)
	{
		BOOL result;
		static BYTE parms[] = VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1bd, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, MaxIterations, MaxChange);
		return result;
	}
	void ScriptGetLastValidCell(VARIANT * Col, VARIANT * Row)
	{
		static BYTE parms[] = VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1be, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Col, Row);
	}
	void ScriptRowHeightToTwips(long Row, float Height, VARIANT * Twips)
	{
		static BYTE parms[] = VTS_I4 VTS_R4 VTS_PVARIANT ;
		InvokeHelper(0x1bf, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Row, Height, Twips);
	}
	void ScriptTwipsToColWidth(long Twips, VARIANT * ColWidth)
	{
		static BYTE parms[] = VTS_I4 VTS_PVARIANT ;
		InvokeHelper(0x1c0, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Twips, ColWidth);
	}
	void ScriptTwipsToRowHeight(long Row, long Twips, VARIANT * RowHeight)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PVARIANT ;
		InvokeHelper(0x1c1, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Row, Twips, RowHeight);
	}
	BOOL ScriptGetActionKey(short Action, VARIANT * fShift, VARIANT * fCtrl, VARIANT * Key)
	{
		BOOL result;
		static BYTE parms[] = VTS_I2 VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1c2, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Action, fShift, fCtrl, Key);
		return result;
	}
	BOOL ScriptGetOddEvenRowColor(VARIANT * clrBackOdd, VARIANT * clrForeOdd, VARIANT * clrBackEven, VARIANT * clrForeEven)
	{
		BOOL result;
		static BYTE parms[] = VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1c3, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, clrBackOdd, clrForeOdd, clrBackEven, clrForeEven);
		return result;
	}
	BOOL ScriptGetFloat(long Col, long Row, VARIANT * Value)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PVARIANT ;
		InvokeHelper(0x1c4, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Value);
		return result;
	}
	BOOL ScriptGetInteger(long Col, long Row, VARIANT * Value)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PVARIANT ;
		InvokeHelper(0x1c5, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Value);
		return result;
	}
	BOOL ScriptGetTextTipAppearance(VARIANT * FontName, VARIANT * FontSize, VARIANT * FontBold, VARIANT * FontItalic, VARIANT * BackColor, VARIANT * ForeColor)
	{
		BOOL result;
		static BYTE parms[] = VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1c6, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FontName, FontSize, FontBold, FontItalic, BackColor, ForeColor);
		return result;
	}
	BOOL ScriptGetExcelSheetList(LPCTSTR FileName, VARIANT VarArray, VARIANT * ListCount, LPCTSTR LogFileName, VARIANT * WorkbookHandle, BOOL Replace)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_VARIANT VTS_PVARIANT VTS_BSTR VTS_PVARIANT VTS_BOOL ;
		InvokeHelper(0x1c7, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName, &VarArray, ListCount, LogFileName, WorkbookHandle, Replace);
		return result;
	}
	BOOL ScriptEnumCustomFunction(LPCTSTR PrevFuncName, VARIANT * FuncName)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_PVARIANT ;
		InvokeHelper(0x1c8, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, PrevFuncName, FuncName);
		return result;
	}
	BOOL ScriptGetCustomFunction(LPCTSTR FuncName, VARIANT * MinArgs, VARIANT * MaxArgs, VARIANT * Flags)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1c9, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FuncName, MinArgs, MaxArgs, Flags);
		return result;
	}
	BOOL ScriptExportToXMLBuffer(LPCTSTR Root, LPCTSTR Collection, VARIANT * Buff, long Flags, LPCTSTR LogFile)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_PVARIANT VTS_I4 VTS_BSTR ;
		InvokeHelper(0x1ca, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Root, Collection, Buff, Flags, LogFile);
		return result;
	}
	BOOL ScriptExportRangeToXMLBuffer(long Col, long Row, long Col2, long Row2, LPCTSTR Root, LPCTSTR Collection, VARIANT * Buff, long Flags, LPCTSTR LogFile)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_PVARIANT VTS_I4 VTS_BSTR ;
		InvokeHelper(0x1cb, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Col2, Row2, Root, Collection, Buff, Flags, LogFile);
		return result;
	}
	long ScriptOwnerPrintPageCount(long hDC, long Left, long Top, long Right, long Bottom, VARIANT * PageCount)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PVARIANT ;
		InvokeHelper(0x1cc, DISPATCH_METHOD, VT_I4, (void*)&result, parms, hDC, Left, Top, Right, Bottom, PageCount);
		return result;
	}
	void ScriptGetCalTextOverride(VARIANT * ShortDays, VARIANT * LenShortDays, VARIANT * LongDays, VARIANT * LenLongDays, VARIANT * ShortMonths, VARIANT * LenShortMonths, VARIANT * LongMonths, VARIANT * LenLongMonths, VARIANT * OkText, VARIANT * LenOkText, VARIANT * CancelText, VARIANT * LenCancelText)
	{
		static BYTE parms[] = VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1cd, DISPATCH_METHOD, VT_EMPTY, NULL, parms, ShortDays, LenShortDays, LongDays, LenLongDays, ShortMonths, LenShortMonths, LongMonths, LenLongMonths, OkText, LenOkText, CancelText, LenCancelText);
	}
	void PrintSheet(VARIANT * Flags)
	{
		static BYTE parms[] = VTS_PVARIANT ;
		InvokeHelper(0x1ce, DISPATCH_METHOD, VT_EMPTY, NULL, parms, Flags);
	}
	void AddSelection(long lCol, long lRow, long lCol2, long lRow2)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1cf, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lRow, lCol2, lRow2);
	}
	void ClearRange(long lCol, long lRow, long lCol2, long lRow2, BOOL bDataOnly)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BOOL ;
		InvokeHelper(0x1d0, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lRow, lCol2, lRow2, bDataOnly);
	}
	void ClearSelection()
	{
		InvokeHelper(0x1d1, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void ClipboardCopy()
	{
		InvokeHelper(0x1d2, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void ClipboardCut()
	{
		InvokeHelper(0x1d3, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void ClipboardPaste()
	{
		InvokeHelper(0x1d4, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void CopyRange(long lCol, long lRow, long lCol2, long lRow2, long lColDest, long lRowDest)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1d5, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lRow, lCol2, lRow2, lColDest, lRowDest);
	}
	void DeleteCols(long lCol, long lNumCols)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x1d6, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lNumCols);
	}
	void DeleteRows(long lRow, long lNumRows)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x1d7, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lRow, lNumRows);
	}
	void DataSave()
	{
		InvokeHelper(0x1d8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void GetCellBorder(long lCol, long lRow, short nIndex, VARIANT * pcrColor, VARIANT * pnStyle)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I2 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1d9, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lRow, nIndex, pcrColor, pnStyle);
	}
	void GetSelection(long lIndex, VARIANT * plCol, VARIANT * plRow, VARIANT * plCol2, VARIANT * plRow2)
	{
		static BYTE parms[] = VTS_I4 VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1da, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lIndex, plCol, plRow, plCol2, plRow2);
	}
	void InsertCols(long lCol, long lNumCols)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x1db, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lNumCols);
	}
	void InsertRows(long lRow, long lNumRows)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x1dc, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lRow, lNumRows);
	}
	void MoveRange(long lCol, long lRow, long lCol2, long lRow2, long lColDest, long lRowDest)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1dd, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lRow, lCol2, lRow2, lColDest, lRowDest);
	}
	void ReCalc()
	{
		InvokeHelper(0x1de, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void Reset()
	{
		InvokeHelper(0x1df, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void SetActiveCell(long lCol, long lRow)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x1e0, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lRow);
	}
	void SetCellBorder(long lCol, long lRow, long lCol2, long lRow2, short nIndex, unsigned long crColor, long nStyle)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I2 VTS_UI4 VTS_I4 ;
		InvokeHelper(0x1e1, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lRow, lCol2, lRow2, nIndex, crColor, nStyle);
	}
	void SetSelection(long lCol, long lRow, long lCol2, long lRow2)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1e2, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lRow, lCol2, lRow2);
	}
	void ShowCell(long lCol, long lRow, long nPosition)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1e3, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lRow, nPosition);
	}
	BOOL Sort(long lCol, long lRow, long lCol2, long lRow2, long nSortBy, VARIANT * SortKeys, VARIANT * SortKeyOrders)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1e4, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, lCol, lRow, lCol2, lRow2, nSortBy, SortKeys, SortKeyOrders);
		return result;
	}
	void SwapRange(long lCol, long lRow, long lCol2, long lRow2, long lColDest, long lRowDest)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1e5, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lRow, lCol2, lRow2, lColDest, lRowDest);
	}
	void TypeComboBoxClear(long lCol, long lRow)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x1e6, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lRow);
	}
	void TypeComboBoxRemoveItem(long lCol, long lRow, short nIndex)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I2 ;
		InvokeHelper(0x1e7, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lRow, nIndex);
	}
	void VirtualRefresh()
	{
		InvokeHelper(0x1e8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	BOOL AddCellSpan(long lCol, long lRow, long lNumCols, long lNumRows)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1ea, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, lCol, lRow, lNumCols, lNumRows);
		return result;
	}
	long GetCellSpan(long lCol, long lRow, VARIANT * pvColAnchor, VARIANT * pvRowAnchor, VARIANT * pvNumCols, VARIANT * pvNumRows)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT ;
		InvokeHelper(0x1eb, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lCol, lRow, pvColAnchor, pvRowAnchor, pvNumCols, pvNumRows);
		return result;
	}
	void RemoveCellSpan(long lCol, long lRow)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 ;
		InvokeHelper(0x1ec, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lRow);
	}
	long GetColFromID(LPCTSTR ColID)
	{
		long result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x1ed, DISPATCH_METHOD, VT_I4, (void*)&result, parms, ColID);
		return result;
	}
	void CopyColRange(long lCol, long lCol2, long lColDest)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1ee, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lCol2, lColDest);
	}
	void CopyRowRange(long lRow, long lRow2, long lRowDest)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1ef, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lRow, lRow2, lRowDest);
	}
	void MoveColRange(long lCol, long lCol2, long lColDest)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1f0, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lCol2, lColDest);
	}
	void MoveRowRange(long lRow, long lRow2, long lRowDest)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1f1, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lRow, lRow2, lRowDest);
	}
	void SwapColRange(long lCol, long lCol2, long lColDest)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1f2, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lCol, lCol2, lColDest);
	}
	void SwapRowRange(long lRow, long lRow2, long lRowDest)
	{
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 ;
		InvokeHelper(0x1f3, DISPATCH_METHOD, VT_EMPTY, NULL, parms, lRow, lRow2, lRowDest);
	}
	long SearchCol(long lCol, long lRowStart, long lRowEnd, LPCTSTR Text, long SearchFlags)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_I4 ;
		InvokeHelper(0x1f4, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lCol, lRowStart, lRowEnd, Text, SearchFlags);
		return result;
	}
	long SearchRow(long lRow, long lColStart, long lColEnd, LPCTSTR Text, long SearchFlags)
	{
		long result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_I4 ;
		InvokeHelper(0x1f5, DISPATCH_METHOD, VT_I4, (void*)&result, parms, lRow, lColStart, lColEnd, Text, SearchFlags);
		return result;
	}
	BOOL get_EventEnabled(long Event)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 ;
		InvokeHelper(0x1f6, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, parms, Event);
		return result;
	}
	void put_EventEnabled(long Event, BOOL newValue)
	{
		static BYTE parms[] = VTS_I4 VTS_BOOL ;
		InvokeHelper(0x1f6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms, Event, newValue);
	}
	BOOL ExportRangeToTextFileU(long Col, long Row, long Col2, long Row2, LPCTSTR FileName, LPCTSTR CellDelim, LPCTSTR ColDelim, LPCTSTR RowDelim, long Flags, LPCTSTR LogFile)
	{
		BOOL result;
		static BYTE parms[] = VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0x1f7, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, Col, Row, Col2, Row2, FileName, CellDelim, ColDelim, RowDelim, Flags, LogFile);
		return result;
	}
	BOOL ExportToTextFileU(LPCTSTR FileName, LPCTSTR CellDelim, LPCTSTR ColDelim, LPCTSTR RowDelim, long Flags, LPCTSTR LogFile)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR ;
		InvokeHelper(0x1f8, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName, CellDelim, ColDelim, RowDelim, Flags, LogFile);
		return result;
	}
	BOOL SaveTabFileU(LPCTSTR FileName)
	{
		BOOL result;
		static BYTE parms[] = VTS_BSTR ;
		InvokeHelper(0x1f9, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, FileName);
		return result;
	}
	void Refresh()
	{
		InvokeHelper(DISPID_REFRESH, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}
	void AboutBox()
	{
		InvokeHelper(DISPID_ABOUTBOX, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
	}

// Properties
//

long GetAction()
{
	long result;
	GetProperty(0x1, VT_I4, (void*)&result);
	return result;
}
void SetAction(long propVal)
{
	SetProperty(0x1, VT_I4, propVal);
}
long GetActiveCol()
{
	long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}
void SetActiveCol(long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}
long GetActiveRow()
{
	long result;
	GetProperty(0x3, VT_I4, (void*)&result);
	return result;
}
void SetActiveRow(long propVal)
{
	SetProperty(0x3, VT_I4, propVal);
}
BOOL GetAllowCellOverflow()
{
	BOOL result;
	GetProperty(0x4, VT_BOOL, (void*)&result);
	return result;
}
void SetAllowCellOverflow(BOOL propVal)
{
	SetProperty(0x4, VT_BOOL, propVal);
}
BOOL GetAllowDragDrop()
{
	BOOL result;
	GetProperty(0x5, VT_BOOL, (void*)&result);
	return result;
}
void SetAllowDragDrop(BOOL propVal)
{
	SetProperty(0x5, VT_BOOL, propVal);
}
BOOL GetAllowMultiBlocks()
{
	BOOL result;
	GetProperty(0x6, VT_BOOL, (void*)&result);
	return result;
}
void SetAllowMultiBlocks(BOOL propVal)
{
	SetProperty(0x6, VT_BOOL, propVal);
}
BOOL GetAllowUserFormulas()
{
	BOOL result;
	GetProperty(0x7, VT_BOOL, (void*)&result);
	return result;
}
void SetAllowUserFormulas(BOOL propVal)
{
	SetProperty(0x7, VT_BOOL, propVal);
}
BOOL GetArrowsExitEditMode()
{
	BOOL result;
	GetProperty(0x8, VT_BOOL, (void*)&result);
	return result;
}
void SetArrowsExitEditMode(BOOL propVal)
{
	SetProperty(0x8, VT_BOOL, propVal);
}
BOOL GetAutoCalc()
{
	BOOL result;
	GetProperty(0x9, VT_BOOL, (void*)&result);
	return result;
}
void SetAutoCalc(BOOL propVal)
{
	SetProperty(0x9, VT_BOOL, propVal);
}
BOOL GetAutoClipboard()
{
	BOOL result;
	GetProperty(0xa, VT_BOOL, (void*)&result);
	return result;
}
void SetAutoClipboard(BOOL propVal)
{
	SetProperty(0xa, VT_BOOL, propVal);
}
BOOL GetAutoSize()
{
	BOOL result;
	GetProperty(0xb, VT_BOOL, (void*)&result);
	return result;
}
void SetAutoSize(BOOL propVal)
{
	SetProperty(0xb, VT_BOOL, propVal);
}
BOOL GetBlockMode()
{
	BOOL result;
	GetProperty(0xc, VT_BOOL, (void*)&result);
	return result;
}
void SetBlockMode(BOOL propVal)
{
	SetProperty(0xc, VT_BOOL, propVal);
}
short GetButtonDrawMode()
{
	short result;
	GetProperty(0xd, VT_I2, (void*)&result);
	return result;
}
void SetButtonDrawMode(short propVal)
{
	SetProperty(0xd, VT_I2, propVal);
}
BOOL GetCalcDependencies()
{
	BOOL result;
	GetProperty(0xe, VT_BOOL, (void*)&result);
	return result;
}
void SetCalcDependencies(BOOL propVal)
{
	SetProperty(0xe, VT_BOOL, propVal);
}
unsigned long GetCellBorderColor()
{
	unsigned long result;
	GetProperty(0xf, VT_UI4, (void*)&result);
	return result;
}
void SetCellBorderColor(unsigned long propVal)
{
	SetProperty(0xf, VT_UI4, propVal);
}
long GetCellBorderStyle()
{
	long result;
	GetProperty(0x10, VT_I4, (void*)&result);
	return result;
}
void SetCellBorderStyle(long propVal)
{
	SetProperty(0x10, VT_I4, propVal);
}
short GetCellBorderType()
{
	short result;
	GetProperty(0x11, VT_I2, (void*)&result);
	return result;
}
void SetCellBorderType(short propVal)
{
	SetProperty(0x11, VT_I2, propVal);
}
long GetCellType()
{
	long result;
	GetProperty(0x12, VT_I4, (void*)&result);
	return result;
}
void SetCellType(long propVal)
{
	SetProperty(0x12, VT_I4, propVal);
}
BOOL GetChangeMade()
{
	BOOL result;
	GetProperty(0x13, VT_BOOL, (void*)&result);
	return result;
}
void SetChangeMade(BOOL propVal)
{
	SetProperty(0x13, VT_BOOL, propVal);
}
CString GetClip()
{
	CString result;
	GetProperty(0x14, VT_BSTR, (void*)&result);
	return result;
}
void SetClip(CString propVal)
{
	SetProperty(0x14, VT_BSTR, propVal);
}
CString GetClipValue()
{
	CString result;
	GetProperty(0x15, VT_BSTR, (void*)&result);
	return result;
}
void SetClipValue(CString propVal)
{
	SetProperty(0x15, VT_BSTR, propVal);
}
long GetCol()
{
	long result;
	GetProperty(0x16, VT_I4, (void*)&result);
	return result;
}
void SetCol(long propVal)
{
	SetProperty(0x16, VT_I4, propVal);
}
long GetCol2()
{
	long result;
	GetProperty(0x17, VT_I4, (void*)&result);
	return result;
}
void SetCol2(long propVal)
{
	SetProperty(0x17, VT_I4, propVal);
}
long GetColHeaderDisplay()
{
	long result;
	GetProperty(0x18, VT_I4, (void*)&result);
	return result;
}
void SetColHeaderDisplay(long propVal)
{
	SetProperty(0x18, VT_I4, propVal);
}
BOOL GetColHidden()
{
	BOOL result;
	GetProperty(0x19, VT_BOOL, (void*)&result);
	return result;
}
void SetColHidden(BOOL propVal)
{
	SetProperty(0x19, VT_BOOL, propVal);
}
BOOL GetColPageBreak()
{
	BOOL result;
	GetProperty(0x1a, VT_BOOL, (void*)&result);
	return result;
}
void SetColPageBreak(BOOL propVal)
{
	SetProperty(0x1a, VT_BOOL, propVal);
}
long GetColsFrozen()
{
	long result;
	GetProperty(0x1b, VT_I4, (void*)&result);
	return result;
}
void SetColsFrozen(long propVal)
{
	SetProperty(0x1b, VT_I4, propVal);
}
long GetCursorStyle()
{
	long result;
	GetProperty(0x1c, VT_I4, (void*)&result);
	return result;
}
void SetCursorStyle(long propVal)
{
	SetProperty(0x1c, VT_I4, propVal);
}
long GetCursorType()
{
	long result;
	GetProperty(0x1d, VT_I4, (void*)&result);
	return result;
}
void SetCursorType(long propVal)
{
	SetProperty(0x1d, VT_I4, propVal);
}
BOOL GetDataChanged()
{
	BOOL result;
	GetProperty(0x1e, VT_BOOL, (void*)&result);
	return result;
}
void SetDataChanged(BOOL propVal)
{
	SetProperty(0x1e, VT_BOOL, propVal);
}
long GetDataColCnt()
{
	long result;
	GetProperty(0x1f, VT_I4, (void*)&result);
	return result;
}
void SetDataColCnt(long propVal)
{
	SetProperty(0x1f, VT_I4, propVal);
}
CString GetDataField()
{
	CString result;
	GetProperty(0x20, VT_BSTR, (void*)&result);
	return result;
}
void SetDataField(CString propVal)
{
	SetProperty(0x20, VT_BSTR, propVal);
}
BOOL GetDataFillEvent()
{
	BOOL result;
	GetProperty(0x21, VT_BOOL, (void*)&result);
	return result;
}
void SetDataFillEvent(BOOL propVal)
{
	SetProperty(0x21, VT_BOOL, propVal);
}
long GetDataRowCnt()
{
	long result;
	GetProperty(0x22, VT_I4, (void*)&result);
	return result;
}
void SetDataRowCnt(long propVal)
{
	SetProperty(0x22, VT_I4, propVal);
}
LPUNKNOWN GetDataSource()
{
	LPUNKNOWN result;
	GetProperty(0x23, VT_UNKNOWN, (void*)&result);
	return result;
}
void SetDataSource(LPUNKNOWN propVal)
{
	SetProperty(0x23, VT_UNKNOWN, propVal);
}
BOOL GetDAutoCellTypes()
{
	BOOL result;
	GetProperty(0x24, VT_BOOL, (void*)&result);
	return result;
}
void SetDAutoCellTypes(BOOL propVal)
{
	SetProperty(0x24, VT_BOOL, propVal);
}
BOOL GetDAutoFill()
{
	BOOL result;
	GetProperty(0x25, VT_BOOL, (void*)&result);
	return result;
}
void SetDAutoFill(BOOL propVal)
{
	SetProperty(0x25, VT_BOOL, propVal);
}
BOOL GetDAutoHeadings()
{
	BOOL result;
	GetProperty(0x26, VT_BOOL, (void*)&result);
	return result;
}
void SetDAutoHeadings(BOOL propVal)
{
	SetProperty(0x26, VT_BOOL, propVal);
}
BOOL GetDAutoSave()
{
	BOOL result;
	GetProperty(0x27, VT_BOOL, (void*)&result);
	return result;
}
void SetDAutoSave(BOOL propVal)
{
	SetProperty(0x27, VT_BOOL, propVal);
}
long GetDAutoSizeCols()
{
	long result;
	GetProperty(0x28, VT_I4, (void*)&result);
	return result;
}
void SetDAutoSizeCols(long propVal)
{
	SetProperty(0x28, VT_I4, propVal);
}
long GetDestCol()
{
	long result;
	GetProperty(0x29, VT_I4, (void*)&result);
	return result;
}
void SetDestCol(long propVal)
{
	SetProperty(0x29, VT_I4, propVal);
}
long GetDestRow()
{
	long result;
	GetProperty(0x2a, VT_I4, (void*)&result);
	return result;
}
void SetDestRow(long propVal)
{
	SetProperty(0x2a, VT_I4, propVal);
}
BOOL GetDInformActiveRowChange()
{
	BOOL result;
	GetProperty(0x2b, VT_BOOL, (void*)&result);
	return result;
}
void SetDInformActiveRowChange(BOOL propVal)
{
	SetProperty(0x2b, VT_BOOL, propVal);
}
BOOL GetDisplayColHeaders()
{
	BOOL result;
	GetProperty(0x2c, VT_BOOL, (void*)&result);
	return result;
}
void SetDisplayColHeaders(BOOL propVal)
{
	SetProperty(0x2c, VT_BOOL, propVal);
}
BOOL GetDisplayRowHeaders()
{
	BOOL result;
	GetProperty(0x2d, VT_BOOL, (void*)&result);
	return result;
}
void SetDisplayRowHeaders(BOOL propVal)
{
	SetProperty(0x2d, VT_BOOL, propVal);
}
long GetEditEnterAction()
{
	long result;
	GetProperty(0x2e, VT_I4, (void*)&result);
	return result;
}
void SetEditEnterAction(long propVal)
{
	SetProperty(0x2e, VT_I4, propVal);
}
BOOL GetEditMode()
{
	BOOL result;
	GetProperty(0x2f, VT_BOOL, (void*)&result);
	return result;
}
void SetEditMode(BOOL propVal)
{
	SetProperty(0x2f, VT_BOOL, propVal);
}
BOOL GetEditModePermanent()
{
	BOOL result;
	GetProperty(0x30, VT_BOOL, (void*)&result);
	return result;
}
void SetEditModePermanent(BOOL propVal)
{
	SetProperty(0x30, VT_BOOL, propVal);
}
BOOL GetEditModeReplace()
{
	BOOL result;
	GetProperty(0x31, VT_BOOL, (void*)&result);
	return result;
}
void SetEditModeReplace(BOOL propVal)
{
	SetProperty(0x31, VT_BOOL, propVal);
}
short GetFileNum()
{
	short result;
	GetProperty(0x32, VT_I2, (void*)&result);
	return result;
}
void SetFileNum(short propVal)
{
	SetProperty(0x32, VT_I2, propVal);
}
short GetFloatDefCurrencyChar()
{
	short result;
	GetProperty(0x33, VT_I2, (void*)&result);
	return result;
}
void SetFloatDefCurrencyChar(short propVal)
{
	SetProperty(0x33, VT_I2, propVal);
}
short GetFloatDefDecimalChar()
{
	short result;
	GetProperty(0x34, VT_I2, (void*)&result);
	return result;
}
void SetFloatDefDecimalChar(short propVal)
{
	SetProperty(0x34, VT_I2, propVal);
}
short GetFloatDefSepChar()
{
	short result;
	GetProperty(0x35, VT_I2, (void*)&result);
	return result;
}
void SetFloatDefSepChar(short propVal)
{
	SetProperty(0x35, VT_I2, propVal);
}
CString GetFormula()
{
	CString result;
	GetProperty(0x36, VT_BSTR, (void*)&result);
	return result;
}
void SetFormula(CString propVal)
{
	SetProperty(0x36, VT_BSTR, propVal);
}
unsigned long GetGrayAreaBackColor()
{
	unsigned long result;
	GetProperty(0x37, VT_UI4, (void*)&result);
	return result;
}
void SetGrayAreaBackColor(unsigned long propVal)
{
	SetProperty(0x37, VT_UI4, propVal);
}
unsigned long GetGridColor()
{
	unsigned long result;
	GetProperty(0x38, VT_UI4, (void*)&result);
	return result;
}
void SetGridColor(unsigned long propVal)
{
	SetProperty(0x38, VT_UI4, propVal);
}
BOOL GetGridShowHoriz()
{
	BOOL result;
	GetProperty(0x39, VT_BOOL, (void*)&result);
	return result;
}
void SetGridShowHoriz(BOOL propVal)
{
	SetProperty(0x39, VT_BOOL, propVal);
}
BOOL GetGridShowVert()
{
	BOOL result;
	GetProperty(0x3a, VT_BOOL, (void*)&result);
	return result;
}
void SetGridShowVert(BOOL propVal)
{
	SetProperty(0x3a, VT_BOOL, propVal);
}
BOOL GetGridSolid()
{
	BOOL result;
	GetProperty(0x3b, VT_BOOL, (void*)&result);
	return result;
}
void SetGridSolid(BOOL propVal)
{
	SetProperty(0x3b, VT_BOOL, propVal);
}
OLE_HANDLE GethDCPrinter()
{
	OLE_HANDLE result;
	GetProperty(0x3c, VT_I4, (void*)&result);
	return result;
}
void SethDCPrinter(OLE_HANDLE propVal)
{
	SetProperty(0x3c, VT_I4, propVal);
}
short GetInterfaceDesigner()
{
	short result;
	GetProperty(0x3d, VT_I2, (void*)&result);
	return result;
}
void SetInterfaceDesigner(short propVal)
{
	SetProperty(0x3d, VT_I2, propVal);
}
BOOL GetIsBlockSelected()
{
	BOOL result;
	GetProperty(0x3e, VT_BOOL, (void*)&result);
	return result;
}
void SetIsBlockSelected(BOOL propVal)
{
	SetProperty(0x3e, VT_BOOL, propVal);
}
long GetLeftCol()
{
	long result;
	GetProperty(0x3f, VT_I4, (void*)&result);
	return result;
}
void SetLeftCol(long propVal)
{
	SetProperty(0x3f, VT_I4, propVal);
}
BOOL GetLock()
{
	BOOL result;
	GetProperty(0x40, VT_BOOL, (void*)&result);
	return result;
}
void SetLock(BOOL propVal)
{
	SetProperty(0x40, VT_BOOL, propVal);
}
unsigned long GetLockBackColor()
{
	unsigned long result;
	GetProperty(0x41, VT_UI4, (void*)&result);
	return result;
}
void SetLockBackColor(unsigned long propVal)
{
	SetProperty(0x41, VT_UI4, propVal);
}
unsigned long GetLockForeColor()
{
	unsigned long result;
	GetProperty(0x42, VT_UI4, (void*)&result);
	return result;
}
void SetLockForeColor(unsigned long propVal)
{
	SetProperty(0x42, VT_UI4, propVal);
}
long GetMaxCols()
{
	long result;
	GetProperty(0x43, VT_I4, (void*)&result);
	return result;
}
void SetMaxCols(long propVal)
{
	SetProperty(0x43, VT_I4, propVal);
}
long GetMaxRows()
{
	long result;
	GetProperty(0x44, VT_I4, (void*)&result);
	return result;
}
void SetMaxRows(long propVal)
{
	SetProperty(0x44, VT_I4, propVal);
}
double GetMaxTextCellHeight()
{
	double result;
	GetProperty(0x45, VT_R8, (void*)&result);
	return result;
}
void SetMaxTextCellHeight(double propVal)
{
	SetProperty(0x45, VT_R8, propVal);
}
double GetMaxTextCellWidth()
{
	double result;
	GetProperty(0x46, VT_R8, (void*)&result);
	return result;
}
void SetMaxTextCellWidth(double propVal)
{
	SetProperty(0x46, VT_R8, propVal);
}
BOOL GetMoveActiveOnFocus()
{
	BOOL result;
	GetProperty(0x47, VT_BOOL, (void*)&result);
	return result;
}
void SetMoveActiveOnFocus(BOOL propVal)
{
	SetProperty(0x47, VT_BOOL, propVal);
}
long GetMultiSelCount()
{
	long result;
	GetProperty(0x48, VT_I4, (void*)&result);
	return result;
}
void SetMultiSelCount(long propVal)
{
	SetProperty(0x48, VT_I4, propVal);
}
long GetMultiSelIndex()
{
	long result;
	GetProperty(0x49, VT_I4, (void*)&result);
	return result;
}
void SetMultiSelIndex(long propVal)
{
	SetProperty(0x49, VT_I4, propVal);
}
BOOL GetNoBeep()
{
	BOOL result;
	GetProperty(0x4a, VT_BOOL, (void*)&result);
	return result;
}
void SetNoBeep(BOOL propVal)
{
	SetProperty(0x4a, VT_BOOL, propVal);
}
BOOL GetNoBorder()
{
	BOOL result;
	GetProperty(0x4b, VT_BOOL, (void*)&result);
	return result;
}
void SetNoBorder(BOOL propVal)
{
	SetProperty(0x4b, VT_BOOL, propVal);
}
long GetOperationMode()
{
	long result;
	GetProperty(0x4c, VT_I4, (void*)&result);
	return result;
}
void SetOperationMode(long propVal)
{
	SetProperty(0x4c, VT_I4, propVal);
}
long GetPosition()
{
	long result;
	GetProperty(0x4d, VT_I4, (void*)&result);
	return result;
}
void SetPosition(long propVal)
{
	SetProperty(0x4d, VT_I4, propVal);
}
CString GetPrintAbortMsg()
{
	CString result;
	GetProperty(0x4e, VT_BSTR, (void*)&result);
	return result;
}
void SetPrintAbortMsg(CString propVal)
{
	SetProperty(0x4e, VT_BSTR, propVal);
}
BOOL GetPrintBorder()
{
	BOOL result;
	GetProperty(0x4f, VT_BOOL, (void*)&result);
	return result;
}
void SetPrintBorder(BOOL propVal)
{
	SetProperty(0x4f, VT_BOOL, propVal);
}
BOOL GetPrintColHeaders()
{
	BOOL result;
	GetProperty(0x50, VT_BOOL, (void*)&result);
	return result;
}
void SetPrintColHeaders(BOOL propVal)
{
	SetProperty(0x50, VT_BOOL, propVal);
}
BOOL GetPrintColor()
{
	BOOL result;
	GetProperty(0x51, VT_BOOL, (void*)&result);
	return result;
}
void SetPrintColor(BOOL propVal)
{
	SetProperty(0x51, VT_BOOL, propVal);
}
CString GetPrintFooter()
{
	CString result;
	GetProperty(0x52, VT_BSTR, (void*)&result);
	return result;
}
void SetPrintFooter(CString propVal)
{
	SetProperty(0x52, VT_BSTR, propVal);
}
BOOL GetPrintGrid()
{
	BOOL result;
	GetProperty(0x53, VT_BOOL, (void*)&result);
	return result;
}
void SetPrintGrid(BOOL propVal)
{
	SetProperty(0x53, VT_BOOL, propVal);
}
CString GetPrintHeader()
{
	CString result;
	GetProperty(0x54, VT_BSTR, (void*)&result);
	return result;
}
void SetPrintHeader(CString propVal)
{
	SetProperty(0x54, VT_BSTR, propVal);
}
CString GetPrintJobName()
{
	CString result;
	GetProperty(0x55, VT_BSTR, (void*)&result);
	return result;
}
void SetPrintJobName(CString propVal)
{
	SetProperty(0x55, VT_BSTR, propVal);
}
long GetPrintMarginBottom()
{
	long result;
	GetProperty(0x56, VT_I4, (void*)&result);
	return result;
}
void SetPrintMarginBottom(long propVal)
{
	SetProperty(0x56, VT_I4, propVal);
}
long GetPrintMarginLeft()
{
	long result;
	GetProperty(0x57, VT_I4, (void*)&result);
	return result;
}
void SetPrintMarginLeft(long propVal)
{
	SetProperty(0x57, VT_I4, propVal);
}
long GetPrintMarginRight()
{
	long result;
	GetProperty(0x58, VT_I4, (void*)&result);
	return result;
}
void SetPrintMarginRight(long propVal)
{
	SetProperty(0x58, VT_I4, propVal);
}
long GetPrintMarginTop()
{
	long result;
	GetProperty(0x59, VT_I4, (void*)&result);
	return result;
}
void SetPrintMarginTop(long propVal)
{
	SetProperty(0x59, VT_I4, propVal);
}
short GetPrintPageEnd()
{
	short result;
	GetProperty(0x5a, VT_I2, (void*)&result);
	return result;
}
void SetPrintPageEnd(short propVal)
{
	SetProperty(0x5a, VT_I2, propVal);
}
short GetPrintPageStart()
{
	short result;
	GetProperty(0x5b, VT_I2, (void*)&result);
	return result;
}
void SetPrintPageStart(short propVal)
{
	SetProperty(0x5b, VT_I2, propVal);
}
BOOL GetPrintRowHeaders()
{
	BOOL result;
	GetProperty(0x5c, VT_BOOL, (void*)&result);
	return result;
}
void SetPrintRowHeaders(BOOL propVal)
{
	SetProperty(0x5c, VT_BOOL, propVal);
}
BOOL GetPrintShadows()
{
	BOOL result;
	GetProperty(0x5d, VT_BOOL, (void*)&result);
	return result;
}
void SetPrintShadows(BOOL propVal)
{
	SetProperty(0x5d, VT_BOOL, propVal);
}
long GetPrintType()
{
	long result;
	GetProperty(0x5e, VT_I4, (void*)&result);
	return result;
}
void SetPrintType(long propVal)
{
	SetProperty(0x5e, VT_I4, propVal);
}
BOOL GetPrintUseDataMax()
{
	BOOL result;
	GetProperty(0x5f, VT_BOOL, (void*)&result);
	return result;
}
void SetPrintUseDataMax(BOOL propVal)
{
	SetProperty(0x5f, VT_BOOL, propVal);
}
BOOL GetProcessTab()
{
	BOOL result;
	GetProperty(0x60, VT_BOOL, (void*)&result);
	return result;
}
void SetProcessTab(BOOL propVal)
{
	SetProperty(0x60, VT_BOOL, propVal);
}
BOOL GetProtect()
{
	BOOL result;
	GetProperty(0x61, VT_BOOL, (void*)&result);
	return result;
}
void SetProtect(BOOL propVal)
{
	SetProperty(0x61, VT_BOOL, propVal);
}
BOOL GetReDraw()
{
	BOOL result;
	GetProperty(0x62, VT_BOOL, (void*)&result);
	return result;
}
void SetReDraw(BOOL propVal)
{
	SetProperty(0x62, VT_BOOL, propVal);
}
BOOL GetRestrictCols()
{
	BOOL result;
	GetProperty(0x63, VT_BOOL, (void*)&result);
	return result;
}
void SetRestrictCols(BOOL propVal)
{
	SetProperty(0x63, VT_BOOL, propVal);
}
BOOL GetRestrictRows()
{
	BOOL result;
	GetProperty(0x64, VT_BOOL, (void*)&result);
	return result;
}
void SetRestrictRows(BOOL propVal)
{
	SetProperty(0x64, VT_BOOL, propVal);
}
BOOL GetRetainSelBlock()
{
	BOOL result;
	GetProperty(0x65, VT_BOOL, (void*)&result);
	return result;
}
void SetRetainSelBlock(BOOL propVal)
{
	SetProperty(0x65, VT_BOOL, propVal);
}
long GetRow()
{
	long result;
	GetProperty(0x66, VT_I4, (void*)&result);
	return result;
}
void SetRow(long propVal)
{
	SetProperty(0x66, VT_I4, propVal);
}
long GetRow2()
{
	long result;
	GetProperty(0x67, VT_I4, (void*)&result);
	return result;
}
void SetRow2(long propVal)
{
	SetProperty(0x67, VT_I4, propVal);
}
long GetRowHeaderDisplay()
{
	long result;
	GetProperty(0x68, VT_I4, (void*)&result);
	return result;
}
void SetRowHeaderDisplay(long propVal)
{
	SetProperty(0x68, VT_I4, propVal);
}
BOOL GetRowHidden()
{
	BOOL result;
	GetProperty(0x69, VT_BOOL, (void*)&result);
	return result;
}
void SetRowHidden(BOOL propVal)
{
	SetProperty(0x69, VT_BOOL, propVal);
}
BOOL GetRowPageBreak()
{
	BOOL result;
	GetProperty(0x6a, VT_BOOL, (void*)&result);
	return result;
}
void SetRowPageBreak(BOOL propVal)
{
	SetProperty(0x6a, VT_BOOL, propVal);
}
long GetRowsFrozen()
{
	long result;
	GetProperty(0x6b, VT_I4, (void*)&result);
	return result;
}
void SetRowsFrozen(long propVal)
{
	SetProperty(0x6b, VT_I4, propVal);
}
BOOL GetScrollBarExtMode()
{
	BOOL result;
	GetProperty(0x6c, VT_BOOL, (void*)&result);
	return result;
}
void SetScrollBarExtMode(BOOL propVal)
{
	SetProperty(0x6c, VT_BOOL, propVal);
}
BOOL GetScrollBarMaxAlign()
{
	BOOL result;
	GetProperty(0x6d, VT_BOOL, (void*)&result);
	return result;
}
void SetScrollBarMaxAlign(BOOL propVal)
{
	SetProperty(0x6d, VT_BOOL, propVal);
}
long GetScrollBars()
{
	long result;
	GetProperty(0x6e, VT_I4, (void*)&result);
	return result;
}
void SetScrollBars(long propVal)
{
	SetProperty(0x6e, VT_I4, propVal);
}
BOOL GetScrollBarShowMax()
{
	BOOL result;
	GetProperty(0x6f, VT_BOOL, (void*)&result);
	return result;
}
void SetScrollBarShowMax(BOOL propVal)
{
	SetProperty(0x6f, VT_BOOL, propVal);
}
long GetSelBlockCol()
{
	long result;
	GetProperty(0x70, VT_I4, (void*)&result);
	return result;
}
void SetSelBlockCol(long propVal)
{
	SetProperty(0x70, VT_I4, propVal);
}
long GetSelBlockCol2()
{
	long result;
	GetProperty(0x71, VT_I4, (void*)&result);
	return result;
}
void SetSelBlockCol2(long propVal)
{
	SetProperty(0x71, VT_I4, propVal);
}
long GetSelBlockRow()
{
	long result;
	GetProperty(0x72, VT_I4, (void*)&result);
	return result;
}
void SetSelBlockRow(long propVal)
{
	SetProperty(0x72, VT_I4, propVal);
}
long GetSelBlockRow2()
{
	long result;
	GetProperty(0x73, VT_I4, (void*)&result);
	return result;
}
void SetSelBlockRow2(long propVal)
{
	SetProperty(0x73, VT_I4, propVal);
}
short GetSelectBlockOptions()
{
	short result;
	GetProperty(0x74, VT_I2, (void*)&result);
	return result;
}
void SetSelectBlockOptions(short propVal)
{
	SetProperty(0x74, VT_I2, propVal);
}
long GetSelLength()
{
	long result;
	GetProperty(0x75, VT_I4, (void*)&result);
	return result;
}
void SetSelLength(long propVal)
{
	SetProperty(0x75, VT_I4, propVal);
}
long GetSelModeIndex()
{
	long result;
	GetProperty(0x76, VT_I4, (void*)&result);
	return result;
}
void SetSelModeIndex(long propVal)
{
	SetProperty(0x76, VT_I4, propVal);
}
long GetSelModeSelCount()
{
	long result;
	GetProperty(0x77, VT_I4, (void*)&result);
	return result;
}
void SetSelModeSelCount(long propVal)
{
	SetProperty(0x77, VT_I4, propVal);
}
BOOL GetSelModeSelected()
{
	BOOL result;
	GetProperty(0x78, VT_BOOL, (void*)&result);
	return result;
}
void SetSelModeSelected(BOOL propVal)
{
	SetProperty(0x78, VT_BOOL, propVal);
}
long GetSelStart()
{
	long result;
	GetProperty(0x79, VT_I4, (void*)&result);
	return result;
}
void SetSelStart(long propVal)
{
	SetProperty(0x79, VT_I4, propVal);
}
CString GetSelText()
{
	CString result;
	GetProperty(0x7a, VT_BSTR, (void*)&result);
	return result;
}
void SetSelText(CString propVal)
{
	SetProperty(0x7a, VT_BSTR, propVal);
}
unsigned long GetShadowColor()
{
	unsigned long result;
	GetProperty(0x7b, VT_UI4, (void*)&result);
	return result;
}
void SetShadowColor(unsigned long propVal)
{
	SetProperty(0x7b, VT_UI4, propVal);
}
unsigned long GetShadowDark()
{
	unsigned long result;
	GetProperty(0x7c, VT_UI4, (void*)&result);
	return result;
}
void SetShadowDark(unsigned long propVal)
{
	SetProperty(0x7c, VT_UI4, propVal);
}
unsigned long GetShadowText()
{
	unsigned long result;
	GetProperty(0x7d, VT_UI4, (void*)&result);
	return result;
}
void SetShadowText(unsigned long propVal)
{
	SetProperty(0x7d, VT_UI4, propVal);
}
long GetSortBy()
{
	long result;
	GetProperty(0x7e, VT_I4, (void*)&result);
	return result;
}
void SetSortBy(long propVal)
{
	SetProperty(0x7e, VT_I4, propVal);
}
long GetStartingColNumber()
{
	long result;
	GetProperty(0x7f, VT_I4, (void*)&result);
	return result;
}
void SetStartingColNumber(long propVal)
{
	SetProperty(0x7f, VT_I4, propVal);
}
long GetStartingRowNumber()
{
	long result;
	GetProperty(0x80, VT_I4, (void*)&result);
	return result;
}
void SetStartingRowNumber(long propVal)
{
	SetProperty(0x80, VT_I4, propVal);
}
long GetTopRow()
{
	long result;
	GetProperty(0x81, VT_I4, (void*)&result);
	return result;
}
void SetTopRow(long propVal)
{
	SetProperty(0x81, VT_I4, propVal);
}
long GetTypeButtonAlign()
{
	long result;
	GetProperty(0x82, VT_I4, (void*)&result);
	return result;
}
void SetTypeButtonAlign(long propVal)
{
	SetProperty(0x82, VT_I4, propVal);
}
unsigned long GetTypeButtonBorderColor()
{
	unsigned long result;
	GetProperty(0x83, VT_UI4, (void*)&result);
	return result;
}
void SetTypeButtonBorderColor(unsigned long propVal)
{
	SetProperty(0x83, VT_UI4, propVal);
}
unsigned long GetTypeButtonColor()
{
	unsigned long result;
	GetProperty(0x84, VT_UI4, (void*)&result);
	return result;
}
void SetTypeButtonColor(unsigned long propVal)
{
	SetProperty(0x84, VT_UI4, propVal);
}
unsigned long GetTypeButtonDarkColor()
{
	unsigned long result;
	GetProperty(0x85, VT_UI4, (void*)&result);
	return result;
}
void SetTypeButtonDarkColor(unsigned long propVal)
{
	SetProperty(0x85, VT_UI4, propVal);
}
unsigned long GetTypeButtonLightColor()
{
	unsigned long result;
	GetProperty(0x86, VT_UI4, (void*)&result);
	return result;
}
void SetTypeButtonLightColor(unsigned long propVal)
{
	SetProperty(0x86, VT_UI4, propVal);
}
CPicture GetTypeButtonPicture()
{
	LPDISPATCH result;
	GetProperty(0x87, VT_DISPATCH, (void*)&result);
	return CPicture(result);
}
void SetTypeButtonPicture(LPDISPATCH propVal)
{
	SetProperty(0x87, VT_DISPATCH, propVal);
}
CPicture GetTypeButtonPictureDown()
{
	LPDISPATCH result;
	GetProperty(0x88, VT_DISPATCH, (void*)&result);
	return CPicture(result);
}
void SetTypeButtonPictureDown(LPDISPATCH propVal)
{
	SetProperty(0x88, VT_DISPATCH, propVal);
}
short GetTypeButtonShadowSize()
{
	short result;
	GetProperty(0x89, VT_I2, (void*)&result);
	return result;
}
void SetTypeButtonShadowSize(short propVal)
{
	SetProperty(0x89, VT_I2, propVal);
}
CString GetTypeButtonText()
{
	CString result;
	GetProperty(0x8a, VT_BSTR, (void*)&result);
	return result;
}
void SetTypeButtonText(CString propVal)
{
	SetProperty(0x8a, VT_BSTR, propVal);
}
unsigned long GetTypeButtonTextColor()
{
	unsigned long result;
	GetProperty(0x8b, VT_UI4, (void*)&result);
	return result;
}
void SetTypeButtonTextColor(unsigned long propVal)
{
	SetProperty(0x8b, VT_UI4, propVal);
}
long GetTypeButtonType()
{
	long result;
	GetProperty(0x8c, VT_I4, (void*)&result);
	return result;
}
void SetTypeButtonType(long propVal)
{
	SetProperty(0x8c, VT_I4, propVal);
}
BOOL GetTypeCheckCenter()
{
	BOOL result;
	GetProperty(0x8d, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeCheckCenter(BOOL propVal)
{
	SetProperty(0x8d, VT_BOOL, propVal);
}
CString GetTypeCheckText()
{
	CString result;
	GetProperty(0x8e, VT_BSTR, (void*)&result);
	return result;
}
void SetTypeCheckText(CString propVal)
{
	SetProperty(0x8e, VT_BSTR, propVal);
}
long GetTypeCheckTextAlign()
{
	long result;
	GetProperty(0x8f, VT_I4, (void*)&result);
	return result;
}
void SetTypeCheckTextAlign(long propVal)
{
	SetProperty(0x8f, VT_I4, propVal);
}
short GetTypeComboBoxCount()
{
	short result;
	GetProperty(0x90, VT_I2, (void*)&result);
	return result;
}
void SetTypeComboBoxCount(short propVal)
{
	SetProperty(0x90, VT_I2, propVal);
}
short GetTypeComboBoxCurSel()
{
	short result;
	GetProperty(0x91, VT_I2, (void*)&result);
	return result;
}
void SetTypeComboBoxCurSel(short propVal)
{
	SetProperty(0x91, VT_I2, propVal);
}
BOOL GetTypeComboBoxEditable()
{
	BOOL result;
	GetProperty(0x92, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeComboBoxEditable(BOOL propVal)
{
	SetProperty(0x92, VT_BOOL, propVal);
}
short GetTypeComboBoxIndex()
{
	short result;
	GetProperty(0x93, VT_I2, (void*)&result);
	return result;
}
void SetTypeComboBoxIndex(short propVal)
{
	SetProperty(0x93, VT_I2, propVal);
}
CString GetTypeComboBoxList()
{
	CString result;
	GetProperty(0x94, VT_BSTR, (void*)&result);
	return result;
}
void SetTypeComboBoxList(CString propVal)
{
	SetProperty(0x94, VT_BSTR, propVal);
}
CString GetTypeComboBoxString()
{
	CString result;
	GetProperty(0x95, VT_BSTR, (void*)&result);
	return result;
}
void SetTypeComboBoxString(CString propVal)
{
	SetProperty(0x95, VT_BSTR, propVal);
}
BOOL GetTypeDateCentury()
{
	BOOL result;
	GetProperty(0x96, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeDateCentury(BOOL propVal)
{
	SetProperty(0x96, VT_BOOL, propVal);
}
long GetTypeDateFormat()
{
	long result;
	GetProperty(0x97, VT_I4, (void*)&result);
	return result;
}
void SetTypeDateFormat(long propVal)
{
	SetProperty(0x97, VT_I4, propVal);
}
CString GetTypeDateMax()
{
	CString result;
	GetProperty(0x98, VT_BSTR, (void*)&result);
	return result;
}
void SetTypeDateMax(CString propVal)
{
	SetProperty(0x98, VT_BSTR, propVal);
}
CString GetTypeDateMin()
{
	CString result;
	GetProperty(0x99, VT_BSTR, (void*)&result);
	return result;
}
void SetTypeDateMin(CString propVal)
{
	SetProperty(0x99, VT_BSTR, propVal);
}
short GetTypeDateSeparator()
{
	short result;
	GetProperty(0x9a, VT_I2, (void*)&result);
	return result;
}
void SetTypeDateSeparator(short propVal)
{
	SetProperty(0x9a, VT_I2, propVal);
}
long GetTypeEditCharCase()
{
	long result;
	GetProperty(0x9b, VT_I4, (void*)&result);
	return result;
}
void SetTypeEditCharCase(long propVal)
{
	SetProperty(0x9b, VT_I4, propVal);
}
long GetTypeEditCharSet()
{
	long result;
	GetProperty(0x9c, VT_I4, (void*)&result);
	return result;
}
void SetTypeEditCharSet(long propVal)
{
	SetProperty(0x9c, VT_I4, propVal);
}
long GetTypeEditLen()
{
	long result;
	GetProperty(0x9d, VT_I4, (void*)&result);
	return result;
}
void SetTypeEditLen(long propVal)
{
	SetProperty(0x9d, VT_I4, propVal);
}
BOOL GetTypeEditMultiLine()
{
	BOOL result;
	GetProperty(0x9e, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeEditMultiLine(BOOL propVal)
{
	SetProperty(0x9e, VT_BOOL, propVal);
}
BOOL GetTypeEditPassword()
{
	BOOL result;
	GetProperty(0x9f, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeEditPassword(BOOL propVal)
{
	SetProperty(0x9f, VT_BOOL, propVal);
}
short GetTypeFloatCurrencyChar()
{
	short result;
	GetProperty(0xa0, VT_I2, (void*)&result);
	return result;
}
void SetTypeFloatCurrencyChar(short propVal)
{
	SetProperty(0xa0, VT_I2, propVal);
}
short GetTypeFloatDecimalChar()
{
	short result;
	GetProperty(0xa1, VT_I2, (void*)&result);
	return result;
}
void SetTypeFloatDecimalChar(short propVal)
{
	SetProperty(0xa1, VT_I2, propVal);
}
short GetTypeFloatDecimalPlaces()
{
	short result;
	GetProperty(0xa2, VT_I2, (void*)&result);
	return result;
}
void SetTypeFloatDecimalPlaces(short propVal)
{
	SetProperty(0xa2, VT_I2, propVal);
}
double GetTypeFloatMax()
{
	double result;
	GetProperty(0xa3, VT_R8, (void*)&result);
	return result;
}
void SetTypeFloatMax(double propVal)
{
	SetProperty(0xa3, VT_R8, propVal);
}
double GetTypeFloatMin()
{
	double result;
	GetProperty(0xa4, VT_R8, (void*)&result);
	return result;
}
void SetTypeFloatMin(double propVal)
{
	SetProperty(0xa4, VT_R8, propVal);
}
BOOL GetTypeFloatMoney()
{
	BOOL result;
	GetProperty(0xa5, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeFloatMoney(BOOL propVal)
{
	SetProperty(0xa5, VT_BOOL, propVal);
}
BOOL GetTypeFloatSeparator()
{
	BOOL result;
	GetProperty(0xa6, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeFloatSeparator(BOOL propVal)
{
	SetProperty(0xa6, VT_BOOL, propVal);
}
short GetTypeFloatSepChar()
{
	short result;
	GetProperty(0xa7, VT_I2, (void*)&result);
	return result;
}
void SetTypeFloatSepChar(short propVal)
{
	SetProperty(0xa7, VT_I2, propVal);
}
long GetTypeHAlign()
{
	long result;
	GetProperty(0xa8, VT_I4, (void*)&result);
	return result;
}
void SetTypeHAlign(long propVal)
{
	SetProperty(0xa8, VT_I4, propVal);
}
long GetTypeIntegerMax()
{
	long result;
	GetProperty(0xa9, VT_I4, (void*)&result);
	return result;
}
void SetTypeIntegerMax(long propVal)
{
	SetProperty(0xa9, VT_I4, propVal);
}
long GetTypeIntegerMin()
{
	long result;
	GetProperty(0xaa, VT_I4, (void*)&result);
	return result;
}
void SetTypeIntegerMin(long propVal)
{
	SetProperty(0xaa, VT_I4, propVal);
}
long GetTypeIntegerSpinInc()
{
	long result;
	GetProperty(0xab, VT_I4, (void*)&result);
	return result;
}
void SetTypeIntegerSpinInc(long propVal)
{
	SetProperty(0xab, VT_I4, propVal);
}
BOOL GetTypeIntegerSpinWrap()
{
	BOOL result;
	GetProperty(0xac, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeIntegerSpinWrap(BOOL propVal)
{
	SetProperty(0xac, VT_BOOL, propVal);
}
long GetTypeOwnerDrawStyle()
{
	long result;
	GetProperty(0xad, VT_I4, (void*)&result);
	return result;
}
void SetTypeOwnerDrawStyle(long propVal)
{
	SetProperty(0xad, VT_I4, propVal);
}
CString GetTypePicDefaultText()
{
	CString result;
	GetProperty(0xae, VT_BSTR, (void*)&result);
	return result;
}
void SetTypePicDefaultText(CString propVal)
{
	SetProperty(0xae, VT_BSTR, propVal);
}
CString GetTypePicMask()
{
	CString result;
	GetProperty(0xaf, VT_BSTR, (void*)&result);
	return result;
}
void SetTypePicMask(CString propVal)
{
	SetProperty(0xaf, VT_BSTR, propVal);
}
BOOL GetTypePictCenter()
{
	BOOL result;
	GetProperty(0xb0, VT_BOOL, (void*)&result);
	return result;
}
void SetTypePictCenter(BOOL propVal)
{
	SetProperty(0xb0, VT_BOOL, propVal);
}
BOOL GetTypePictMaintainScale()
{
	BOOL result;
	GetProperty(0xb1, VT_BOOL, (void*)&result);
	return result;
}
void SetTypePictMaintainScale(BOOL propVal)
{
	SetProperty(0xb1, VT_BOOL, propVal);
}
CPicture GetTypePictPicture()
{
	LPDISPATCH result;
	GetProperty(0xb2, VT_DISPATCH, (void*)&result);
	return CPicture(result);
}
void SetTypePictPicture(LPDISPATCH propVal)
{
	SetProperty(0xb2, VT_DISPATCH, propVal);
}
BOOL GetTypePictStretch()
{
	BOOL result;
	GetProperty(0xb3, VT_BOOL, (void*)&result);
	return result;
}
void SetTypePictStretch(BOOL propVal)
{
	SetProperty(0xb3, VT_BOOL, propVal);
}
BOOL GetTypeSpin()
{
	BOOL result;
	GetProperty(0xb4, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeSpin(BOOL propVal)
{
	SetProperty(0xb4, VT_BOOL, propVal);
}
long GetTypeTextAlignVert()
{
	long result;
	GetProperty(0xb5, VT_I4, (void*)&result);
	return result;
}
void SetTypeTextAlignVert(long propVal)
{
	SetProperty(0xb5, VT_I4, propVal);
}
BOOL GetTypeTextPrefix()
{
	BOOL result;
	GetProperty(0xb6, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeTextPrefix(BOOL propVal)
{
	SetProperty(0xb6, VT_BOOL, propVal);
}
BOOL GetTypeTextShadow()
{
	BOOL result;
	GetProperty(0xb7, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeTextShadow(BOOL propVal)
{
	SetProperty(0xb7, VT_BOOL, propVal);
}
BOOL GetTypeTextShadowIn()
{
	BOOL result;
	GetProperty(0xb8, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeTextShadowIn(BOOL propVal)
{
	SetProperty(0xb8, VT_BOOL, propVal);
}
BOOL GetTypeTextWordWrap()
{
	BOOL result;
	GetProperty(0xb9, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeTextWordWrap(BOOL propVal)
{
	SetProperty(0xb9, VT_BOOL, propVal);
}
long GetTypeTime24Hour()
{
	long result;
	GetProperty(0xba, VT_I4, (void*)&result);
	return result;
}
void SetTypeTime24Hour(long propVal)
{
	SetProperty(0xba, VT_I4, propVal);
}
CString GetTypeTimeMax()
{
	CString result;
	GetProperty(0xbb, VT_BSTR, (void*)&result);
	return result;
}
void SetTypeTimeMax(CString propVal)
{
	SetProperty(0xbb, VT_BSTR, propVal);
}
CString GetTypeTimeMin()
{
	CString result;
	GetProperty(0xbc, VT_BSTR, (void*)&result);
	return result;
}
void SetTypeTimeMin(CString propVal)
{
	SetProperty(0xbc, VT_BSTR, propVal);
}
BOOL GetTypeTimeSeconds()
{
	BOOL result;
	GetProperty(0xbd, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeTimeSeconds(BOOL propVal)
{
	SetProperty(0xbd, VT_BOOL, propVal);
}
short GetTypeTimeSeparator()
{
	short result;
	GetProperty(0xbe, VT_I2, (void*)&result);
	return result;
}
void SetTypeTimeSeparator(short propVal)
{
	SetProperty(0xbe, VT_I2, propVal);
}
long GetUnitType()
{
	long result;
	GetProperty(0xbf, VT_I4, (void*)&result);
	return result;
}
void SetUnitType(long propVal)
{
	SetProperty(0xbf, VT_I4, propVal);
}
long GetUserResize()
{
	long result;
	GetProperty(0xc0, VT_I4, (void*)&result);
	return result;
}
void SetUserResize(long propVal)
{
	SetProperty(0xc0, VT_I4, propVal);
}
long GetUserResizeCol()
{
	long result;
	GetProperty(0xc1, VT_I4, (void*)&result);
	return result;
}
void SetUserResizeCol(long propVal)
{
	SetProperty(0xc1, VT_I4, propVal);
}
long GetUserResizeRow()
{
	long result;
	GetProperty(0xc2, VT_I4, (void*)&result);
	return result;
}
void SetUserResizeRow(long propVal)
{
	SetProperty(0xc2, VT_I4, propVal);
}
CString GetValue()
{
	CString result;
	GetProperty(0xc3, VT_BSTR, (void*)&result);
	return result;
}
void SetValue(CString propVal)
{
	SetProperty(0xc3, VT_BSTR, propVal);
}
long GetVirtualCurRowCount()
{
	long result;
	GetProperty(0xc4, VT_I4, (void*)&result);
	return result;
}
void SetVirtualCurRowCount(long propVal)
{
	SetProperty(0xc4, VT_I4, propVal);
}
long GetVirtualCurTop()
{
	long result;
	GetProperty(0xc5, VT_I4, (void*)&result);
	return result;
}
void SetVirtualCurTop(long propVal)
{
	SetProperty(0xc5, VT_I4, propVal);
}
long GetVirtualMaxRows()
{
	long result;
	GetProperty(0xc6, VT_I4, (void*)&result);
	return result;
}
void SetVirtualMaxRows(long propVal)
{
	SetProperty(0xc6, VT_I4, propVal);
}
BOOL GetVirtualMode()
{
	BOOL result;
	GetProperty(0xc7, VT_BOOL, (void*)&result);
	return result;
}
void SetVirtualMode(BOOL propVal)
{
	SetProperty(0xc7, VT_BOOL, propVal);
}
long GetVirtualOverlap()
{
	long result;
	GetProperty(0xc8, VT_I4, (void*)&result);
	return result;
}
void SetVirtualOverlap(long propVal)
{
	SetProperty(0xc8, VT_I4, propVal);
}
long GetVirtualRows()
{
	long result;
	GetProperty(0xc9, VT_I4, (void*)&result);
	return result;
}
void SetVirtualRows(long propVal)
{
	SetProperty(0xc9, VT_I4, propVal);
}
BOOL GetVirtualScrollBuffer()
{
	BOOL result;
	GetProperty(0xca, VT_BOOL, (void*)&result);
	return result;
}
void SetVirtualScrollBuffer(BOOL propVal)
{
	SetProperty(0xca, VT_BOOL, propVal);
}
long GetVisibleCols()
{
	long result;
	GetProperty(0xcb, VT_I4, (void*)&result);
	return result;
}
void SetVisibleCols(long propVal)
{
	SetProperty(0xcb, VT_I4, propVal);
}
long GetVisibleRows()
{
	long result;
	GetProperty(0xcc, VT_I4, (void*)&result);
	return result;
}
void SetVisibleRows(long propVal)
{
	SetProperty(0xcc, VT_I4, propVal);
}
BOOL GetVScrollSpecial()
{
	BOOL result;
	GetProperty(0xcd, VT_BOOL, (void*)&result);
	return result;
}
void SetVScrollSpecial(BOOL propVal)
{
	SetProperty(0xcd, VT_BOOL, propVal);
}
short GetVScrollSpecialType()
{
	short result;
	GetProperty(0xce, VT_I2, (void*)&result);
	return result;
}
void SetVScrollSpecialType(short propVal)
{
	SetProperty(0xce, VT_I2, propVal);
}
BOOL GetFontBold()
{
	BOOL result;
	GetProperty(0xcf, VT_BOOL, (void*)&result);
	return result;
}
void SetFontBold(BOOL propVal)
{
	SetProperty(0xcf, VT_BOOL, propVal);
}
BOOL GetFontItalic()
{
	BOOL result;
	GetProperty(0xd0, VT_BOOL, (void*)&result);
	return result;
}
void SetFontItalic(BOOL propVal)
{
	SetProperty(0xd0, VT_BOOL, propVal);
}
CString GetFontName()
{
	CString result;
	GetProperty(0xd1, VT_BSTR, (void*)&result);
	return result;
}
void SetFontName(CString propVal)
{
	SetProperty(0xd1, VT_BSTR, propVal);
}
float GetFontSize()
{
	float result;
	GetProperty(0xd2, VT_R4, (void*)&result);
	return result;
}
void SetFontSize(float propVal)
{
	SetProperty(0xd2, VT_R4, propVal);
}
BOOL GetFontStrikethru()
{
	BOOL result;
	GetProperty(0xd3, VT_BOOL, (void*)&result);
	return result;
}
void SetFontStrikethru(BOOL propVal)
{
	SetProperty(0xd3, VT_BOOL, propVal);
}
BOOL GetFontUnderline()
{
	BOOL result;
	GetProperty(0xd4, VT_BOOL, (void*)&result);
	return result;
}
void SetFontUnderline(BOOL propVal)
{
	SetProperty(0xd4, VT_BOOL, propVal);
}
CPicture GetCursorIcon()
{
	LPDISPATCH result;
	GetProperty(0xd5, VT_DISPATCH, (void*)&result);
	return CPicture(result);
}
void SetCursorIcon(LPDISPATCH propVal)
{
	SetProperty(0xd5, VT_DISPATCH, propVal);
}
long GetPrintOrientation()
{
	long result;
	GetProperty(0xd6, VT_I4, (void*)&result);
	return result;
}
void SetPrintOrientation(long propVal)
{
	SetProperty(0xd6, VT_I4, propVal);
}
long GetBackColorStyle()
{
	long result;
	GetProperty(0xd7, VT_I4, (void*)&result);
	return result;
}
void SetBackColorStyle(long propVal)
{
	SetProperty(0xd7, VT_I4, propVal);
}
BOOL GetFormulaSync()
{
	BOOL result;
	GetProperty(0xd8, VT_BOOL, (void*)&result);
	return result;
}
void SetFormulaSync(BOOL propVal)
{
	SetProperty(0xd8, VT_BOOL, propVal);
}
long GetTypeCheckType()
{
	long result;
	GetProperty(0x110, VT_I4, (void*)&result);
	return result;
}
void SetTypeCheckType(long propVal)
{
	SetProperty(0x110, VT_I4, propVal);
}
long GetAppearance()
{
	long result;
	GetProperty(0x111, VT_I4, (void*)&result);
	return result;
}
void SetAppearance(long propVal)
{
	SetProperty(0x111, VT_I4, propVal);
}
short GetTypeComboBoxMaxDrop()
{
	short result;
	GetProperty(0x112, VT_I2, (void*)&result);
	return result;
}
void SetTypeComboBoxMaxDrop(short propVal)
{
	SetProperty(0x112, VT_I2, propVal);
}
short GetTypeMaxEditLen()
{
	short result;
	GetProperty(0x113, VT_I2, (void*)&result);
	return result;
}
void SetTypeMaxEditLen(short propVal)
{
	SetProperty(0x113, VT_I2, propVal);
}
short GetTypeComboBoxWidth()
{
	short result;
	GetProperty(0x114, VT_I2, (void*)&result);
	return result;
}
void SetTypeComboBoxWidth(short propVal)
{
	SetProperty(0x114, VT_I2, propVal);
}
long GetTextTip()
{
	long result;
	GetProperty(0x115, VT_I4, (void*)&result);
	return result;
}
void SetTextTip(long propVal)
{
	SetProperty(0x115, VT_I4, propVal);
}
long GetTextTipDelay()
{
	long result;
	GetProperty(0x116, VT_I4, (void*)&result);
	return result;
}
void SetTextTipDelay(long propVal)
{
	SetProperty(0x116, VT_I4, propVal);
}
long GetScrollBarTrack()
{
	long result;
	GetProperty(0x117, VT_I4, (void*)&result);
	return result;
}
void SetScrollBarTrack(long propVal)
{
	SetProperty(0x117, VT_I4, propVal);
}
long GetTypeVAlign()
{
	long result;
	GetProperty(0x118, VT_I4, (void*)&result);
	return result;
}
void SetTypeVAlign(long propVal)
{
	SetProperty(0x118, VT_I4, propVal);
}
short GetClipboardOptions()
{
	short result;
	GetProperty(0x119, VT_I2, (void*)&result);
	return result;
}
void SetClipboardOptions(short propVal)
{
	SetProperty(0x119, VT_I2, propVal);
}
BOOL GetPrintSmartPrint()
{
	BOOL result;
	GetProperty(0x11a, VT_BOOL, (void*)&result);
	return result;
}
void SetPrintSmartPrint(BOOL propVal)
{
	SetProperty(0x11a, VT_BOOL, propVal);
}
long GetPrintPageOrder()
{
	long result;
	GetProperty(0x11b, VT_I4, (void*)&result);
	return result;
}
void SetPrintPageOrder(long propVal)
{
	SetProperty(0x11b, VT_I4, propVal);
}
long GetPrintPageCount()
{
	long result;
	GetProperty(0x11c, VT_I4, (void*)&result);
	return result;
}
void SetPrintPageCount(long propVal)
{
	SetProperty(0x11c, VT_I4, propVal);
}
long GetPrintFirstPageNumber()
{
	long result;
	GetProperty(0x11d, VT_I4, (void*)&result);
	return result;
}
void SetPrintFirstPageNumber(long propVal)
{
	SetProperty(0x11d, VT_I4, propVal);
}
long GetPrintNextPageBreakCol()
{
	long result;
	GetProperty(0x11e, VT_I4, (void*)&result);
	return result;
}
void SetPrintNextPageBreakCol(long propVal)
{
	SetProperty(0x11e, VT_I4, propVal);
}
long GetPrintNextPageBreakRow()
{
	long result;
	GetProperty(0x11f, VT_I4, (void*)&result);
	return result;
}
void SetPrintNextPageBreakRow(long propVal)
{
	SetProperty(0x11f, VT_I4, propVal);
}
long GetTypeComboBoxhWnd()
{
	long result;
	GetProperty(0x120, VT_I4, (void*)&result);
	return result;
}
void SetTypeComboBoxhWnd(long propVal)
{
	SetProperty(0x120, VT_I4, propVal);
}
unsigned long GetSelBackColor()
{
	unsigned long result;
	GetProperty(0x121, VT_UI4, (void*)&result);
	return result;
}
void SetSelBackColor(unsigned long propVal)
{
	SetProperty(0x121, VT_UI4, propVal);
}
unsigned long GetSelForeColor()
{
	unsigned long result;
	GetProperty(0x122, VT_UI4, (void*)&result);
	return result;
}
void SetSelForeColor(unsigned long propVal)
{
	SetProperty(0x122, VT_UI4, propVal);
}
short GetTwoDigitYearMax()
{
	short result;
	GetProperty(0x123, VT_I2, (void*)&result);
	return result;
}
void SetTwoDigitYearMax(short propVal)
{
	SetProperty(0x123, VT_I2, propVal);
}
long GetOLEDropMode()
{
	long result;
	GetProperty(0x13b, VT_I4, (void*)&result);
	return result;
}
void SetOLEDropMode(long propVal)
{
	SetProperty(0x13b, VT_I4, propVal);
}
CString GetCellNote()
{
	CString result;
	GetProperty(0x13e, VT_BSTR, (void*)&result);
	return result;
}
void SetCellNote(CString propVal)
{
	SetProperty(0x13e, VT_BSTR, propVal);
}
long GetUserColAction()
{
	long result;
	GetProperty(0x13f, VT_I4, (void*)&result);
	return result;
}
void SetUserColAction(long propVal)
{
	SetProperty(0x13f, VT_I4, propVal);
}
long GetShowScrollTips()
{
	long result;
	GetProperty(0x140, VT_I4, (void*)&result);
	return result;
}
void SetShowScrollTips(long propVal)
{
	SetProperty(0x140, VT_I4, propVal);
}
long GetCellNoteIndicator()
{
	long result;
	GetProperty(0x141, VT_I4, (void*)&result);
	return result;
}
void SetCellNoteIndicator(long propVal)
{
	SetProperty(0x141, VT_I4, propVal);
}
BOOL GetTypeEllipses()
{
	BOOL result;
	GetProperty(0x14a, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeEllipses(BOOL propVal)
{
	SetProperty(0x14a, VT_BOOL, propVal);
}
BOOL GetScriptEnhanced()
{
	BOOL result;
	GetProperty(0x150, VT_BOOL, (void*)&result);
	return result;
}
void SetScriptEnhanced(BOOL propVal)
{
	SetProperty(0x150, VT_BOOL, propVal);
}
long GetColMerge()
{
	long result;
	GetProperty(0x151, VT_I4, (void*)&result);
	return result;
}
void SetColMerge(long propVal)
{
	SetProperty(0x151, VT_I4, propVal);
}
long GetRowMerge()
{
	long result;
	GetProperty(0x152, VT_I4, (void*)&result);
	return result;
}
void SetRowMerge(long propVal)
{
	SetProperty(0x152, VT_I4, propVal);
}
CString GetColID()
{
	CString result;
	GetProperty(0x153, VT_BSTR, (void*)&result);
	return result;
}
void SetColID(CString propVal)
{
	SetProperty(0x153, VT_BSTR, propVal);
}
long GetRowHeaderCols()
{
	long result;
	GetProperty(0x154, VT_I4, (void*)&result);
	return result;
}
void SetRowHeaderCols(long propVal)
{
	SetProperty(0x154, VT_I4, propVal);
}
long GetRowHeadersAutoTextIndex()
{
	long result;
	GetProperty(0x155, VT_I4, (void*)&result);
	return result;
}
void SetRowHeadersAutoTextIndex(long propVal)
{
	SetProperty(0x155, VT_I4, propVal);
}
long GetColHeaderRows()
{
	long result;
	GetProperty(0x156, VT_I4, (void*)&result);
	return result;
}
void SetColHeaderRows(long propVal)
{
	SetProperty(0x156, VT_I4, propVal);
}
long GetColHeadersAutoTextIndex()
{
	long result;
	GetProperty(0x157, VT_I4, (void*)&result);
	return result;
}
void SetColHeadersAutoTextIndex(long propVal)
{
	SetProperty(0x157, VT_I4, propVal);
}
long GetColHeadersUserSortIndex()
{
	long result;
	GetProperty(0x158, VT_I4, (void*)&result);
	return result;
}
void SetColHeadersUserSortIndex(long propVal)
{
	SetProperty(0x158, VT_I4, propVal);
}
long GetTypeComboBoxAutoSearch()
{
	long result;
	GetProperty(0x159, VT_I4, (void*)&result);
	return result;
}
void SetTypeComboBoxAutoSearch(long propVal)
{
	SetProperty(0x159, VT_I4, propVal);
}
double GetTypeSpinInc()
{
	double result;
	GetProperty(0x15a, VT_R8, (void*)&result);
	return result;
}
void SetTypeSpinInc(double propVal)
{
	SetProperty(0x15a, VT_R8, propVal);
}
BOOL GetTypeSpinWrap()
{
	BOOL result;
	GetProperty(0x15b, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeSpinWrap(BOOL propVal)
{
	SetProperty(0x15b, VT_BOOL, propVal);
}
BOOL GetTypeNegRed()
{
	BOOL result;
	GetProperty(0x15c, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeNegRed(BOOL propVal)
{
	SetProperty(0x15c, VT_BOOL, propVal);
}
double GetTypeCurrencyMin()
{
	double result;
	GetProperty(0x15d, VT_R8, (void*)&result);
	return result;
}
void SetTypeCurrencyMin(double propVal)
{
	SetProperty(0x15d, VT_R8, propVal);
}
double GetTypeCurrencyMax()
{
	double result;
	GetProperty(0x15e, VT_R8, (void*)&result);
	return result;
}
void SetTypeCurrencyMax(double propVal)
{
	SetProperty(0x15e, VT_R8, propVal);
}
CString GetTypeCurrencyDecimal()
{
	CString result;
	GetProperty(0x15f, VT_BSTR, (void*)&result);
	return result;
}
void SetTypeCurrencyDecimal(CString propVal)
{
	SetProperty(0x15f, VT_BSTR, propVal);
}
CString GetTypeCurrencySeparator()
{
	CString result;
	GetProperty(0x160, VT_BSTR, (void*)&result);
	return result;
}
void SetTypeCurrencySeparator(CString propVal)
{
	SetProperty(0x160, VT_BSTR, propVal);
}
CString GetTypeCurrencySymbol()
{
	CString result;
	GetProperty(0x161, VT_BSTR, (void*)&result);
	return result;
}
void SetTypeCurrencySymbol(CString propVal)
{
	SetProperty(0x161, VT_BSTR, propVal);
}
long GetTypeCurrencyLeadingZero()
{
	long result;
	GetProperty(0x162, VT_I4, (void*)&result);
	return result;
}
void SetTypeCurrencyLeadingZero(long propVal)
{
	SetProperty(0x162, VT_I4, propVal);
}
short GetTypeCurrencyDecPlaces()
{
	short result;
	GetProperty(0x163, VT_I2, (void*)&result);
	return result;
}
void SetTypeCurrencyDecPlaces(short propVal)
{
	SetProperty(0x163, VT_I2, propVal);
}
long GetTypeCurrencyNegStyle()
{
	long result;
	GetProperty(0x164, VT_I4, (void*)&result);
	return result;
}
void SetTypeCurrencyNegStyle(long propVal)
{
	SetProperty(0x164, VT_I4, propVal);
}
long GetTypeCurrencyPosStyle()
{
	long result;
	GetProperty(0x165, VT_I4, (void*)&result);
	return result;
}
void SetTypeCurrencyPosStyle(long propVal)
{
	SetProperty(0x165, VT_I4, propVal);
}
BOOL GetTypeCurrencyShowSep()
{
	BOOL result;
	GetProperty(0x166, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeCurrencyShowSep(BOOL propVal)
{
	SetProperty(0x166, VT_BOOL, propVal);
}
BOOL GetTypeCurrencyShowSymbol()
{
	BOOL result;
	GetProperty(0x167, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeCurrencyShowSymbol(BOOL propVal)
{
	SetProperty(0x167, VT_BOOL, propVal);
}
double GetTypeNumberMin()
{
	double result;
	GetProperty(0x168, VT_R8, (void*)&result);
	return result;
}
void SetTypeNumberMin(double propVal)
{
	SetProperty(0x168, VT_R8, propVal);
}
double GetTypeNumberMax()
{
	double result;
	GetProperty(0x169, VT_R8, (void*)&result);
	return result;
}
void SetTypeNumberMax(double propVal)
{
	SetProperty(0x169, VT_R8, propVal);
}
CString GetTypeNumberDecimal()
{
	CString result;
	GetProperty(0x16a, VT_BSTR, (void*)&result);
	return result;
}
void SetTypeNumberDecimal(CString propVal)
{
	SetProperty(0x16a, VT_BSTR, propVal);
}
CString GetTypeNumberSeparator()
{
	CString result;
	GetProperty(0x16b, VT_BSTR, (void*)&result);
	return result;
}
void SetTypeNumberSeparator(CString propVal)
{
	SetProperty(0x16b, VT_BSTR, propVal);
}
long GetTypeNumberLeadingZero()
{
	long result;
	GetProperty(0x16c, VT_I4, (void*)&result);
	return result;
}
void SetTypeNumberLeadingZero(long propVal)
{
	SetProperty(0x16c, VT_I4, propVal);
}
short GetTypeNumberDecPlaces()
{
	short result;
	GetProperty(0x16d, VT_I2, (void*)&result);
	return result;
}
void SetTypeNumberDecPlaces(short propVal)
{
	SetProperty(0x16d, VT_I2, propVal);
}
long GetTypeNumberNegStyle()
{
	long result;
	GetProperty(0x16e, VT_I4, (void*)&result);
	return result;
}
void SetTypeNumberNegStyle(long propVal)
{
	SetProperty(0x16e, VT_I4, propVal);
}
BOOL GetTypeNumberShowSep()
{
	BOOL result;
	GetProperty(0x16f, VT_BOOL, (void*)&result);
	return result;
}
void SetTypeNumberShowSep(BOOL propVal)
{
	SetProperty(0x16f, VT_BOOL, propVal);
}
double GetTypePercentMin()
{
	double result;
	GetProperty(0x170, VT_R8, (void*)&result);
	return result;
}
void SetTypePercentMin(double propVal)
{
	SetProperty(0x170, VT_R8, propVal);
}
double GetTypePercentMax()
{
	double result;
	GetProperty(0x171, VT_R8, (void*)&result);
	return result;
}
void SetTypePercentMax(double propVal)
{
	SetProperty(0x171, VT_R8, propVal);
}
CString GetTypePercentDecimal()
{
	CString result;
	GetProperty(0x172, VT_BSTR, (void*)&result);
	return result;
}
void SetTypePercentDecimal(CString propVal)
{
	SetProperty(0x172, VT_BSTR, propVal);
}
short GetTypePercentDecPlaces()
{
	short result;
	GetProperty(0x173, VT_I2, (void*)&result);
	return result;
}
void SetTypePercentDecPlaces(short propVal)
{
	SetProperty(0x173, VT_I2, propVal);
}
long GetTypePercentNegStyle()
{
	long result;
	GetProperty(0x174, VT_I4, (void*)&result);
	return result;
}
void SetTypePercentNegStyle(long propVal)
{
	SetProperty(0x174, VT_I4, propVal);
}
long GetTypeTextOrient()
{
	long result;
	GetProperty(0x175, VT_I4, (void*)&result);
	return result;
}
void SetTypeTextOrient(long propVal)
{
	SetProperty(0x175, VT_I4, propVal);
}
CString GetCellTag()
{
	CString result;
	GetProperty(0x176, VT_BSTR, (void*)&result);
	return result;
}
void SetCellTag(CString propVal)
{
	SetProperty(0x176, VT_BSTR, propVal);
}
BOOL GetAllowEditOverflow()
{
	BOOL result;
	GetProperty(0x177, VT_BOOL, (void*)&result);
	return result;
}
void SetAllowEditOverflow(BOOL propVal)
{
	SetProperty(0x177, VT_BOOL, propVal);
}
long GetColHeadersAutoText()
{
	long result;
	GetProperty(0x178, VT_I4, (void*)&result);
	return result;
}
void SetColHeadersAutoText(long propVal)
{
	SetProperty(0x178, VT_I4, propVal);
}
BOOL GetColHeadersShow()
{
	BOOL result;
	GetProperty(0x179, VT_BOOL, (void*)&result);
	return result;
}
void SetColHeadersShow(BOOL propVal)
{
	SetProperty(0x179, VT_BOOL, propVal);
}
long GetRowHeadersAutoText()
{
	long result;
	GetProperty(0x17a, VT_I4, (void*)&result);
	return result;
}
void SetRowHeadersAutoText(long propVal)
{
	SetProperty(0x17a, VT_I4, propVal);
}
BOOL GetRowHeadersShow()
{
	BOOL result;
	GetProperty(0x17b, VT_BOOL, (void*)&result);
	return result;
}
void SetRowHeadersShow(BOOL propVal)
{
	SetProperty(0x17b, VT_BOOL, propVal);
}
long GetTypePercentLeadingZero()
{
	long result;
	GetProperty(0x17c, VT_I4, (void*)&result);
	return result;
}
void SetTypePercentLeadingZero(long propVal)
{
	SetProperty(0x17c, VT_I4, propVal);
}
long GetSelectionCount()
{
	long result;
	GetProperty(0x1e9, VT_I4, (void*)&result);
	return result;
}
void SetSelectionCount(long propVal)
{
	SetProperty(0x1e9, VT_I4, propVal);
}
unsigned long GetBackColor()
{
	unsigned long result;
	GetProperty(DISPID_BACKCOLOR, VT_UI4, (void*)&result);
	return result;
}
void SetBackColor(unsigned long propVal)
{
	SetProperty(DISPID_BACKCOLOR, VT_UI4, propVal);
}
long GetBorderStyle()
{
	long result;
	GetProperty(DISPID_BORDERSTYLE, VT_I4, (void*)&result);
	return result;
}
void SetBorderStyle(long propVal)
{
	SetProperty(DISPID_BORDERSTYLE, VT_I4, propVal);
}
BOOL GetEnabled()
{
	BOOL result;
	GetProperty(DISPID_ENABLED, VT_BOOL, (void*)&result);
	return result;
}
void SetEnabled(BOOL propVal)
{
	SetProperty(DISPID_ENABLED, VT_BOOL, propVal);
}
COleFont GetFont()
{
	LPDISPATCH result;
	GetProperty(DISPID_FONT, VT_DISPATCH, (void*)&result);
	return COleFont(result);
}
void SetFont(LPDISPATCH propVal)
{
	SetProperty(DISPID_FONT, VT_DISPATCH, propVal);
}
unsigned long GetForeColor()
{
	unsigned long result;
	GetProperty(DISPID_FORECOLOR, VT_UI4, (void*)&result);
	return result;
}
void SetForeColor(unsigned long propVal)
{
	SetProperty(DISPID_FORECOLOR, VT_UI4, propVal);
}
OLE_HANDLE GethWnd()
{
	OLE_HANDLE result;
	GetProperty(DISPID_HWND, VT_I4, (void*)&result);
	return result;
}
void SethWnd(OLE_HANDLE propVal)
{
	SetProperty(DISPID_HWND, VT_I4, propVal);
}
CString GetText()
{
	CString result;
	GetProperty(DISPID_TEXT, VT_BSTR, (void*)&result);
	return result;
}
void SetText(CString propVal)
{
	SetProperty(DISPID_TEXT, VT_BSTR, propVal);
}


};

#include "zxmgr.h"
#include "zxmgr_gui.h"
#include "Image.h"
#include "utils.h"
#include "config.h"

#include "menulib.h"

static MMainMenu* BaseWidget = NULL;
static Image* img_MenuBG = NULL;
static Image* img_MenuLogo = NULL;

static MTextField* m_Login = NULL;
static MTextField* m_Password = NULL;

static MRadioList* m_RadioGamemode = NULL;

static MEnterButton* m_Submit = NULL;

static MCheckBox* m_CheckRemember = NULL;

static MIconButton* m_Authors = NULL;
static MIconButton* m_Exit = NULL;

static byte* MenuCurrent = NULL;

static int MenuCursor = 0;

static void SetCString(const char** cstr, const char* text)
{
	const char* ocstr = NULL;
	__asm
	{
		// create cstring
		push	text
		lea		ecx, ocstr
		mov		edx, 0x005AB8B8 // CString::CString(const char*)
		call	edx
		
		// assign
		lea		ecx, ocstr
		push	ecx
		mov		ecx, cstr
		mov		edx, 0x005AB991 // CString::operator=
		call	edx

		// delete cstring
		lea		ecx, ocstr
		mov		edx, 0x005AB84A // CString::~CString
		call	edx
	}
}

void _stdcall MENU_close(int message)
{
	__asm
	{
		mov		ecx, MenuCurrent
		mov		edx, [ecx]
		call	[edx+0x34]
		//0x488

		// send regular message
		push	0
		push	0
		push	message
		mov		edx, 0x00401870
		call	edx
		mov		ecx, eax
		mov		edx, 0x00420960
		call	edx

		// close menu
		// ?

		// send magic ROM2 message
		push	message
		mov		ecx, MenuCurrent
		mov		edx, [ecx]
		call	[edx+0x84]
	}

	MenuCursor = 0;
}

static std::string EncodeDecodeAuth(std::string s)
{
	for (int i = 0; i < s.length(); i++)
		s[i] = (i-100) ^ s[i];
	return s;
}

void _stdcall MENU_Connect()
{
	// do init stuff
	// send message 1160 ? 0x416 + 114 in order to start connecting
	unsigned long mwnd = zxmgr::AfxGetMainWnd();
	*(unsigned long*)(mwnd + 0x3C8) = m_RadioGamemode->GetSelection(); // game type
	/*
	// 0. Classic Softcore // was 2
	// 1. Classic Hardcore	 // new
	// 2. Arena          // was 1*/
	switch (m_RadioGamemode->GetSelection())
	{
	case 1:
		*(unsigned long*)(mwnd + 0x3C8) = 2;
		break;
	case 0:
		*(unsigned long*)(mwnd + 0x3C8) = 4;
		break;
	case 2:
		*(unsigned long*)(mwnd + 0x3C8) = 3;
		break;
	}

	SetCString((const char**)(mwnd + 0x3C0), m_Login->GetText().c_str());
	SetCString((const char**)(mwnd + 0x3C4), m_Password->GetText().c_str());
	SetCString((const char**)(mwnd + 0x3B8), "hat.allods2.eu");
	*(unsigned long*)(mwnd + 0x3D0) = m_CheckRemember->IsChecked()?1:0;
	*(unsigned long*)(mwnd + 0x3F4) = 1;
	
	// close menu, start connecting
	MENU_close(1160);
}

bool MMainMenu::OnChildMessage(MWidget* sender, int id, unsigned long d1, unsigned long d2)
{
	if (sender == m_Submit)
	{
		MENU_Connect();
		return true;
	}
	else if (sender == m_Exit)
	{
		MENU_close(16);
		return true;
	}
	else if (sender == m_Authors)
	{
		MENU_close(1064);
		return true;
	}

	return false;
}

void _stdcall DoMenuLoad(byte* menu)
{
	MenuCurrent = menu;

	if (!BaseWidget)
	{
		const char* locale = z_russian ? "ru" : "en";
		img_MenuBG = new Image(Format("data/locale/%s/graphics/mainmenu/menunew.png", locale));
		img_MenuLogo = new Image(Format("data/locale/%s/graphics/mainmenu/menulogo.png", locale));

		BaseWidget = new MMainMenu();
		MRect rec = M_GetDisplayRect();
		MRect menurec(rec.GetWidth() / 2 - 320, rec.GetHeight() / 2 - 240,
					  rec.GetWidth() / 2 + 320, rec.GetHeight() / 2 + 240);
		BaseWidget->SetRect(menurec);

		// create children
		MLabel* lab = new MLabel(BaseWidget);
		lab->SetAlign(M_ALIGN_CENTER);
		lab->SetRect(MRect(0, 221, 640, 221+16));
		lab->SetText(zxmgr::GetPatchString(241));
		lab->SetBold(true);

		m_Login = new MTextField(BaseWidget);
		m_Login->SetRect(MRect::FromXYWH(230, 133, 180, 33));
		m_Login->SetPlaceholder(zxmgr::GetPatchString(110));

		m_Password = new MTextField(BaseWidget);
		m_Password->SetRect(MRect::FromXYWH(230, 173, 180, 33));
		m_Password->SetPlaceholder(zxmgr::GetPatchString(111));
		m_Password->SetMasked(true);

		m_RadioGamemode = new MRadioList(BaseWidget);
		m_RadioGamemode->SetRect(MRect::FromXYWH(264, 248, 108, 80));
		m_RadioGamemode->AddOption(zxmgr::GetPatchString(237)); // classic hardcore
		m_RadioGamemode->AddOption(zxmgr::GetPatchString(213)); // classic softcore
		m_RadioGamemode->AddOption(zxmgr::GetPatchString(109)); // arena
		m_RadioGamemode->SetSelection(0);

		m_Submit = new MEnterButton(BaseWidget);
		m_Submit->SetRect(MRect::FromXYWH(257, 345, 125, 48));
		m_Submit->SetText(zxmgr::GetPatchString(238));

		m_CheckRemember = new MCheckBox(BaseWidget);
		m_CheckRemember->SetRect(MRect::FromXYWH(266, 414, 96, 15));
		m_CheckRemember->SetText(zxmgr::GetPatchString(242));
		m_CheckRemember->SetChecked(true);

		m_Exit = new MIconButton(BaseWidget);
		m_Exit->SetRect(MRect::FromXYWH(529, 430, 88, 28));
		m_Exit->SetText(zxmgr::GetPatchString(240));
		m_Exit->SetIcon(new Image("data/locale/ru/graphics/mainmenu/mb_exit.png"),
						new Image("data/locale/ru/graphics/mainmenu/mb_exit_on.png"),
						new Image("data/locale/ru/graphics/mainmenu/mb_exit_off.png"));

		m_Authors = new MIconButton(BaseWidget);
		m_Authors->SetRect(MRect::FromXYWH(24, 430, 88, 28));
		m_Authors->SetText(zxmgr::GetPatchString(239));
		m_Authors->SetIcon(new Image("data/locale/ru/graphics/mainmenu/mb_authors.png"),
						   new Image("data/locale/ru/graphics/mainmenu/mb_authors_on.png"),
						   new Image("data/locale/ru/graphics/mainmenu/mb_authors_off.png"));
		//m_Authors->SetEnabled(false);

		// get current login and password, if this is the first run
		HKEY hKey;
		if (!RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\1C\\Allods2.eu", 0, 0x20019, &hKey))
		{
			char tmpstr[1024];
			int csz = 1024;
			
			csz = 1024;
			if(!RegQueryValueExA(hKey, "login", 0, 0, (LPBYTE)&tmpstr, (LPDWORD)&csz))
				m_Login->SetText(EncodeDecodeAuth(tmpstr));

			csz = 1024;
			if (!RegQueryValueExA(hKey, "password", 0, 0, (LPBYTE)&tmpstr, (LPDWORD)&csz));
				m_Password->SetText(EncodeDecodeAuth(tmpstr));

			int cdm;
			csz = 4;
			if (!RegQueryValueExA(hKey, "deathmatch", 0, 0, (LPBYTE)&cdm, (LPDWORD)&csz))
			{
				switch (cdm)
				{
				case 2:
					m_RadioGamemode->SetSelection(1);
					break;
				case 3:
					m_RadioGamemode->SetSelection(2);
					break;
				case 4:
					m_RadioGamemode->SetSelection(0);
					break;
				}
			}

			csz = 4;
			if (!RegQueryValueExA(hKey, "store", 0, 0, (LPBYTE)&cdm, (LPDWORD)&csz))
			{
				m_CheckRemember->SetChecked(!!cdm);
			}

			RegCloseKey(hKey);
		}
	}

	// init stuff
	m_Login->SetFocused(true);
}

void _stdcall DoMenuWMDirect(int msg, int wparam, int lparam);
void _stdcall DoMenuDraw()
{
	// check shift

	if (MenuCursor++ < 3)
	{
		// set cursor
		__asm
		{
			mov		ecx, 0x00630E8C
			mov		ecx, [ecx]
			mov		edx, 0x00480EDF
			call	edx
		}
	}

	//if (!MenuVisible) return;
	static bool shiftwaspressed = false;
	static bool ctrlwaspressed = false;
	static bool altwaspressed = false;

	if (shiftwaspressed && !M_IsShiftDown())
		DoMenuWMDirect(WM_KEYUP, VK_SHIFT, 0);
	else if (!shiftwaspressed && M_IsShiftDown())
		DoMenuWMDirect(WM_KEYDOWN, VK_SHIFT, 0);
	shiftwaspressed = M_IsShiftDown();

	if (ctrlwaspressed && !M_IsCtrlDown())
		DoMenuWMDirect(WM_KEYUP, VK_CONTROL, 0);
	else if (!ctrlwaspressed && M_IsCtrlDown())
		DoMenuWMDirect(WM_KEYDOWN, VK_CONTROL, 0);
	ctrlwaspressed = M_IsCtrlDown();

	if (altwaspressed && !M_IsAltDown())
		DoMenuWMDirect(WM_KEYUP, VK_MENU, 0);
	else if (!altwaspressed && M_IsAltDown())
		DoMenuWMDirect(WM_KEYDOWN, VK_MENU, 0);
	altwaspressed = M_IsAltDown();

	MRect MenuRect = BaseWidget->GetAbsoluteRect();

	zxmgr::LockBuffer();
	img_MenuBG->Display(MenuRect.Left, MenuRect.Top);
	 // 163, 21
	img_MenuLogo->Display(MenuRect.Left+163, MenuRect.Top+21);
	BaseWidget->Display();
	zxmgr::UnlockBuffer();
}

int _stdcall DoMenuClick(int button)
{
	log_format("click\n");
	return 0;
}

#define MOUSEEVENT_DOWN 0
#define MOUSEEVENT_UP 1
#define MOUSEEVENT_MOVE 2
void _stdcall DoMenuHover(byte* menu, int x, int y, unsigned int buttons, int action)
{
	//if (!MenuVisible) return;
	if (action == MOUSEEVENT_DOWN)
		BaseWidget->OnMouseDown(x, y, buttons);
	else if (action == MOUSEEVENT_UP)
		BaseWidget->OnMouseUp(x, y, buttons);
	else BaseWidget->OnMouseMove(x, y);
	// put data to menu (most notably +F0)
	// 0 = 5
	// 1 = 3
	// 2 = 7
	/*
	if (buttonclick == buttonhover)
	{
		switch (buttonclick)
		{
		case 0:
			*(uint32_t*)(menu + 0xF0) = 5;
			break;

		case 1:
			*(uint32_t*)(menu + 0xF0) = 3;
			break;

		case 2:
			*(uint32_t*)(menu + 0xF0) = 7;
			break;

		default:
			*(uint32_t*)(menu + 0xF0) = -1;
			break;
		}
	}
	else
	{
		*(uint32_t*)(menu + 0xF0) = -1;
	}*/
}

void _stdcall DoMenuWMDirect(int msg, int wparam, int lparam)
{
	//if (!MenuVisible) return;
	static unsigned long lastdown = 0;
	static unsigned long lastchr = 0;

	//log_format("WM: %06X %d %d\n", msg, wparam, lparam);
	// WM_KEYDOWN = 100
	// WM_KEYUP = 101
	// WM_CHAR = 102
	if (msg == WM_KEYDOWN)
	{
		if (GetTickCount()-lastdown > 25)
		{
			if (wparam == VK_RETURN) // handle return globally
			{
				if (!BaseWidget->OnKeyDown(wparam))
					MENU_Connect();
			}
			else BaseWidget->OnKeyDown(wparam);
			lastdown = GetTickCount();
		}
	}
	else if (msg == WM_KEYUP)
	{
		BaseWidget->OnKeyUp(wparam);
		lastdown = 0;
		lastchr = 0;
	}
	else if (msg == WM_CHAR)
	{
		if (GetTickCount()-lastchr > 25)
		{
			BaseWidget->OnTextEntered(wparam); // unicode
			lastchr = GetTickCount();
		}
	}
}

void __declspec(naked) MENU_load()
{
	// @ 4AEDF5
	__asm
	{
		push	[ebp-0x4C]
		call	DoMenuLoad
		/*lea		ecx, [ebp-0x10]
		mov		edx, 0x005AB6FF
		call	edx
		mov		edx, 0x004AEDFD
		jmp		edx*/
		mov		edx, 0x004AF09D
		jmp		edx
	}
}

void __declspec(naked) MENU_draw()
{
	// @ 4AF2C8
	__asm
	{
		call	DoMenuDraw

		mov		edx, 0x004AF43D
		jmp		edx
	}
}

void __declspec(naked) MENU_click()
{
	// @ 4AF93F
	__asm
	{
		cmp		[ebp-0x0C], 7
		ja		loc_4AFA34 // old handling
		mov		edx, 0x004AF949
		jmp		edx

		push	[ebp-0x0C]
		call	DoMenuClick
		mov		[ebp-0x04], eax

		mov		edx, 0x004AFA34
		jmp		edx

loc_4AFA34:
		mov		edx, 0x4AFA34
		jmp		edx
	}
}

void __declspec(naked) MENU_hover1()
{
	// @ 4AF8E6
	__asm
	{
		push	MOUSEEVENT_DOWN
		push	[ebp+0x08]
		push	[ebp+0x10]
		push	[ebp+0x0C]
		push	[ebp-0x04]
		call	DoMenuHover

		mov		edx, 0x004AF918
		jmp		edx
	}
}

void __declspec(naked) MENU_hover2()
{
	// @ 4AF8C3
	__asm
	{
		push	MOUSEEVENT_MOVE
		push	[ebp+0x08]
		push	[ebp+0x10]
		push	[ebp+0x0C]
		push	[ebp-0x04]
		call	DoMenuHover

		mov		edx, 0x004AF8D7
		jmp		edx
	}
}

void __declspec(naked) MENU_hover3()
{
	// @ 4AFA5B
	__asm
	{
		push	MOUSEEVENT_UP
		push	[ebp+0x08]
		push	[ebp+0x10]
		push	[ebp+0x0C]
		push	[ebp-0x08]
		call	DoMenuHover

		mov		edx, 0x004AFA6F
		jmp		edx
	}
}

void __declspec(naked) MENU_wmdirect()
{
	// @ 4AF457
	__asm
	{
		// msg, wparam, lparam
		push	[ebp+0x10]
		push	[ebp+0x0C]
		push	[ebp+0x08]
		call	DoMenuWMDirect

		mov		eax, [ebp+0x08]
		mov		[ebp-0x08], eax
		cmp		eax, 0x402
		mov		edx, 0x004AF464
		jmp		edx
	}
}

void __declspec(naked) MENU_noauth()
{
	// 48A969
	__asm
	{
		/*mov		edx, [ebp-0x3F4]
		mov		[ebp-0x2A0], edx
		mov		byte ptr [ebp-0x04], 0
		mov		eax, [ebp-0x2A0]
		push	eax
		mov		ecx, [ebp-0x3DC]
		mov		edx, 0x004913EB
		call	edx*/
		// and cancel immediately
		// note: ignore signal 1159 (0x487)
		push	ebp
		mov		ebp, esp

		push	0xFFFFFFFF
		push	0x005C972B
		mov		eax, fs:[0]
		push	eax
		mov		fs:[0], esp
		sub		esp, 0x488
		mov		[ebp-0x3DC], ecx
		
		cmp		[ebp+0x08], 0x487
		jz		noauth

		mov		edx, 0x0048A98D
		jmp		edx

noauth:
		/*push	0
		push	0
		push	0x442
		mov		ecx, [ebp-0x3DC]
		mov		edx, 0x00420960
		call	edx*/

		//call	zxmgr::DoMessageLoop

		push	MenuCurrent
		mov		ecx, [ebp-0x3DC]
		mov		edx, 0x004913EB
		call	edx
		
		call	zxmgr::DoMessageLoop

		mov		esp, ebp
		pop		ebp
		retn	0x000C
	}
}
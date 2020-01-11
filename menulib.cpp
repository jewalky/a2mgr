#include "menulib.h"
#include <windows.h>
#include <algorithm> // for min/max
#undef min
#undef max

#include "utils.h"
#include "Image.h"
#include "zxmgr.h"

// for text
#include <SDL/SDL_ttf.h>
static TTF_Font* MFont5VeryBig = NULL;
static TTF_Font* MFont5Big = NULL;
static TTF_Font* MFont5 = NULL;
static TTF_Font* MFont5Small = NULL;
static TTF_Font* MFont5VerySmall = NULL;

static TTF_Font* M_GetVeryBigFont()
{
	if (!MFont5VeryBig)
		MFont5VeryBig = TTF_OpenFont("data/graphics/font5/font5.ttf", 14);
	TTF_SetFontHinting(MFont5VeryBig, TTF_HINTING_LIGHT);
	return MFont5VeryBig;
}

static TTF_Font* M_GetBigFont()
{
	if (!MFont5Big)
		MFont5Big = TTF_OpenFont("data/graphics/font5/font5.ttf", 13);
	TTF_SetFontHinting(MFont5Big, TTF_HINTING_LIGHT);
	return MFont5Big;
}

static TTF_Font* M_GetFont()
{
	if (!MFont5)
		MFont5 = TTF_OpenFont("data/graphics/font5/font5.ttf", 14);
	TTF_SetFontHinting(MFont5, TTF_HINTING_LIGHT);
	return MFont5;
}

static TTF_Font* M_GetSmallFont()
{
	if (!MFont5Small)
		MFont5Small = TTF_OpenFont("data/graphics/font5/font5.ttf", 12);
	TTF_SetFontHinting(MFont5Small, TTF_HINTING_LIGHT);
	return MFont5Small;
}

static TTF_Font* M_GetVerySmallFont()
{
	if (!MFont5VerySmall)
		MFont5VerySmall = TTF_OpenFont("data/graphics/font5/font5.ttf", 10);
	TTF_SetFontHinting(MFont5VerySmall, TTF_HINTING_LIGHT);
	return MFont5VerySmall;
}

MRect M_GetDisplayRect()
{
	MRect outr;
	RECT pRect = *(RECT*)(0x00625768);
	outr.Left = pRect.left;
	outr.Right = pRect.right;
	outr.Top = pRect.top;
	outr.Bottom = pRect.bottom;
	return outr.Normalized();
}

void MLabel::Display()
{
	MRect absRect = GetAbsoluteRect();
	// draw label
	if (m_Align == M_ALIGN_RIGHT)
		m_TextImage->Display(absRect.Right-m_TextImage->GetWidth(), absRect.Top);
	else if (m_Align == M_ALIGN_CENTER)
		m_TextImage->Display(absRect.Left+absRect.GetWidth()/2-m_TextImage->GetWidth()/2, absRect.Top);
	else m_TextImage->Display(absRect.Left, absRect.Top);
	//log_format("displaying label!\n");
}

void MLabel::UpdateImage()
{
	if (m_TextImage) delete m_TextImage;
	int flagsc = TTF_GetFontStyle(M_GetBigFont());
	int flagsn = 0;
	if (m_Bold) flagsn |= TTF_STYLE_BOLD;
	if (m_Italic) flagsn |= TTF_STYLE_ITALIC;
	TTF_SetFontStyle(M_GetBigFont(), flagsn);
	m_TextImage = Image::RenderText(M_GetBigFont(), m_Text, 255, 255, 255);
	TTF_SetFontStyle(M_GetBigFont(), flagsc);
}

static Image* img_MTextFieldBGUnfocused = NULL;
static Image* img_MTextFieldBGFocused = NULL;
MTextField::MTextField(MWidget* parent) : MWidget(parent)
{
	m_PlaceholderImage = NULL;
	m_TextImage = NULL;

	m_Hovered = false;

	if (!img_MTextFieldBGUnfocused)
		img_MTextFieldBGUnfocused = new Image("data/locale/ru/graphics/mainmenu/mfield_unfocused.png");
	if (!img_MTextFieldBGFocused)
		img_MTextFieldBGFocused = new Image("data/locale/ru/graphics/mainmenu/mfield_focused.png");

	m_LastBlinkTime = GetTickCount();
	m_Selection1 = 0;
	m_Selection2 = 0;

	m_SelectingK = m_SelectingM = false;
	m_Masked = false;
}

void MTextField::Display()
{
	MRect absRect = GetAbsoluteRect();
	if (m_Hovered || IsFocused())
		img_MTextFieldBGFocused->Display(absRect.Left, absRect.Top);
	else img_MTextFieldBGUnfocused->Display(absRect.Left, absRect.Top);

	std::string mtext = GetMaskedText();

	// now if this field is unfocused AND empty, display placeholder
	if (!IsFocused() && !mtext.length() && m_PlaceholderImage)
	{
		m_PlaceholderImage->Display(absRect.Left+absRect.GetWidth()/2-m_PlaceholderImage->GetWidth()/2,
									absRect.Top+absRect.GetHeight()/2-m_PlaceholderImage->GetHeight()/2+1);
	}

	int mtextx = absRect.Left+absRect.GetWidth()/2;
	int mtexty = absRect.Top+absRect.GetHeight()/2-13/2;
	int cselection1 = Image::RenderTextWidth(m_Masked?M_GetVeryBigFont():M_GetSmallFont(), mtext.substr(0, m_Selection1));
	int cselection2 = Image::RenderTextWidth(m_Masked?M_GetVeryBigFont():M_GetSmallFont(), mtext.substr(0, m_Selection2));

	if (m_Text.length())
	{
		mtextx -= m_TextImage->GetWidth()/2;

		// draw selection if it's large
		if (cselection1 != cselection2)
		{
			int rcselection1 = std::min(cselection1, cselection2);
			int rcselection2 = std::max(cselection1, cselection2);

			zxmgr::FillRect(mtextx+rcselection1, mtexty-1, mtextx+rcselection2, mtexty+13, 0);
		}

		m_TextImage->Display(mtextx,
							 mtexty+1);
	}

	// display the cursor (m_Selection2)
	if (IsFocused() && ((GetTickCount()-m_LastBlinkTime) % 800) < 400)
	{
		zxmgr::DrawLine(mtextx+cselection2, mtexty, mtextx+cselection2, mtexty+13, 0xFFFFFFFF);
	}
}

bool MTextField::OnMouseDown(int x, int y, int button)
{
	if (button != 1) return false;
	MRect absRect = GetAbsoluteRect();
	if (!absRect.Contains(x, y))
		return false;
	
	SetFocused(true); // click on this element
	m_SelectingK = false;
	m_SelectingM = true;
	m_Selection2 = GetSelectionByXY(x, y);
	m_Selection1 = m_Selection2;
	return true;
}

int MTextField::GetSelectionByXY(int x, int y)
{
	MRect absRect = GetAbsoluteRect();

	int mtextx = absRect.Left+absRect.GetWidth()/2;

	if (m_Text.length())
		mtextx -= m_TextImage->GetWidth()/2;

	// do selection
	std::string mtext = GetMaskedText();
	if (x < mtextx) return 0;
	for (int i = 1; i <= m_Text.length(); i++)
	{
		int msp = mtextx+Image::RenderTextWidth(m_Masked?M_GetVeryBigFont():M_GetSmallFont(), mtext.substr(0, i-1));
		int msc = mtextx+Image::RenderTextWidth(m_Masked?M_GetVeryBigFont():M_GetSmallFont(), mtext.substr(0, i));
		if (x >= msp && x < msc)
		{
			int center = (msp+msc)/2;
			if (x >= center) return i;
			return i-1;
		}
	}

	return m_Text.length();
}

bool MTextField::OnMouseUp(int x, int y, int button)
{
	m_SelectingM = false;
	return false;
}

bool MTextField::OnMouseMove(int x, int y)
{
	MRect absRect = GetAbsoluteRect();
	m_Hovered = absRect.Contains(x, y);

	// if selecting with mouse, move selection2.
	if (m_SelectingM)
	{
		m_Selection2 = GetSelectionByXY(x, y);
		return true;
	}

	return false;
}


bool MTextField::OnKeyDown(int vk)
{
	if (!IsFocused())
		return false;
	if (vk == VK_LEFT)
	{
		if (m_Selection2 > 0)
			m_Selection2--;
		if (!m_SelectingK)
			m_Selection1 = m_Selection2;
		m_LastBlinkTime = GetTickCount();
		return true;
	}
	else if (vk == VK_RIGHT)
	{
		if (m_Selection2 < m_Text.length())
			m_Selection2++;
		if (!m_SelectingK)
			m_Selection1 = m_Selection2;
		m_LastBlinkTime = GetTickCount();
		return true;
	}
	else if (vk == VK_HOME)
	{
		m_Selection2 = 0;
		if (!m_SelectingK)
			m_Selection1 = m_Selection2;
		m_LastBlinkTime = GetTickCount();
		return true;
	}
	else if (vk == VK_END)
	{
		m_Selection2 = m_Text.length();
		if (!m_SelectingK)
			m_Selection1 = m_Selection2;
		m_LastBlinkTime = GetTickCount();
		return true;
	}
	else if (vk == VK_SHIFT)
	{
		m_SelectingK = true;
		return true;
	}
	else if (vk == VK_BACK || vk == VK_DELETE)
	{
		if (m_Selection2 != m_Selection1)
		{
			int rselection1 = std::min(m_Selection1, m_Selection2);
			int rselection2 = std::max(m_Selection1, m_Selection2);
			SetText(m_Text.substr(0, rselection1)+m_Text.substr(rselection2));
			m_Selection2 = rselection1;
			m_Selection1 = m_Selection2;
		}
		else if (vk == VK_BACK)
		{
			if (m_Selection2 > 0)
			{
				SetText(m_Text.substr(0, m_Selection2-1)+m_Text.substr(m_Selection2));
				m_Selection2--;
				m_Selection1 = m_Selection2;
			}
		}
		else
		{
			if (m_Selection2 < m_Text.length())
			{
				SetText(m_Text.substr(0, m_Selection2)+m_Text.substr(m_Selection2+1));
			}
		}
		m_LastBlinkTime = GetTickCount();
		return true;
	}

	return false;
}

bool MTextField::OnKeyUp(int vk)
{
	if (!IsFocused())
		return false;
	if (vk == VK_SHIFT)
	{
		m_SelectingK = false;
	}
	return true;
}

bool MTextField::OnTextEntered(int unichar)
{
	if (!IsFocused())
		return false;
	// make cp866 from unichar and put it to text
	char ch = unichar;
	//log_format("unichar = %04X\n", unichar);
	/*if (unichar >= 0x410 && unichar <= 0x43F)
		ch = unichar-0x390;
	else if (unichar >= 0x440 && unichar <= 0x44F)
		ch = unichar-0x360;
	else if (unichar == 0x401)
		ch = 0xF0;
	else if (unichar == 0x451)
		ch = 0xF1;
	else if (unichar > 0x7F || unichar < 0x20)
		return true;*/
	// fun fact: is NOT unicode, instead windows-1251, despite what the docs say
	if (unichar >= 0xF0 && unichar <= 0xFF)
		ch = unichar-0x10;
	else if (unichar >= 0xC0 && unichar <= 0xEF)
		ch = unichar-0x40;
	else if (unichar == 0xA8)
		ch = 0xF0;
	else if (unichar == 0xB8)
		ch = 0xF1;
	else if (unichar > 0x7F || unichar < 0x20)
		return true;
	// predict size. if size is larger than 145 pixels, dont accept char
	int rselection1 = std::min(m_Selection1, m_Selection2);
	int rselection2 = std::max(m_Selection1, m_Selection2);
	std::string newtext = m_Text.substr(0, rselection1)+m_Text.substr(rselection2);
	//newtext += ch;
	newtext.insert(rselection1, std::string()+ch);
	if (Image::RenderTextWidth(m_Masked?M_GetVeryBigFont():M_GetSmallFont(), newtext) > 145)
		return true; // skip
	SetText(newtext);
	m_Selection2 = rselection1+1;
	m_Selection1 = m_Selection2;
	m_LastBlinkTime = GetTickCount();
	return true;
}

void MTextField::UpdatePlaceholder()
{
	if (m_PlaceholderImage) delete m_PlaceholderImage;
	m_PlaceholderImage = Image::RenderText(M_GetVerySmallFont(), m_Placeholder, 155, 148, 121);
}

void MTextField::UpdateText()
{
	if (m_TextImage) delete m_TextImage;
	m_TextImage = Image::RenderText(m_Masked?M_GetVeryBigFont():M_GetSmallFont(), GetMaskedText(), 255, 255, 255);
}

static Image* img_MRadioListOn = NULL;
static Image* img_MRadioListOff = NULL;
static Image* img_MRadioListOnOff = NULL;
MRadioList::MRadioList(MWidget* parent) : MWidget(parent)
{
	m_Selection = -1;
	m_Hovered = m_Clicked = -1;

	if (!img_MRadioListOn) img_MRadioListOn = new Image("data/locale/ru/graphics/mainmenu/radio_on.png");
	if (!img_MRadioListOff) img_MRadioListOff = new Image("data/locale/ru/graphics/mainmenu/radio_off.png");
	if (!img_MRadioListOnOff) img_MRadioListOnOff = new Image("data/locale/ru/graphics/mainmenu/radio_onoff.png");
}

MRadioList::~MRadioList()
{
	for (int i = 0; i < m_Options.size(); i++)
	{
		delete m_Options[i];
		delete m_OptionsHovered[i];
	}
	
	m_Options.clear();
	m_OptionsHovered.clear();
}

void MRadioList::AddOption(std::string text)
{
	int flagsc = TTF_GetFontStyle(M_GetVerySmallFont());
	int flagsn = TTF_STYLE_BOLD;
	TTF_SetFontStyle(M_GetVerySmallFont(), flagsn);
	Image* optionimg = Image::RenderText(M_GetVerySmallFont(), text, 218, 202, 136); // unhovered
	Image* optionimghovered = Image::RenderText(M_GetVerySmallFont(), text, 255, 255, 255); // unhovered
	TTF_SetFontStyle(M_GetVerySmallFont(), flagsc);
	m_Options.push_back(optionimg);
	m_OptionsHovered.push_back(optionimghovered);
}

void MRadioList::Display()
{
	MRect absRect = GetAbsoluteRect();

	for (int i = 0; i < m_Options.size(); i++)
	{
		if (m_Hovered == i || (IsFocused() && m_Selection == i))
			m_OptionsHovered[i]->Display(absRect.Left+18, absRect.Top+i*20+3);
		else m_Options[i]->Display(absRect.Left+18, absRect.Top+i*20+3);
		if (m_Clicked >= 0 && (i == m_Selection || i == m_Clicked))
			img_MRadioListOnOff->Display(absRect.Left, absRect.Top+i*20-1);
		else if (i == m_Selection)
			img_MRadioListOn->Display(absRect.Left, absRect.Top+i*20-1);
		else img_MRadioListOff->Display(absRect.Left, absRect.Top+i*20-1);
	}
}

bool MRadioList::OnMouseDown(int x, int y, int button)
{
	if (button != 1) return false;
	MRect absRect = GetAbsoluteRect();
	if (!absRect.Contains(x, y))
		return false;

	SetFocused(true);
	if (m_Hovered >= 0)
		m_Clicked = m_Hovered;
	return true;
}

bool MRadioList::OnMouseUp(int x, int y, int button)
{
	if (m_Clicked >= 0)
	{
		m_Selection = m_Clicked;
		m_Clicked = -1;
	}
	return false;
}

bool MRadioList::OnMouseMove(int x, int y)
{
	m_Hovered = -1;

	MRect absRect = GetAbsoluteRect();
	if (!absRect.Contains(x, y))
		return false;

	int iny = (y-absRect.Top) / 20;
	if (iny < m_Options.size())
		m_Hovered = iny;
	return false;
}

bool MRadioList::OnKeyDown(int vk)
{
	if (!IsFocused())
		return false;

	if (vk == VK_DOWN)
	{
		if (m_Selection < 0)
			m_Selection = 0;
		else if (m_Selection < m_Options.size()-1)
			m_Selection++;
		return true;
	}
	else if (vk == VK_UP)
	{
		if (m_Selection < 0)
			m_Selection = m_Options.size()-1;
		else if (m_Selection > 0)
			m_Selection--;
		return true;
	}
	else if (vk == VK_HOME)
	{
		m_Selection = 0;
		return true;
	}
	else if (vk == VK_END)
	{
		m_Selection = m_Options.size()-1;
		return true;
	}

	return false;
}

Image* img_MEnterButtonOff = NULL;
Image* img_MEnterButtonOn = NULL;
MEnterButton::MEnterButton(MWidget *parent) : MWidget(parent)
{
	m_TextImage = NULL;
	m_TextImageHovered = NULL;

	m_Clicked = false;
	m_Msg = 0;

	if (!img_MEnterButtonOff) img_MEnterButtonOff = new Image("data/locale/ru/graphics/mainmenu/enter_off.png");
	if (!img_MEnterButtonOn) img_MEnterButtonOn = new Image("data/locale/ru/graphics/mainmenu/enter_on.png");
}

void MEnterButton::UpdateText()
{
	if (m_TextImage) delete m_TextImage;
	if (m_TextImageHovered) delete m_TextImageHovered;
	int flagsc = TTF_GetFontStyle(M_GetFont());
	int flagsn = TTF_STYLE_BOLD;
	TTF_SetFontStyle(M_GetFont(), flagsn);
	m_TextImage = Image::RenderText(M_GetFont(), m_Text, 218, 202, 136); // unhovered
	m_TextImageHovered = Image::RenderText(M_GetFont(), m_Text, 255, 255, 255); // unhovered
	TTF_SetFontStyle(M_GetFont(), flagsc);
}

void MEnterButton::Display()
{
	MRect absRect = GetAbsoluteRect();

	// draw image
	if (!m_Clicked)
		img_MEnterButtonOff->Display(absRect.Left, absRect.Top);
	else img_MEnterButtonOn->Display(absRect.Left, absRect.Top);

	if (m_Hovered || IsFocused() || m_Clicked)
		m_TextImageHovered->Display(absRect.Left+absRect.GetWidth()/2-m_TextImageHovered->GetWidth()/2,
									absRect.Top+absRect.GetHeight()/2-m_TextImageHovered->GetHeight()/2+(m_Clicked?1:0));
	else m_TextImage->Display(absRect.Left+absRect.GetWidth()/2-m_TextImage->GetWidth()/2,
							  absRect.Top+absRect.GetHeight()/2-m_TextImage->GetHeight()/2);
}

bool MEnterButton::OnMouseDown(int x, int y, int button)
{
	if (!m_Enabled) return false;
	if (button != 1) return false;
	MRect absRect = GetAbsoluteRect();
	if (!absRect.Contains(x, y))
		return false;

	m_Clicked = true;
	return true;
}

bool MEnterButton::OnMouseUp(int x, int y, int button)
{
	if (!m_Enabled) return false;
	if (!m_Clicked) return false;
	m_Clicked = false;
	if (!m_Hovered) return false;
	SendChildMessage(m_Msg, 0, 0);
	return false;
}

bool MEnterButton::OnMouseMove(int x, int y)
{
	m_Hovered = false;
	if (!m_Enabled) return false;

	MRect absRect = GetAbsoluteRect();
	if (!absRect.Contains(x, y))
		return false;

	m_Hovered = true;
	return false;
}

bool MEnterButton::OnKeyDown(int vk)
{
	if (!m_Enabled) return false;
	if (!IsFocused()) return false;

	if (vk == VK_RETURN)
	{
		SendChildMessage(m_Msg, 0, 0);
		return true;
	}
	else if (vk == VK_SPACE)
	{
		m_Clicked = true;
		return true;
	}
	else if (vk == VK_ESCAPE)
	{
		m_Clicked = false;
		return true;
	}

	return false;
}

bool MEnterButton::OnKeyUp(int vk)
{
	if (!m_Enabled) return false;

	if (vk == VK_SPACE && m_Clicked)
	{
		m_Clicked = false;
		SendChildMessage(m_Msg, 0, 0);
	}

	return false;
}

Image* img_MCheckOff = NULL;
Image* img_MCheckOn = NULL;
Image* img_MCheckOnOff = NULL;
MCheckBox::MCheckBox(MWidget *parent) : MWidget(parent)
{
	m_TextImage = NULL;
	m_TextImageHovered = NULL;

	m_Clicked = false;

	if (!img_MCheckOff) img_MCheckOff = new Image("data/locale/ru/graphics/mainmenu/check_off.png");
	if (!img_MCheckOn) img_MCheckOn = new Image("data/locale/ru/graphics/mainmenu/check_on.png");
	if (!img_MCheckOnOff) img_MCheckOnOff = new Image("data/locale/ru/graphics/mainmenu/check_onoff.png");
}

void MCheckBox::UpdateText()
{
	if (m_TextImage) delete m_TextImage;
	if (m_TextImageHovered) delete m_TextImageHovered;
	int flagsc = TTF_GetFontStyle(M_GetVerySmallFont());
	int flagsn = TTF_STYLE_BOLD;
	TTF_SetFontStyle(M_GetVerySmallFont(), flagsn);
	m_TextImage = Image::RenderText(M_GetVerySmallFont(), m_Text, 218, 202, 136); // unhovered
	m_TextImageHovered = Image::RenderText(M_GetVerySmallFont(), m_Text, 255, 255, 255); // unhovered
	TTF_SetFontStyle(M_GetVerySmallFont(), flagsc);
}

void MCheckBox::Display()
{
	MRect absRect = GetAbsoluteRect();

	// draw image
	if (m_Clicked)
		img_MCheckOnOff->Display(absRect.Left, absRect.Top);
	else if (m_Checked)
		img_MCheckOn->Display(absRect.Left, absRect.Top);
	else img_MCheckOff->Display(absRect.Left, absRect.Top);

	if (m_Hovered || IsFocused() || m_Clicked)
		m_TextImageHovered->Display(absRect.Left+20, absRect.Top+4);
	else m_TextImage->Display(absRect.Left+20, absRect.Top+4);
}

bool MCheckBox::OnMouseDown(int x, int y, int button)
{
	if (button != 1) return false;
	MRect absRect = GetAbsoluteRect();
	if (!absRect.Contains(x, y))
		return false;

	m_Clicked = true;
	return true;
}

bool MCheckBox::OnMouseUp(int x, int y, int button)
{
	if (!m_Clicked) return false;
	m_Clicked = false;
	if (!m_Hovered) return false;
	m_Checked = !m_Checked;
	return false;
}

bool MCheckBox::OnMouseMove(int x, int y)
{
	m_Hovered = false;

	MRect absRect = GetAbsoluteRect();
	if (!absRect.Contains(x, y))
		return false;

	m_Hovered = true;
	return false;
}

bool MCheckBox::OnKeyDown(int vk)
{
	if (!IsFocused()) return false;

	if (vk == VK_RETURN)
	{
		m_Checked = !m_Checked;
		return true;
	}
	else if (vk == VK_SPACE)
	{
		m_Clicked = true;
		return true;
	}
	else if (vk == VK_ESCAPE)
	{
		m_Clicked = false;
		return true;
	}

	return false;
}

bool MCheckBox::OnKeyUp(int vk)
{
	if (vk == VK_SPACE && m_Clicked)
	{
		m_Clicked = false;
		m_Checked = !m_Checked;
	}

	return false;
}

void MIconButton::Display()
{
	MRect absRect = GetAbsoluteRect();

	if (m_Hovered || IsFocused() || m_Clicked)
	{
		if (m_HoveredIcon)
			m_HoveredIcon->Display(absRect.Left, absRect.Top+(m_Clicked?1:0));
		else if (m_Icon)
			m_Icon->Display(absRect.Left, absRect.Top+(m_Clicked?1:0));
		m_TextImageHovered->Display(absRect.Left+31,
									absRect.Top+absRect.GetHeight()/2-m_TextImageHovered->GetHeight()/2+(m_Clicked?1:0));
	}
	else
	{
		if (!IsEnabled() && m_OffIcon)
			m_OffIcon->Display(absRect.Left, absRect.Top);
		else if (m_Icon)
			m_Icon->Display(absRect.Left, absRect.Top);
		m_TextImage->Display(absRect.Left+31,
							 absRect.Top+absRect.GetHeight()/2-m_TextImage->GetHeight()/2);
	}
}

void MIconButton::UpdateText()
{
	if (m_TextImage) delete m_TextImage;
	if (m_TextImageHovered) delete m_TextImageHovered;
	int flagsc = TTF_GetFontStyle(M_GetSmallFont());
	int flagsn = 0;//TTF_STYLE_BOLD;
	TTF_SetFontStyle(M_GetSmallFont(), flagsn);
	if (m_Enabled)
		m_TextImage = Image::RenderText(M_GetSmallFont(), m_Text, 218, 202, 136); // unhovered
	else m_TextImage = Image::RenderText(M_GetSmallFont(), m_Text, 132, 132, 132); // unhovered, disabled
	m_TextImageHovered = Image::RenderText(M_GetSmallFont(), m_Text, 255, 255, 255); // hovered
	TTF_SetFontStyle(M_GetSmallFont(), flagsc);
}
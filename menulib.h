#pragma once

#include <vector>
#include <string>
#include "Image.h"

// widgets
inline bool M_IsShiftDown() { return !!*(unsigned long*)(0x0062C9A4); }
inline bool M_IsCtrlDown() { return !!*(unsigned long*)(0x0062C9A8); }
inline bool M_IsAltDown() { return !!*(unsigned long*)(0x0062C9A0); }


#define M_ALIGN_LEFT 0
#define M_ALIGN_RIGHT 1
#define M_ALIGN_CENTER 2

struct MRect
{
	int Left;
	int Top;
	int Right;
	int Bottom;

	MRect()
	{
		Left = Top = Right = Bottom = 0;
	}

	static MRect FromXYWH(int x, int y, int w, int h)
	{
		MRect rec;
		rec.Left = x;
		rec.Top = y;
		rec.Right = x+w;
		rec.Bottom = y+h;
		return rec;
	}

	MRect(int left, int top, int right, int bottom)
	{
		Left = left;
		Right = right;
		Top = top;
		Bottom = bottom;
	}

	MRect Normalized()
	{
		MRect outr = *this;
		if (Right < Left)
		{
			outr.Left = Right;
			outr.Right = Left;
		}

		if (Bottom < Top)
		{
			outr.Top = Bottom;
			outr.Bottom = Top;
		}

		return outr;
	}

	int GetWidth()
	{
		return (Left < Right) ? Right-Left : Left-Right;
	}

	int GetHeight()
	{
		return (Top < Bottom) ? Bottom-Top : Top-Bottom;
	}

	bool Contains(int x, int y)
	{
		MRect norm = Normalized();
		return (x >= norm.Left && x < norm.Right && y >= norm.Top && y < norm.Bottom);
	}
};

// global stuff
MRect M_GetDisplayRect();

class MWidget
{
public:
	MWidget(MWidget* parent)
	{
		m_Focused = false;
		m_Enabled = true;
		m_Parent = parent;
		if (m_Parent) m_Parent->m_Children.push_back(this);
	}
	
	virtual ~MWidget()
	{
		for (int i = 0; i < m_Children.size(); i++)
			delete m_Children[i];
		m_Children.clear();
	}

	virtual void Display()
	{
		for (int i = 0; i < m_Children.size(); i++)
			m_Children[i]->Display();
	}

	virtual bool OnKeyDown(int vk)
	{
		if (vk == 0x09) // VK_TAB, not including windows.h here
		{
			// find currently focused child (if any)
			MWidget* cfoc = NULL;
			bool reverse = M_IsShiftDown();
			int cfoi = reverse ? m_Children.size() : -1;
			for (int i = 0; i < m_Children.size(); i++)
			{
				if (m_Children[i]->IsFocusable() && m_Children[i]->IsFocused())
				{
					cfoc = m_Children[i];
					cfoi = i;
					break;
				}
			}

			for (int i = 0; i < m_Children.size(); i++)
			{
				int cel = cfoi-i-1;
				if (cel < 0) cel = m_Children.size()+cel;
				MWidget* w = m_Children[(reverse ? cel : (cfoi+i+1)) % m_Children.size()];
				if (w->IsFocusable())
				{
					w->SetFocused(true);
					break;
				}
			}

			return true;
		}

		for (int i = 0; i < m_Children.size(); i++)
			if (m_Children[i]->OnKeyDown(vk)) return true;
		return false;
	}

	virtual bool OnKeyUp(int vk)
	{
		for (int i = 0; i < m_Children.size(); i++)
			if (m_Children[i]->OnKeyUp(vk)) return true;
		return false;
	}

	virtual bool OnTextEntered(int unichar)
	{
		for (int i = 0; i < m_Children.size(); i++)
			if (m_Children[i]->OnTextEntered(unichar)) return true;
		return false;
	}
	
	virtual bool OnMouseMove(int x, int y)
	{
		for (int i = 0; i < m_Children.size(); i++)
			if (m_Children[i]->OnMouseMove(x, y)) return true;
		return false;
	}

	virtual bool OnMouseDown(int x, int y, int button)
	{
		for (int i = 0; i < m_Children.size(); i++)
			if (m_Children[i]->OnMouseDown(x, y, button)) return true;
		return false;
	}

	virtual bool OnMouseUp(int x, int y, int button)
	{
		for (int i = 0; i < m_Children.size(); i++)
			if (m_Children[i]->OnMouseUp(x, y, button)) return true;
		return false;
	}

	MRect GetRect()
	{
		return m_Rect;
	}

	MRect GetAbsoluteRect()
	{
		MRect outr = m_Rect.Normalized();

		MWidget* w = m_Parent;
		while (w)
		{
			MRect tr = w->m_Rect.Normalized();
			outr.Left += tr.Left;
			outr.Right += tr.Left;
			outr.Top += tr.Top;
			outr.Bottom += tr.Top;
			w = w->m_Parent;
		}

		return outr;
	}

	void SetRect(MRect rect)
	{
		m_Rect = rect;
	}

	virtual bool OnChildMessage(MWidget* sender, int id, unsigned long d1, unsigned long d2) { return false; }

	void SendChildMessage(int id, unsigned long d1, unsigned long d2)
	{
		if (m_Parent)
			m_Parent->OnChildMessagePass(this, id, d1, d2);
	}

	bool IsFocused() { return m_Focused; }
	void SetFocused(bool f)
	{
		// set all siblings to unfocused, then focus this one
		if (m_Parent && f)
		{
			for (int i = 0; i < m_Parent->m_Children.size(); i++)
				m_Parent->m_Children[i]->SetFocused(false);
		}

		if (!f)
		{
			for (int i = 0; i < m_Children.size(); i++)
				m_Children[i]->SetFocused(false);
		}

		m_Focused = f;
	}

	virtual bool IsFocusable() { return false; }

	virtual void OnEnable() {}
	virtual void OnDisable() {}

	void SetEnabled(bool enabled) { if (m_Enabled != enabled) { m_Enabled = enabled; m_Enabled?OnEnable():OnDisable(); } }
	bool IsEnabled() { return m_Enabled; }
	
protected:
	std::vector<MWidget*> m_Children;
	MWidget* m_Parent;

	MRect m_Rect;

	virtual void OnChildMessagePass(MWidget* sender, int id, unsigned long d1, unsigned long d2)
	{
		if (!OnChildMessage(sender, id, d1, d2) && m_Parent)
			m_Parent->OnChildMessagePass(sender, id, d1, d2);
	}

	bool m_Focused;
	bool m_Enabled;

};

class MMainMenu : public MWidget
{
public:
	MMainMenu() : MWidget(NULL) {}

	virtual bool OnChildMessage(MWidget* sender, int id, unsigned long d1, unsigned long d2);
};

class MLabel : public MWidget
{
public:
	MLabel(MWidget* parent = NULL) : MWidget(parent)
	{
		m_TextImage = NULL;
		m_Bold = m_Italic = false;
	}

	void SetText(std::string text) { if (m_Text != text) { m_Text = text; UpdateImage(); } }
	std::string GetText() { return m_Text; }

	void SetAlign(int align) { m_Align = align; }
	int GetAlign() { return m_Align; }

	virtual void Display();

	bool IsBold() { return m_Bold; }
	void SetBold(bool bold) { if (m_Bold != bold) { m_Bold = bold; UpdateImage(); } }
	bool IsItalic() { return m_Italic; }
	void SetItalic(bool italic) { if (m_Italic != italic) { m_Italic = italic; UpdateImage(); } }

private:
	void UpdateImage();

	std::string m_Text;
	Image* m_TextImage;

	int m_Align;
	bool m_Bold;
	bool m_Italic;
};

class MTextField : public MWidget
{
public:
	MTextField(MWidget* parent = NULL);

	void SetPlaceholder(std::string text)
	{
		if (text != m_Placeholder)
		{
			m_Placeholder = text;
			UpdatePlaceholder();
		}
	}

	void SetText(std::string text)
	{
		if (text != m_Text)
		{
			m_Text = text;
			UpdateText();
		}
	}

	std::string GetPlaceholder() { return m_Placeholder; }
	std::string GetText() { return m_Text; }

	virtual void Display();

	virtual bool OnMouseDown(int x, int y, int button);
	virtual bool OnMouseUp(int x, int y, int button);
	virtual bool OnMouseMove(int x, int y);
	virtual bool OnKeyDown(int vk);
	virtual bool OnKeyUp(int vk);
	virtual bool OnTextEntered(int unichar);

	bool IsMasked() { return m_Masked; }
	void SetMasked(bool masked) { if (m_Masked != masked) { m_Masked = masked; UpdateText(); } }

	std::string GetMaskedText()
	{
		if (!m_Masked) return m_Text;
		std::string outs;
		for (int i = 0; i < m_Text.length(); i++)
			outs += '*';
		return outs;
	}

	virtual bool IsFocusable() { return true; }

private:
	void UpdatePlaceholder();
	void UpdateText();

	int GetSelectionByXY(int x, int y);

	std::string m_Placeholder;
	Image* m_PlaceholderImage;

	std::string m_Text;
	Image* m_TextImage;

	bool m_Hovered;

	unsigned long m_LastBlinkTime;

	int m_Selection1;
	int m_Selection2;

	bool m_SelectingK;
	bool m_SelectingM;

	bool m_Masked;
};

class MRadioList : public MWidget
{
public:
	MRadioList(MWidget* parent = NULL);
	virtual ~MRadioList();

	void AddOption(std::string text);

	int GetSelection() { return m_Selection; }
	void SetSelection(int selection) { m_Selection = selection; }

	virtual void Display();
	virtual bool OnMouseDown(int x, int y, int button);
	virtual bool OnMouseUp(int x, int y, int button);
	virtual bool OnMouseMove(int x, int y);
	virtual bool OnKeyDown(int vk);

	virtual bool IsFocusable() { return true; }

private:
	std::vector<Image*> m_Options;
	std::vector<Image*> m_OptionsHovered;
	
	int m_Selection;
	int m_Hovered;
	int m_Clicked;
};

class MEnterButton : public MWidget
{
public:
	MEnterButton(MWidget* parent = NULL);

	void SetText(std::string text) { if (m_Text != text) { m_Text = text; UpdateText(); } }
	std::string GetText() { return m_Text; }

	void SetMsg(int msg) { m_Msg = msg; }
	int GetMsg() { return m_Msg; }

	virtual void Display();
	virtual bool OnMouseDown(int x, int y, int button);
	virtual bool OnMouseUp(int x, int y, int button);
	virtual bool OnMouseMove(int x, int y);
	virtual bool OnKeyDown(int vk);
	virtual bool OnKeyUp(int vk);

	virtual bool IsFocusable() { return m_Enabled; }

protected:
	virtual void UpdateText();

	std::string m_Text;
	Image* m_TextImage;
	Image* m_TextImageHovered;

	bool m_Hovered;
	bool m_Clicked;
	
	int m_Msg;
};

class MCheckBox : public MWidget
{
public:
	MCheckBox(MWidget* parent = NULL);

	void SetText(std::string text) { if (m_Text != text) { m_Text = text; UpdateText(); } }
	std::string GetText() { return m_Text; }

	bool IsChecked() { return m_Checked; }
	void SetChecked(bool checked) { m_Checked = checked; }

	virtual void Display();
	virtual bool OnMouseDown(int x, int y, int button);
	virtual bool OnMouseUp(int x, int y, int button);
	virtual bool OnMouseMove(int x, int y);
	virtual bool OnKeyDown(int vk);
	virtual bool OnKeyUp(int vk);

	virtual bool IsFocusable() { return true; }

private:
	void UpdateText();

	bool m_Hovered;
	bool m_Clicked;
	bool m_Checked;
	
	std::string m_Text;
	Image* m_TextImage;
	Image* m_TextImageHovered;
};

class MIconButton : public MEnterButton
{
public:
	MIconButton(MWidget* parent = NULL) : MEnterButton(parent)
	{
		m_Icon = m_HoveredIcon = m_OffIcon = NULL;
		m_Enabled = true;
	}
	
	virtual ~MIconButton()
	{
		delete m_Icon;
		delete m_HoveredIcon;
	}

	void SetIcon(Image* icon, Image* hovered_icon, Image* off_icon) { m_Icon = icon; m_HoveredIcon = hovered_icon; m_OffIcon = off_icon; }
	Image* GetIcon() { return m_Icon; }
	Image* GetHoveredIcon() { return m_HoveredIcon; }
	Image* GetOffIcon() { return m_OffIcon; }

	virtual void Display();

	virtual void OnEnable() { UpdateText(); }
	virtual void OnDisable() { UpdateText(); }

private:
	virtual void UpdateText();

	Image* m_Icon;
	Image* m_HoveredIcon;
	Image* m_OffIcon;
};
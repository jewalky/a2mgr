#include "config.h"
#include "utils.h"

std::string current_directory = "";

unsigned long v_screentype = SCREENTYPE_PNG;

unsigned long g_scrollintv = 25;

bool r_fps = false;
bool r_time = false;

bool r_grid = false;

bool g_altchat = false;

unsigned long r_width = 640;
unsigned long r_height = 480;

bool z_screendir = true;
bool z_logsdir = true;
unsigned long z_resolution = 0;

bool z_russian = true;
bool z_lang_override = false;

bool show_stopwatch = false;

unsigned long net_key = 0;
unsigned long net_maxplayers = 16;

unsigned long r_grid_color = 0x3800;
unsigned long z_handle = 0;

unsigned long crc32_allods2_exe		= 0;
unsigned long crc32_a2mgr_dll		= 0;
unsigned long crc32_patch_res		= 0;//ccrc.CalcCRC(patchr.c_str());
unsigned long crc32_world_res		= 0;//ccrc.CalcCRC(worldr.c_str());
unsigned long crc32_graphics_res	= 0;//ccrc.CalcCRC(graphr.c_str());

unsigned char sha1_allods2_exe[20];
unsigned char sha1_a2mgr_dll[20];
unsigned char sha1_patch_res[20];
unsigned char sha1_world_res[20];
unsigned char sha1_graphics_res[20];
unsigned char sha1_uuid[20];

using namespace std;

void ParseConfig2x(ifstream& f_temp)
{
	unsigned long r_widtho = r_width;
	unsigned long r_heighto = r_height;
	bool invalidwh = false;

	string wh;
	while(getline(f_temp, wh))
	{
		wh = Trim(wh);
		if(!wh.length()) continue;
		size_t cw = wh.find_first_of("#;");
		if(cw != string::npos) wh.erase(cw);
		if(!wh.length()) continue;
		cw = wh.find("//");
		if(cw != string::npos) wh.erase(cw);
		if(!wh.length()) continue;

		vector<string> cmd = ParseCommandLine(wh);
		if(!cmd.size() || cmd.size() > 2) continue;

		if(cmd[0] == "v_screentype" && cmd.size() == 2)
		{
			bool invalid = false;
			if(CheckInteger(cmd[1]))
			{
				unsigned long stype = StrToInt(cmd[1]);
				if(stype > SCREENTYPE_GIF)
				{
					stype = v_screentype;
					invalid = true;
				}
				v_screentype = stype;
			}
			else
			{
				string ph = ToLower(cmd[1]);
				if(ph == "png") v_screentype = SCREENTYPE_PNG;
				else if(ph == "jpg") v_screentype = SCREENTYPE_JPG;
				else if(ph == "bmp") v_screentype = SCREENTYPE_BMP;
				else if(ph == "gif") v_screentype = SCREENTYPE_GIF;
				else invalid = true;
			}
		}
		else if(cmd[0] == "g_scrollspeed" && cmd.size() == 2)
		{
			bool invalid = false;
			if(CheckInteger(cmd[1]))
				g_scrollintv = StrToInt(cmd[1]);
			else invalid = true;

			if(!invalid)
			{
				if(g_scrollintv > 100) g_scrollintv = 100;
				unsigned long interval = 1000;
				unsigned long scrollintv = 100 - g_scrollintv;
				g_scrollintv = (interval / 100) * scrollintv;
			}
		}
		else if(cmd[0] == "r_fps")
		{
			if(cmd.size() == 2) r_fps = StrToBoolean(cmd[1]);
			else r_fps = true;
		}
		else if(cmd[0] == "r_time")
		{
			if(cmd.size() == 2) r_time = StrToBoolean(cmd[1]);
			else r_time = true;
		}
		else if(cmd[0] == "show_stopwatch")
		{
			if(cmd.size() == 2) show_stopwatch = StrToBoolean(cmd[1]);
			else show_stopwatch = true;
		}
		else if(cmd[0] == "g_altchat")
		{
			if(cmd.size() == 2) g_altchat = StrToBoolean(cmd[1]);
			else g_altchat = true;
		}
		else if(cmd[0] == "r_width" && cmd.size() == 2)
		{
			bool invalid = false;
			if(CheckInteger(cmd[1]))
				r_width = StrToInt(cmd[1]);
			else invalid = true;

			if(!invalidwh) invalidwh = invalid;
			if(!invalidwh) z_resolution++;
		}
		else if(cmd[0] == "r_height" && cmd.size() == 2)
		{
			bool invalid = false;
			if(CheckInteger(cmd[1]))
				r_height = StrToInt(cmd[1]);
			else invalid = true;

			if(!invalidwh) invalidwh = invalid;
			if(!invalidwh) z_resolution++;
		}
		else if(cmd[0] == "z_logsdir")
		{
			if(cmd.size() == 2) z_logsdir = StrToBoolean(cmd[1]);
			else z_logsdir = true;
		}
		else if(cmd[0] == "z_screendir")
		{
			if(cmd.size() == 2) z_screendir = StrToBoolean(cmd[1]);
			else z_screendir = true;
		}
		else if(cmd[0] == "z_language" && cmd.size() == 2)
		{
			string lng = ToLower(cmd[1]);
			if(lng == "rus" || lng == "russian")
			{
				z_russian = true;
				z_lang_override = true;
			}
			else if(lng == "eng" || lng == "english")
			{
				z_russian = false;
				z_lang_override = true;
			}
			else z_lang_override = false;
		}
		else if(cmd[0] == "r_grid")
		{
			if(cmd.size() == 2) r_grid = StrToBoolean(cmd[1]);
			else r_grid = true;
		}
		else if(cmd[0] == "r_grid_color")
		{
			if(cmd.size() != 2) continue;
			vector<string> parmc = Explode(cmd[1], ":");
			if(parmc.size() != 3) continue;
			unsigned long c_r = 0, c_b = 0, c_g = 0;
			if(CheckInteger(parmc[0])) c_r = StrToInt(parmc[0]);
			else continue;
			if(CheckInteger(parmc[1])) c_g = StrToInt(parmc[1]);
			else continue;
			if(CheckInteger(parmc[2])) c_b = StrToInt(parmc[2]);
			else continue;

			c_r = (unsigned long)((float)c_r / 255.0 * 31.0);
			c_g = (unsigned long)((float)c_g / 255.0 * 63.0);
			c_b = (unsigned long)((float)c_b / 255.0 * 31.0);

			unsigned long val = 0;
			val |= (c_r & 0x1F);
			val <<= 5;
			val |= (c_g & 0x3F);
			val <<= 6;
			val |= (c_b & 0x1F);

			r_grid_color = val;
		}
	}

	if(!z_resolution || z_resolution != 2) invalidwh = true;

	if(invalidwh)
	{
		r_width = r_widtho;
		r_height = r_heighto;
		z_resolution = 0;
	}
}

void ParseConfig1x(ifstream& f_temp)
{
	string wh;
	while(getline(f_temp, wh))
	{
		wh = Trim(wh);

		size_t cw = wh.find_first_of("=");
		if(cw == string::npos) continue;

		string prm = wh;
		string val = wh;
		prm.erase(cw);
		val.erase(0, cw+1);

		prm = Trim(prm);
		val = Trim(val);

		if(prm == "scroll_inverval")
			g_scrollintv = StrToInt(val);
		else if(prm == "show_fps")
			r_fps = StrToBoolean(val);
		else if(prm == "show_watches")
			r_time = StrToBoolean(val);
		else if(prm == "alt_chat")
			g_altchat = StrToBoolean(val);
	}
}

void ParseConfig()
{
	ifstream f_temp;
	bool oldfmt = false;
	f_temp.open("allods2.cfg", ios::in);
	if(!f_temp.is_open())
	{
		f_temp.open("allods2.conf", ios::in);
		if(!f_temp.is_open()) return;
		else oldfmt = true;
	}

	if(oldfmt) ParseConfig1x(f_temp);
	else ParseConfig2x(f_temp);

	f_temp.close();
}
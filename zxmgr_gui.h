#pragma once

#include <string>

namespace zxmgr
{
	namespace GUI
	{
		extern unsigned long DefCWindowVtbl[36];

		class CWindow
		{
		public:
			static inline unsigned long PopulateWindow(unsigned long a1, RECT* rec){ return 0; }
			static inline void IdleProcessing() {}
			static inline unsigned long OnMessage(unsigned long msg, unsigned long wparam, unsigned long lparam) { return 0; }

			unsigned long* _VTABLE;
			unsigned long u_0;
			unsigned long u_1;
			unsigned long u_2;
			unsigned long u_3;
			unsigned long u_4;
			unsigned long u_5;
			unsigned long u_6;
			unsigned long u_7;
			unsigned long u_8;
			unsigned long u_9;
			unsigned long u_10;
			unsigned long u_11;
			unsigned long u_12;
			unsigned long u_13;
			unsigned long u_14;
			unsigned long u_15;
			unsigned long u_16;
			unsigned long u_17;
			unsigned long u_18;
			unsigned long u_19;
			unsigned long u_20;
			unsigned long u_21;
			unsigned long u_22;
			unsigned long u_23;
			unsigned long u_24;
			unsigned long u_25;
			unsigned long u_26;
			unsigned long u_27;
			unsigned long u_28;
			unsigned long u_29;
			unsigned long u_30;

			inline void ReplaceRoutines() { this->_VTABLE = DefCWindowVtbl; }
			void InitializeWindow(unsigned long unkint, unsigned long left, unsigned long top, unsigned long right, unsigned long bottom, unsigned long unkint2);
			void ShowWindow();
		};
	}
}
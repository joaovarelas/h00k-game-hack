#pragma once
#include "main.h"


class SettingVars
{
public:
		float aim_bot, aim_key, aim_spot, aim_fov, aim_shoot, aim_silent, aim_smooth, aim_miscweap; //AIM
		float esp_box, esp_name, esp_weapon, esp_healthbar, esp_armorbar, esp_visible; //ESP
		float rem_recoil, rem_spread, rem_flash, rem_smoke, rem_visrecoil; //REMOVALS
		float misc_autopistol, misc_bhop, misc_antiaim, misc_watermark, misc_lightspam, misc_speedhack, misc_spinbot; //MISC
		float menu_x, menu_y; //MENUPOS
		float menu1,menu2,menu3,menu4,menu5; //SUBMENUS
};

extern SettingVars Cvars;
#ifndef TIBIAWASD_H
#define TIBIAWASD_H

void UpdateTitle();
void DisplayError(DWORD errorcode);

class Settings;
extern Settings settings;
extern const char *tibiaWasd;

/* kConnectionStatusIsOnline values
	Tibia 780 - 986 == 8
	Tibia 1000 - 1010 == 10
	Tibia 1011 == 11
*/
const int kConnectionStatusIsOnline = 11;
const int kActionStateModuleWindowOpen = 11;

#endif
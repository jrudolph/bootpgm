/* The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * The Initial Developer of the Original Code is Johannes Rudolph.
 * Portions created by the Initial Developer are Copyright (C) 2006
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s): 
 *    Johannes Rudolph <johannes_rudolph@gmx.de>
 */ 

#pragma once

class RegKey;
class IO;
class Main;

class RegistryBrowser
{
	RegKey *current;
	RegKey *get_current_key();
public:
	RegistryBrowser(Main &m);
	~RegistryBrowser(void);

	void lk(IO &io,char *args);
	void lv(IO &io,char *args);
	void ck(IO &io,char *args);
	void tk(IO &io,char *args);
};

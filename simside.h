/*--------------------------------------------------------
  "THE BEER-WARE LICENSE" (Revision 42):
  Alex Kostyuk wrote this code. As long as you retain this
  notice you can do whatever you want with this stuff.
  If we meet some day, and you think this stuff is worth it,
  you can buy me a beer in return.
----------------------------------------------------------*/

#ifndef __SIMSIDE_H__
#define __SIMSIDE_H__

float main_callback(float elapsedPrev, float elapsedFlight, int counter, void *ptr);
void dirty_hack(void);
void pass_to_sim(ULONG64 ev_container,int level);
void reload_datarefs(void);
void re_register_all(void);
#endif

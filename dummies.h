/*--------------------------------------------------------
  "THE BEER-WARE LICENSE" (Revision 42):
  Alex Kostyuk wrote this code. As long as you retain this
  notice you can do whatever you want with this stuff.
  If we meet some day, and you think this stuff is worth it,
  you can buy me a beer in return.
----------------------------------------------------------*/


void  XPLMUnregisterFlightLoopCallback(
                                   XPLMFlightLoop_f     inFlightLoop,
                                   void *               inRefcon)
{

}

XPLMWindowID XPLMCreateWindow(int inLeft, int inTop, int inRight, int inBottom,
	int inIsVisible, XPLMDrawWindow_f inDrawCallback, XPLMHandleKey_f inKeyCallback,
	XPLMHandleMouseClick_f inMouseCallback, void * inRefcon) {
	return 0;
}
void XPLMDestroyWindow(XPLMWindowID inWindowID) {
}
void XPLMGetWindowGeometry(XPLMWindowID inWindowID,
	int * outLeft, int * outTop, int * outRight, int * outBottom) {
}
void XPLMDrawTranslucentDarkBox(int inLeft, int inTop, int inRight, int inBottom) {
}
void XPLMDrawString(float *inColorRGB, int inXOffset, int inYOffset, char *inChar,
	int *inWordWrapWidth, XPLMFontID inFontID) {
}
void XPLMRegisterFlightLoopCallback(XPLMFlightLoop_f inFlightLoop, float inInterval,
	void *inRefcon) {
}
float XPLMGetElapsedTime(void) {
	return 0.0;
}

XPLMCommandRef XPLMFindCommand(const char *inName) {
    return NULL;
}
void XPLMCommandBegin(XPLMCommandRef inCommand) {
}
void XPLMCommandEnd(XPLMCommandRef inCommand) {
}
void XPLMCommandOnce(XPLMCommandRef inCommand) {
}
XPLMDataRef XPLMFindDataRef(const char *inDataRefName) {
	return NULL;
}
int XPLMCanWriteDataRef(XPLMDataRef inDataRef) {
	return 0;
}
XPLMDataTypeID XPLMGetDataRefTypes(XPLMDataRef inDataRef) {
	return 0;
}

void                 XPLMGetNthAircraftModel(
                                   int                  inIndex,
                                   char *               outFileName,
                                   char *               outPath)
{
sprintf(outFileName,"model");
sprintf(outPath,"path");
}


int XPLMGetDatai(XPLMDataRef inDataRef) {
	return 0;
}


void XPLMSetDatai(XPLMDataRef inDataRef, int inValue) {
}
float XPLMGetDataf(XPLMDataRef inDataRef) {
	return 0;
}
void XPLMSetDataf(XPLMDataRef inDataRef, float inValue) {
}
double XPLMGetDatad(XPLMDataRef inDataRef) {
	return 0;
}
void XPLMSetDatad(XPLMDataRef inDataRef, double inValue) {
}

int XPLMGetDatavi(XPLMDataRef inDataRef, int *outValues, int inOffset, int inMax) {
	return 0;
}

void XPLMSetDatavi(XPLMDataRef inDataRef, int *inValues, int inoffset, int inCount) {
}

int XPLMGetDatavf(XPLMDataRef inDataRef, float *outValues, int inOffset, int inMax) {
	return 0;
}

void XPLMSetDatavf(XPLMDataRef inDataRef, float *inValues, int inoffset, int inCount) {
}

XPLMMenuID XPLMFindPluginsMenu(void) {
	return 0;
}

int XPLMAppendMenuItem(XPLMMenuID inMenu, const char *inItemName,
  void *inItemRef, int inForceEnglish) {
	return 0;
}

void XPLMCheckMenuItem(XPLMMenuID inMenu, int index, XPLMMenuCheck inCheck) {
}

XPLMMenuID XPLMCreateMenu(const char *         inName,    XPLMMenuID           inParentMenu,    int                  inParentItem,    XPLMMenuHandler_f    inHandler,    void *               inMenuRef) {
	return 0;
}
void XPLMSetMenuItemName(XPLMMenuID inMenu, int inIndex, const char *inItemName, int inForceEnglish) {
}

float XPLMMeasureString(XPLMFontID inFontID, const char *inChar, int inNumChars) {
	return 0.0;
}

int                  XPLMGetDatab(
                                   XPLMDataRef          inDataRef,
                                   void *               outValue,    /* Can be NULL */
                                   int                  inOffset,
                                   int                  inMaxBytes)
{
return 0;
}


void XPLMDestroyMenu(XPLMMenuID dummy)
{

}

void XPLMClearAllMenuItems(XPLMMenuID inMenuID)
{

}


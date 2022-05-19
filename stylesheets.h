#ifndef __STYLESHEETS_H
#define __STYLESHEETS_H
// to the get the stuff a bit closer together
const char styleSheetCombo[] = "padding-left:1px; padding-right:1px";
#ifdef _WIN32
char styleSheetChannel[] = "padding-left:1px; padding-right:1px;";
char styleSheetGain[] = "padding-left:1px; padding-right:1px;";
#else
char styleSheetChannel[] = "padding-left:1px; padding-right:1px; min-width: 3.5em;";
char styleSheetGain[] = "padding-left:1px; padding-right:1px; min-width: 3.5em;";
#endif
char styleSheetLabel[] = "padding-left:0.5em; padding-right:1px";
char styleSheetNoPadding[] = "padding-left:1px; padding-right:1px; width:1em; font-family: courier;";
char styleCheckBox[] = "QCheckBox::indicator {width: 2em; height: 2em;}";
char styleLineEdit[] = "";
char styleProfile[] = "font-size:12px;";
#endif


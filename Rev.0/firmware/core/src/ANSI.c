// VT100 terminal command strings

const char *TERMINAL_CLEAR = "\033c\033[2J";
const char *TERMINAL_QUERY = "\033[s\033[999;999H\033[6n\033[u";
const char *TERMINAL_SET_SCROLL_AREA = "\033[s\033[0;%dr\033[u";
const char *TERMINAL_ECHO = "\033[s\033[%d;0H\033[0K>> %s\033[u";
const char *TERMINAL_CLEARLINE = "\033[s\033[%d;0H\033[0K>> \033[u";
const char *TERMINAL_DISPLAY = "\033[s\033[%d;0H\033[0K>> %s\033[u";

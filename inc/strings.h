#ifndef STRINGS_H
#define STRINGS_H
#ifdef MODULE_MAIN
const char* str_bootup_1 = "Hochfahren";
const char* str_bootup_2 = "www.schwaab.cc";

const char* str_recall_1 = "DMX Master";
const char* str_recall_2 = "Szene ";

const char* str_passtrough_1 = "Durchleitung";

const char* str_record_1 = "Auswahl Szene ";
const char* str_record_2 = "Record zum Start";

const char* str_record_searchframe_1 = "Suche DMX Paket";

const char* str_record_foundframe_1 = "DMX Paket";
const char* str_record_foundframe_2 = "gefunden";

const char* str_record_savedframe_1 = "Paket gesp.";
const char* str_record_savedframe_2 = "Exit um zurueck";

const char* str_record_timeout_1 = "Kein Paket gef.";
const char* str_record_timeout_2 = "Exit um zurueck";

const char* str_null = "                ";
#else
extern const char* str_bootup_1;
extern const char* str_bootup_2;

extern const char* str_recall_1; 
extern const char* str_recall_2;

extern const char* str_record_1;
extern const char* str_record_2;

extern const char* str_passtrough_1;

extern const char* str_record_searchframe_1;

extern const char* str_record_foundframe_1;
extern const char* str_record_foundframe_2;

extern const char* str_record_savedframe_1;
extern const char* str_record_savedframe_2;

extern const char* str_record_timeout_1;
extern const char* str_record_timeout_2;

extern const char* str_null;

#endif
#endif
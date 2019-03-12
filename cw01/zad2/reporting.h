#pragma once

int start_reporting(char* filename, char* title, int append);
int add_report_text(char* content);
void start_report_timer();
int stop_report_timer(char* text);
int stop_reporting();


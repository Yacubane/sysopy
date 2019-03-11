#pragma once

int start_reporting(char* filename, char* title, int flag);
int add_report_text(char* content);
int stop_reporting();

void add_first_reporting_line();
void start_report_timer();
void stop_report_timer(char* text);

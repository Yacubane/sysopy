#pragma once

int fork_job(char* path, int refresh_seconds, int monitor_seconds, int type,
    int cpu_restriction, int memory_restriction);
#pragma once

void initMemory();
int readMemory(const char* key);
void persistMemory(const char* key, int value);
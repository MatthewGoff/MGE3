#pragma once

namespace MGE { namespace String {

char* ToString(char* buffer, int val);
char* ToString(float val);
int Compare(char* s1, char* s2);
char* Format(char* msg, ...);

}}
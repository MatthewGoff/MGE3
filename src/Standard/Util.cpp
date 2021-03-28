/*
Reverse the order of bytes in a 4 byte word
*/
int Util::ReverseEnd(int word)
{
    int result = 0;
    result |= (word & 0xF000) >> (3 * 8);
    result |= (word & 0x0F00) >> (1 * 8);
    result |= (word & 0x00F0) << (1 * 8);
    result |= (word & 0x000F) << (3 * 8);
    return result;
}

int Util::Min(int a, int b)
{
    if (a < b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

int Util::Max(int a, int b)
{
    if (a > b)
    {
        return a;
    }
    else
    {
        return b;
    }
}
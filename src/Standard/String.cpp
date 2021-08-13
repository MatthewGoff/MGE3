
namespace MGE { namespace Standard { namespace String {

char* ToString(char* buffer, int val)
{
    // buffer size >= 32
    *(buffer + 31) = 0;
    
    bool negative = false;
    if (val < 0)
    {
        negative = true;
        val *= -1;
    }

    int mag;
    if (val == 0)
    {
        mag = 0;
    }
    else
    {
        mag = (int)log10(val);
    }
    
    char* runner;
    for (int place = 0; place <= mag; place++)
    {
        int measure = pow(10, place);
        int digit = val / measure;
        digit = digit % 10;
        digit += 48; // ascii offset
        
        runner = buffer + 31 - 1 - place;
        *runner = (char)digit;
    }
    
    if (negative)
    {
        *(--runner) = '-';
    }
    
    return runner;
}

char* ToString(float val)
{
    return nullptr;
}

/*
Return:
<0 iff s1 < s2
=0 iff s1 == s2
>0 iff s1 > s2
*/
int Compare(char* s1, char* s2)
{
    while (*s1 == *s2)
    {
        if (*s1 == 0)
        {
            return 0;
        }
        s1++;
        s2++;
    }
    return (int)(*s1 - *s2);
}

}}}
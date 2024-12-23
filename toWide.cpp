#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

wchar_t* toWide(const char* utf8str)
{
    int size_needed, chars_copied;
    wchar_t* wstr;

    if (utf8str == NULL) return NULL;

    /* 必要なバッファサイズを取得 */
    size_needed = MultiByteToWideChar(CP_UTF8, 0, utf8str, -1, NULL, 0);
    if (size_needed <= 0) return NULL;

    /* バッファを確保 */
    wstr = (wchar_t*)malloc(sizeof(wchar_t) * size_needed);
    if (wstr == NULL) return NULL;

    /* 変換を実行 */
    chars_copied = MultiByteToWideChar(CP_UTF8, 0, utf8str, -1, wstr, size_needed);
    if (chars_copied <= 0)
    {
        free(wstr);
        return NULL;
    }

    return wstr;
}
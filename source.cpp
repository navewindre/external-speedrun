#include "process.h"
#include "vector.h"

c_process g_process( "csgo.exe" );

constexpr std::ptrdiff_t m_dwLocalPlayer	= 0xAAFFFC;
constexpr std::ptrdiff_t m_dwClientState	= 0x5A3334;
constexpr std::ptrdiff_t m_ClientStateAngles	= 0x4D10;
constexpr std::ptrdiff_t m_vecPunch		= 0x301C;

void main( ) {
	if ( !g_process.open_process( ) ) {
		printf( "process not found\n" );
		system( "pause" );
		return;
	}

	g_process.m_client = g_process.get_module( "client.dll" );
	g_process.m_engine = g_process.get_module( "engine.dll" );

	for ( ;; ) {
		auto player_base	= g_process.read< uintptr_t >( g_process.m_client + m_dwLocalPlayer );
		auto client_state	= g_process.read< uintptr_t >( g_process.m_engine + m_dwClientState );

		static vec3_t old_punch;

		if ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 ) {
			auto cur_angles = g_process.read< vec3_t >( client_state + m_ClientStateAngles );
			auto cur_punch	= g_process.read< vec3_t >( player_base + m_vecPunch ) * 2.f;

			auto punch_delta = cur_punch - old_punch;
			old_punch = cur_punch;
			
			g_process.write< vec3_t >( client_state + m_ClientStateAngles, ( cur_angles - punch_delta ).clamp( ) );
		}
		else {
			old_punch = vec3_t{ };
		}
	}
}

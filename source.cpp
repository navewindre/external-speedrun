#include <thread>
#include "process.h"
#include "vector.h"

c_process g_process( "csgo.exe" );

constexpr std::ptrdiff_t m_dwLocalPlayer	= 0xAA8BBC;
constexpr std::ptrdiff_t m_dwClientState	= 0x5A4334;
constexpr std::ptrdiff_t m_ClientStateAngles	= 0x4D10;
constexpr std::ptrdiff_t m_vecPunch		= 0x301C;
constexpr std::ptrdiff_t m_fFlags		= 0x100;
constexpr std::ptrdiff_t m_dwForceJump		= 0x4F1C928;

using namespace std::chrono_literals;

void run_rcs( ) {
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

void run_bhop( ) {
	if ( !( GetAsyncKeyState( VK_SPACE ) & 0x8000 ) ) 
		return;

	auto player_base	= g_process.read< uintptr_t >( g_process.m_client + m_dwLocalPlayer );
	auto player_flags	= g_process.read< int >( player_base + m_fFlags );
				/*FL_ONGROUND*/
	bool air = !( player_flags & 1 << 0 );

	g_process.write< int >( g_process.m_client + m_dwForceJump, air ? 4 : 6 );
}

void main( ) {
	if ( !g_process.open_process( ) ) {
		printf( "process not found\n" );
		system( "pause" );
		return;
	}

	g_process.m_client = g_process.get_module( "client.dll" );
	g_process.m_engine = g_process.get_module( "engine.dll" );

	for ( ;; ) {
		run_rcs( );
		run_bhop( );
		std::this_thread::sleep_for( 1ms );
	}
}

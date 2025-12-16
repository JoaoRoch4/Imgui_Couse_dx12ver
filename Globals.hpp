#pragma once
#include "PCH.hpp"
#include "Classes.hpp"

static const int APP_NUM_FRAMES_IN_FLIGHT = 2;
static   const int APP_NUM_BACK_BUFFERS  = 2;
static   const int APP_SRV_HEAP_SIZE = 64;




class Globals {
private:
   

uint32_t WINDOW_WIDTH;
uint32_t WINDOW_HEIGHT;
bool bShowConsole;

public:


inline uint32_t Get_WINDOW_HEIGHT() const {
	return WINDOW_HEIGHT;
}

inline uint32_t Get_WINDOW_WIDTH() const {
	return WINDOW_WIDTH;
}

inline bool Get_bShowConsole() const {
	return bShowConsole;
}



inline int Get_APP_NUM_BACK_BUFFERS() const {
	return APP_NUM_BACK_BUFFERS;
}

inline int Get_APP_SRV_HEAP_SIZE() const {
	return APP_SRV_HEAP_SIZE;
}

	Globals();
	virtual ~Globals();
};

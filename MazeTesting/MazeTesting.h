
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include <set>
#include <stack>

#include <functional>

#include "LibN64.h"

using namespace LibN64;

class MazeTest : public Frame  {
private:	
	int  m_nMazeWidth;
	int  m_nMazeHeight;
	int *m_maze;
	

	// Some bit fields for convenience
	enum
	{
		CELL_PATH_N = 0x01,
		CELL_PATH_E = 0x02,
		CELL_PATH_S = 0x04,
		CELL_PATH_W = 0x08,
		CELL_VISITED = 0x10,
	};
	
	
	// Algorithm variables
	int  m_nVisitedCells;	
	std::stack<std::pair<int, int>> m_stack;	// (x, y) coordinate pairs
	int  m_nPathWidth;

 public:
   MazeTest(resolution_t res, bitdepth_t dep, antialias_t aa, UIType ui) : Frame(res, dep, aa, ui) {}

   virtual void OnCreate() override;
   virtual void FrameUpdate() override;

};
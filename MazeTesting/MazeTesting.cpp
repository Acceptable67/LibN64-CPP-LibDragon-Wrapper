#include <stdio.h>
#include <sstream>
#include "MazeTesting.h"

#include <memory>
#include <typeinfo>
#include <bitset>
#include <algorithm>
#include <chrono>
#include <ctime>




auto MazeTest::OnCreate() -> void
{

		m_nMazeWidth = 60;
		m_nMazeHeight = 50;		
		m_maze = new int[m_nMazeWidth * m_nMazeHeight];
		memset(m_maze, 0x00, m_nMazeWidth * m_nMazeHeight * sizeof(int));
		m_nPathWidth = 3;
		
		// Choose a starting cell
		int x = rand() % m_nMazeWidth;
		int y = rand() % m_nMazeHeight;
		m_stack.push(std::make_pair(x, y));
		m_maze[y * m_nMazeWidth + x] = CELL_VISITED;
		m_nVisitedCells = 1;
}

void MazeTest::FrameUpdate()
{
		
		// Little lambda function to calculate index in a readable way
		auto offset = [&](int x, int y)
		{
			return (m_stack.top().second + y) * m_nMazeWidth + (m_stack.top().first + x);
		};

		// Do Maze Algorithm
		if (m_nVisitedCells < m_nMazeWidth * m_nMazeHeight)
		{
			// Create a set of unvisted neighbours
			std::vector<int> neighbours;

			// North neighbour
			if (m_stack.top().second > 0 && (m_maze[offset(0, -1)] & CELL_VISITED) == 0)
				neighbours.push_back(0);
			// East neighbour
			if (m_stack.top().first < m_nMazeWidth - 1 && (m_maze[offset(1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(1);
			// South neighbour
			if (m_stack.top().second < m_nMazeHeight - 1 && (m_maze[offset(0, 1)] & CELL_VISITED) == 0)
				neighbours.push_back(2);
			// West neighbour
			if (m_stack.top().first > 0 && (m_maze[offset(-1, 0)] & CELL_VISITED) == 0)
				neighbours.push_back(3);

			// Are there any neighbours available?
			if (!neighbours.empty())
			{
				// Choose one available neighbour at random
				int next_cell_dir = neighbours[rand() % neighbours.size()];

				// Create a path between the neighbour and the current cell
				switch (next_cell_dir)
				{
				case 0: // North
					m_maze[offset(0, -1)] |= CELL_VISITED | CELL_PATH_S;
					m_maze[offset(0,  0)] |= CELL_PATH_N;
					m_stack.push(std::make_pair((m_stack.top().first + 0), (m_stack.top().second - 1)));
					break;

				case 1: // East
					m_maze[offset(+1, 0)] |= CELL_VISITED | CELL_PATH_W;
					m_maze[offset( 0, 0)] |= CELL_PATH_E;
					m_stack.push(std::make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
					break;

				case 2: // South
					m_maze[offset(0, +1)] |= CELL_VISITED | CELL_PATH_N;
					m_maze[offset(0,  0)] |= CELL_PATH_S;
					m_stack.push(std::make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
					break;

				case 3: // West
					m_maze[offset(-1, 0)] |= CELL_VISITED | CELL_PATH_E;
					m_maze[offset( 0, 0)] |= CELL_PATH_W;
					m_stack.push(std::make_pair((m_stack.top().first - 1), (m_stack.top().second + 0)));
					break;

				}

				m_nVisitedCells++;
			}
			else
			{
				// No available neighbours so backtrack!
				m_stack.pop();
			}
		}

		for (int x = 0; x < m_nMazeWidth; x++)
		{
			for (int y = 0; y < m_nMazeHeight; y++)
			{
				// Each cell is inflated by m_nPathWidth, so fill it in
				for (int py = 0; py < m_nPathWidth; py++)
					for (int px = 0; px < m_nPathWidth; px++)
					{
						if (m_maze[y * m_nMazeWidth + x] & CELL_VISITED)
							DrawPixel({x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py}, WHITE); // Draw Cell
						else
							DrawPixel({x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py}, SKY_BLUE); // Draw Cell
					}

				// Draw passageways between cells
				for (int p = 0; p < m_nPathWidth; p++)
				{
					if (m_maze[y * m_nMazeWidth + x] & CELL_PATH_S)
						DrawPixel({x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth}, WHITE); // Draw South Passage

					if (m_maze[y * m_nMazeWidth + x] & CELL_PATH_E)
						DrawPixel({x * (m_nPathWidth + 1) + m_nPathWidth, y * (m_nPathWidth + 1) + p}, WHITE); // Draw East Passage
				}
			}
		}

		// Draw Unit - the top of the stack
		for (int py = 0; py < m_nPathWidth; py++)
			for (int px = 0; px < m_nPathWidth; px++)
				DrawPixel({m_stack.top().first * (m_nPathWidth + 1) + px, m_stack.top().second * (m_nPathWidth + 1) + py}, GREEN); // Draw Cell
		

}

int main(void) {
	MazeTest n64g(RESOLUTION_256x240, DEPTH_32_BPP, ANTIALIAS_RESAMPLE, Frame::UIType::GUI);
	n64g.Begin();
	return 0;
}

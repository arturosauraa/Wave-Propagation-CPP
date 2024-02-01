#include <raylib.h>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <utility>
#include <tuple>
#include <math.h>

using namespace std;

class Pathfinding{

public:

	/// Variables
	int mapWidth;
	int mapHeight;
	int cellSize;
	int borderWidth;

	bool *obstacleMap;

	int *flowFieldZ;

	int startX;
	int startY;
	int endX;
	int endY;


    Pathfinding()
    {
        borderWidth = 4;
		cellSize = 50;
		mapWidth = GetScreenWidth() / cellSize;
		mapHeight = GetScreenHeight() / cellSize;
		obstacleMap = new bool[mapWidth * mapHeight]{ false };
		flowFieldZ = new int[mapWidth * mapHeight]{ 0 };


		startX = 3;
		startY = 7;
		endX = 12;
		endY = 7;
    }

    void Draw()
    {

    }

    void Update()
    {
        auto p = [&](int x, int y) { return y * mapWidth + x; };

        int selectedCellX = GetMouseX() / cellSize;
		int selectedCellY = GetMouseY() / cellSize;

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            obstacleMap[p(selectedCellX, selectedCellY)] = !obstacleMap[p(selectedCellX, selectedCellY)];
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
			if(IsKeyDown(KEY_LEFT_SHIFT))
			{
				endX = selectedCellX;
				endY = selectedCellY;
			} else 
			{
				startX = selectedCellX;
				startY = selectedCellY;
			}

        }

        for(int x = 0; x < mapWidth;x++)
        {
            for(int y = 0; y <mapHeight;y++)
            {
                if(x == 0 || y == 0 || x == (mapWidth - 1) || y == (mapHeight - 1) || obstacleMap[p(x, y)])
				{
					flowFieldZ[p(x, y)] = -1;
				}
				else
				{
					flowFieldZ[p(x, y)] = 0;
				}
            }
        }

		list<tuple<int, int, int>> nodes;

		nodes.push_back({ endX, endY, 1 });

        while(!nodes.empty())
        {
            list<tuple<int,int,int>> new_nodes;

            for (auto &n : nodes)
			{
				int x = get<0>(n); // Map X-Coordinate
				int y = get<1>(n); // Map Y-Coordinate
				int d = get<2>(n); // Distance From Target Location

				flowFieldZ[p(x, y)] = d;

				// East
				if ((x + 1) < mapWidth && flowFieldZ[p(x + 1, y)] == 0)
					new_nodes.push_back({ x + 1, y, d + 1 });

				// West
				if ((x - 1) >= 0 && flowFieldZ[p(x - 1, y)] == 0)
					new_nodes.push_back({ x - 1, y, d + 1 });

				// South
				if ((y + 1) < mapHeight && flowFieldZ[p(x, y + 1)] == 0)
					new_nodes.push_back({ x, y + 1, d + 1 });

				// North
				if ((y - 1) >= 0 && flowFieldZ[p(x, y - 1)] == 0)
					new_nodes.push_back({ x, y - 1, d + 1 });
			}

            new_nodes.sort([&](const std::tuple<int, int, int> &n1, const std::tuple<int, int, int> &n2)
			{
				return p(std::get<0>(n1), std::get<1>(n1)) < p(std::get<0>(n2), std::get<1>(n2));
			});


			new_nodes.unique([&](const std::tuple<int, int, int> &n1, const std::tuple<int, int, int> &n2)
			{
				return  p(std::get<0>(n1), std::get<1>(n1)) == p(std::get<0>(n2), std::get<1>(n2));
			});


			nodes.clear();
			nodes.insert(nodes.begin(), new_nodes.begin(), new_nodes.end());

        }

        list<pair<int, int>> path;
		path.push_back({ startX, startY });
		int locX = startX;
		int locY = startY;
		bool noPath = false;

		while (!(locX == endX && locY == endY) && !noPath)
		{
			std::list<std::tuple<int, int, int>> listNeighbours;

			// 4-Way Connectivity
			if ((locY - 1) >= 0 && flowFieldZ[p(locX, locY - 1)] > 0)
				listNeighbours.push_back({ locX, locY - 1, flowFieldZ[p(locX, locY - 1)] });

			if ((locX + 1) < mapWidth && flowFieldZ[p(locX + 1, locY)] > 0)
				listNeighbours.push_back({ locX + 1, locY, flowFieldZ[p(locX + 1, locY)] });

			if ((locY + 1) < mapHeight && flowFieldZ[p(locX, locY + 1)] > 0)
				listNeighbours.push_back({ locX, locY + 1, flowFieldZ[p(locX, locY + 1)] });

			if ((locX - 1) >= 0 && flowFieldZ[p(locX - 1, locY)] > 0)
				listNeighbours.push_back({ locX - 1, locY, flowFieldZ[p(locX - 1, locY)] });

			// 8-Way Connectivity
			if ((locY - 1) >= 0 && (locX - 1) >= 0 && flowFieldZ[p(locX - 1, locY - 1)] > 0)
				listNeighbours.push_back({ locX - 1, locY - 1, flowFieldZ[p(locX - 1, locY - 1)] });

			if ((locY - 1) >= 0 && (locX + 1) < mapWidth && flowFieldZ[p(locX + 1, locY - 1)] > 0)
				listNeighbours.push_back({ locX + 1, locY - 1, flowFieldZ[p(locX + 1, locY - 1)] });

			if ((locY + 1) < mapHeight && (locX - 1) >= 0 && flowFieldZ[p(locX - 1, locY + 1)] > 0)
				listNeighbours.push_back({ locX - 1, locY + 1, flowFieldZ[p(locX - 1, locY + 1)] });

			if ((locY + 1) < mapHeight && (locX + 1) < mapWidth && flowFieldZ[p(locX + 1, locY + 1)] > 0)
				listNeighbours.push_back({ locX + 1, locY + 1, flowFieldZ[p(locX + 1, locY + 1)] });

			listNeighbours.sort([&](const std::tuple<int, int, int> &n1, const std::tuple<int, int, int> &n2)
			{
				return std::get<2>(n1) < std::get<2>(n2);
			});

			if (listNeighbours.empty()) 
				noPath = true;
			else
			{
				locX = std::get<0>(listNeighbours.front());
				locY = std::get<1>(listNeighbours.front());
				path.push_back({ locX, locY });
			}
		}



        for(int x = 0; x <mapWidth;x++)
        {
            for(int y = 0; y < mapHeight;y++)
            {
                Color tileColor = BLUE;

                if(obstacleMap[p(x,y)] || flowFieldZ[p(x,y)] == -1 )
                tileColor = GRAY;

                if(x == startX && y == startY)
                tileColor = GREEN;

                if(x == endX && y == endY)
                tileColor = RED;

                DrawRectangle(x * cellSize,y * cellSize,cellSize - borderWidth,cellSize - borderWidth,tileColor);
                DrawText(TextFormat("%i",flowFieldZ[p(x,y)]),x * cellSize + cellSize/3, y * cellSize + cellSize/3, 20, WHITE);
                
            }
        }

        bool firstPoint = true;
		int ox, oy;

        for(auto &a : path)
        {
            if (firstPoint)
			{
				ox = a.first;
				oy = a.second;
				firstPoint = false;
			}
			else
			{
				DrawLine(
					ox * cellSize + ((cellSize - borderWidth) / 2),
					oy  * cellSize + ((cellSize - borderWidth) / 2),
					a.first * cellSize + ((cellSize - borderWidth) / 2),
					a.second * cellSize + ((cellSize - borderWidth) / 2), YELLOW);

				ox = a.first;
				oy = a.second;

				DrawCircle(ox * cellSize + ((cellSize - borderWidth) / 2), oy * cellSize + ((cellSize - borderWidth) / 2), 10, YELLOW);
			}
        }
    }
};

int main()
{
    InitWindow(750, 750, "Wave Propagation");
    SetTargetFPS(60);

    Pathfinding pathFinding = Pathfinding();
 
 
    while(WindowShouldClose() == false)
    {
        BeginDrawing();

        pathFinding.Update();

        ClearBackground(BLACK);

        pathFinding.Draw();

        EndDrawing();


    }
    CloseWindow();
    return 0;
} 
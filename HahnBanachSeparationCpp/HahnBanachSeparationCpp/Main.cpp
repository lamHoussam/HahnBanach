#define SDL_MAIN_HANDLED

#include <iostream>
#include <vector>
#include <SDL.h>

#define	APP_WIDTH  480
#define APP_HEIGHT 640

#define POINT_SIZE 2

typedef struct Point {
public:
	Point(int _x, int _y) : x(_x), y(_y) {}

	inline int GetX() const { return x; } 
	inline int GetY() const { return y; }

private:
	int x, y;
};

typedef struct Polygon {
public:
	Polygon(){}

	void AddPoint(const Point point){ 
		points.push_back(point);
	}

	void Render(SDL_Renderer* renderer) {
		int numPoints = points.size();
		if (numPoints <= 1)
			return;

		for (int i = 0; i < numPoints; i++) {
			Point current = points[i];
			Point next = points[(i + 1) % numPoints];
			SDL_RenderDrawLine(renderer, current.GetX(), current.GetY(), next.GetX(), next.GetY());
		}
	}

	bool PointInside(const Point point) {
		int numPoints = points.size();
		if (numPoints < 3) {
			// Polygon must have at least 3 points
			return false;
		}

		// Initialize a ray outside of the polygon
		Point rayStart(point.GetX(), point.GetY() - 1);
		int intersections = 0;

		// Iterate through each edge of the polygon
		for (int i = 0; i < numPoints; i++) {
			Point current = points[i];
			Point next = points[(i + 1) % numPoints];

			// Check if the ray intersects with the current edge
			if ((current.GetY() > point.GetY()) != (next.GetY() > point.GetY()) &&
				point.GetX() < ((next.GetX() - current.GetX()) * (point.GetY() - current.GetY()) / (next.GetY() - current.GetY()) + current.GetX())) {
				intersections++;
			}
		}

		// If the number of intersections is odd, the point is inside the polygon
		return (intersections % 2) == 1;

	}

	bool IsConvex() {
		int numPoints = points.size();

		for (int i = 0; i < numPoints; i++) {
			Point current = points[i];
			Point previous = points[(i + numPoints - 1) % numPoints];
			Point next = points[(i + 1) % numPoints];

			int crossProduct = (previous.GetX() - current.GetX()) * (next.GetY() - current.GetY()) -
				(previous.GetY() - current.GetY()) * (next.GetX() - current.GetX());

			if (crossProduct < 0) {
				return false;
			}
		}

		return true;
	}

	void Reset() { points.clear(); }

private:
	std::vector<Point> points;
};


void DrawSeparationLine(SDL_Renderer* renderer, Polygon pol1, Polygon pol2) {
	// Draw separation line code
}

int main() {
	SDL_SetMainReady();

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("SDL2 Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, APP_WIDTH, APP_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		// Handle window creation error
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		// Handle renderer creation error
		return 1;
	}


	bool isRunning = true;

	Polygon pol1 = Polygon();
	Polygon pol2 = Polygon();

	bool firstPolygon = true;
	Uint8 r = 255, g = 0, b = 0;

	while (isRunning)
	{
		SDL_Event event;
		SDL_PollEvent(&event);


		switch (event.type) {
		case SDL_QUIT:
			isRunning = false;
			break;

		case SDL_KEYDOWN:
			if (event.button.button == SDLK_SPACE) {
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				firstPolygon = true;
				r = 255, g = 0, b = 0;

				pol1.Reset();
				pol2.Reset();

				SDL_RenderClear(renderer);
			}
			else if (event.button.button == SDLK_a) {
				DrawSeparationLine(renderer, pol1, pol2);
			}


			break;

		case SDL_MOUSEBUTTONUP:
			// Handle mouse button up event
			if (event.button.button == SDL_BUTTON_LEFT) {
				int mouseX = event.button.x;
				int mouseY = event.button.y;

				if (firstPolygon)
					pol1.AddPoint(Point(mouseX, mouseY));
				else {
					Point p = Point(mouseX, mouseY);
					if (!pol1.PointInside(p))
						pol2.AddPoint(p);

					std::cout << "Point is inside" << pol1.PointInside(p) << std::endl;
				}

				SDL_SetRenderDrawColor(renderer, r, g, b, 255); // Set the color values

				SDL_RenderDrawPoint(renderer, mouseX, mouseY); 
				for (int i = 1; i < POINT_SIZE; i++) {
					SDL_RenderDrawPoint(renderer, mouseX + i, mouseY + i);
					SDL_RenderDrawPoint(renderer, mouseX + i, mouseY - i);
					SDL_RenderDrawPoint(renderer, mouseX - i, mouseY - i);
					SDL_RenderDrawPoint(renderer, mouseX - i, mouseY + i);
				}
			}
			else if (event.button.button == SDL_BUTTON_RIGHT)
			{
				if ((!pol1.IsConvex() && firstPolygon) || !pol2.IsConvex())
					continue;

				if (firstPolygon) {
					firstPolygon = false;
					r = 0;
					g = 255;
					pol1.Render(renderer);
					std::cout << "Polygon is convex : " << pol1.IsConvex() << std::endl;
				}
				else {
					pol2.Render(renderer);
					std::cout << "Polygon is convex : " << pol2.IsConvex() << std::endl;
				}
			}
			break;
			
		default:
			break;
		}
		// Render all objects here

		SDL_RenderPresent(renderer); // Update the window to show the changes
	}




	std::cout << "Hello world" << std::endl;

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
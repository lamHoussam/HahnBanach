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

typedef struct Line {
public:
	Line(Point _p1, Point _p2) : p1(_p1), p2(_p2){}

	inline Point GetPoint1() { return p1; }
	inline Point GetPoint2() { return p2; }


private:
	Point p1, p2;
};

typedef struct Polygon {
public:
	Polygon(){}

	inline int GetNumOfPoints() { return points.size(); }
	const inline Point GetPoint(int ind) { return points[ind]; }


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
 			return false;
		}

		Point rayStart(point.GetX(), point.GetY() - 1);
		int intersections = 0;

		for (int i = 0; i < numPoints; i++) {
			Point current = points[i];
			Point next = points[(i + 1) % numPoints];

			if ((current.GetY() > point.GetY()) != (next.GetY() > point.GetY()) &&
				point.GetX() < ((next.GetX() - current.GetX()) * (point.GetY() - current.GetY()) / (next.GetY() - current.GetY()) + current.GetX())) {
				intersections++;
			}
		}

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


int orientation(Point p, Point q, Point r) {
	double val = (q.GetY() - p.GetY()) * (r.GetX() - q.GetX()) - (q.GetX() - p.GetX()) * (r.GetY() - q.GetY());
	if (val == 0) return 0; 
	return (val > 0) ? 1 : 2; 
}

bool doIntersect(Point p1, Point q1, Point p2, Point q2) {
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	if (o1 != o2 && o3 != o4) return true;

	// Special cases: segments are collinear and overlapping
	if (o1 == 0 && orientation(p1, p2, q1) == 0 && orientation(p1, q2, q1) == 0)
		return true;
	if (o2 == 0 && orientation(p1, p2, q2) == 0 && orientation(p1, q1, q2) == 0)
		return true;
	if (o3 == 0 && orientation(p2, p1, q2) == 0 && orientation(p2, q1, q2) == 0)
		return true;
	if (o4 == 0 && orientation(p2, p1, q1) == 0 && orientation(p2, q2, q1) == 0)
		return true;

	return false; 
}


void DrawSmallestSegmentBetween(SDL_Renderer* renderer, Polygon pol1, Polygon pol2) {

	Point p1 = pol1.GetPoint(0);
	Point p2 = pol2.GetPoint(0);
	for (int i = 0; i < pol1.GetNumOfPoints(); ++i) {
		int j = (i + 1) % pol1.GetNumOfPoints();
		for (int k = 0; k < pol2.GetNumOfPoints(); ++k) {
			int l = (k + 1) % pol2.GetNumOfPoints();
			if (!doIntersect(pol1.GetPoint(i), pol1.GetPoint(j), pol2.GetPoint(k), pol2.GetPoint(l))) {
				p1 = pol1.GetPoint(i);
				p2 = pol2.GetPoint(k);

				std::cout << "Found" << std::endl;
				SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
				SDL_RenderDrawLine(renderer, p1.GetX(), p1.GetY(), p2.GetX(), p2.GetY());

				return;
			}
		}
	}

}

void DrawSeparationLine(SDL_Renderer* renderer, Polygon pol1, Polygon pol2) {

}

int main() {
	SDL_SetMainReady();

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* window = SDL_CreateWindow("SDL2 Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, APP_WIDTH, APP_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
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
		SDL_Event e;
		SDL_PollEvent(&e);


		switch (e.type) {
		case SDL_QUIT:
			isRunning = false;
			break;

		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_SPACE) {
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
				firstPolygon = true;
				r = 255, g = 0, b = 0;

				pol1.Reset();
				pol2.Reset();

				SDL_RenderClear(renderer);
			}
			if (e.key.keysym.sym == SDLK_a) {
				std::cout << "Draw Separation Line" << std::endl;
				DrawSmallestSegmentBetween(renderer, pol1, pol2);
			}


			break;

		case SDL_MOUSEBUTTONUP:
			if (e.button.button == SDL_BUTTON_LEFT) {
				int mouseX = e.button.x;
				int mouseY = e.button.y;

				if (firstPolygon)
					pol1.AddPoint(Point(mouseX, mouseY));
				else {
					Point p = Point(mouseX, mouseY);
					if (!pol1.PointInside(p))
						pol2.AddPoint(p);

					std::cout << "Point is inside" << pol1.PointInside(p) << std::endl;
				}

				SDL_SetRenderDrawColor(renderer, r, g, b, 255); 

				SDL_RenderDrawPoint(renderer, mouseX, mouseY); 
				for (int i = 1; i < POINT_SIZE; i++) {
					SDL_RenderDrawPoint(renderer, mouseX + i, mouseY + i);
					SDL_RenderDrawPoint(renderer, mouseX + i, mouseY - i);
					SDL_RenderDrawPoint(renderer, mouseX - i, mouseY - i);
					SDL_RenderDrawPoint(renderer, mouseX - i, mouseY + i);
				}
			}
			else if (e.button.button == SDL_BUTTON_RIGHT)
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

		SDL_RenderPresent(renderer); 
	}




	std::cout << "Hello world" << std::endl;

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
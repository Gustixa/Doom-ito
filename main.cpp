#include "src/texture.hpp"
#include "src/include.hpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

array<bool, 5> input = array<bool, 5>();

const uint16_t RESX = 1440;
const uint16_t RESY = 810;
const uint8_t  SUBSAMPLE = 3;

double update_time = 0.2;
double window_time = 0;
double delta_time = 0;
double run_time = 0;
clock_t last_time = 0;
clock_t current_time = 0;

vec2  player_pos   = vec2(2.0f);
float player_angle = 0.0f;

map<string, Texture> texture_map;

vector<vector<int>> player_map = {
		{1,1,1,1,1,1,1,1,1,1},
		{1,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,1},
		{1,0,0,1,1,0,1,0,0,1},
		{1,0,0,1,0,0,1,0,0,1},
		{1,0,0,1,0,0,1,0,0,1},
		{1,0,0,1,0,1,1,0,0,1},
		{1,0,0,0,0,0,0,0,0,1},
		{1,0,0,0,0,0,0,0,0,1},
		{1,1,1,1,1,1,1,1,1,1},
};

void init() {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Samuel | 60.00 FPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RESX, RESY, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void renderPixel(const uint16_t& i_x, const uint16_t& i_y, const vec3& i_color) {
	SDL_SetRenderDrawColor(renderer, Uint8(i_color.r * 255.f), Uint8(i_color.g * 255.f), Uint8(i_color.b * 255.f), 255);
	//for (int x = 0; x < SUBSAMPLE - 1; x++)
		//for (int y = 0; y < SUBSAMPLE - 1; y++)
			SDL_RenderDrawPoint(renderer, i_x, i_y);
}

void movePlayer(const int& i_fwd, const int& i_side) {
	player_pos.x += i_fwd * cos(player_angle * DEG_RAD) * delta_time;
	player_pos.y += i_fwd * sin(player_angle * DEG_RAD) * delta_time;
	player_pos.x += i_side * cos((player_angle + 90.f) * DEG_RAD) * delta_time;
	player_pos.y += i_side * sin((player_angle + 90.f) * DEG_RAD) * delta_time;
}

void render() {
	current_time = clock();
	delta_time = float(current_time - last_time) / CLOCKS_PER_SEC;
	last_time = current_time;
	run_time += delta_time;
	window_time += delta_time;

	if (window_time > update_time) {
		window_time -= update_time;
		double fps = 1.0 / delta_time;
		stringstream stream;
		stream << "Samuel | " << fixed << setprecision(2) << fps << " FPS | " << player_pos.x << " x " << player_pos.y << " | " << player_angle;
		try { SDL_SetWindowTitle(window, stream.str().c_str()); }
		catch (...) {}
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	if (input[KEY_W]) movePlayer(  1,  0 );
	if (input[KEY_S]) movePlayer( -1,  0 );
	if (input[KEY_A]) movePlayer(  0,  1 );
	if (input[KEY_D]) movePlayer(  0, -1 );

	float ray_angle = player_angle - 30.0f;
	for (int x = 0; x < RESX; x ++) {
		ray_angle += x / (RESX);
		float rayCos = cos(ray_angle * DEG_RAD);
		float raySin = sin(ray_angle * DEG_RAD);
		float ray_x = player_pos.x;
		float ray_y = player_pos.y;

		int wall = 0;
		while (wall == 0) {
			ray_x += rayCos;
			ray_y += raySin;
			wall = player_map[floor(ray_y)][floor(ray_x)];
		}
		float distance = sqrt(pow(player_pos.x - ray_x, 2) + pow(player_pos.y - ray_y, 2));
		uint16_t wallheight = floor((RESY / (2)) / distance);

		for (int y = 0; y < RESY; y ++) {
			if (y > 0 && y < (RESY / (2)) - wallheight) { // Floor
				renderPixel(x, y, vec3(0.5));
			}
			else if (y >= (RESY / (2)) - wallheight && y < (RESY / (2)) + wallheight) { // Wall
				renderPixel(x, y, vec3(1.0));
			}
			else { // Ceiling
				renderPixel(x, y, vec3(0.0));
			}
		}
	}

	SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
	init();
	bool running = true;

	// Load.png
	texture_map["Wall"]      = Texture("./res/Wall.png");
	texture_map["Floor"]     = Texture("./res/Floor.png");
	texture_map["Cacodemon"] = Texture("./res/Cacodemon.png");

	while (running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				running = false;
			else if (event.type == SDL_KEYDOWN)
				input[getKey(event.key.keysym.sym)] = true;
			else if (event.type == SDL_KEYUP) {
				input[getKey(event.key.keysym.sym)] = false;
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					SDL_Quit();
					return 0;
				}
			}
			else if (event.type == SDL_MOUSEMOTION) {
				player_angle += event.motion.x * delta_time * 0.0001f;
			}
		}
		render();
	}
	return 0;
}
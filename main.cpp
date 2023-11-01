#include "src/texture.hpp"
#include "src/include.hpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

array<bool, 9> input = array<bool, 9>();

const float WALK_SPEED = 50.0f;
const float SPRINT_SPEED = WALK_SPEED * 2.0f;
const float TURN_SPEED = 40.0f;

const uint16_t WIDTH = 16;
const uint16_t HEIGHT = 11;
const uint16_t BLOCK = 50;
const uint16_t RESX = WIDTH * BLOCK;
const uint16_t RESY = HEIGHT * BLOCK;

const uint16_t HALF_RESY = RESY / 2;
const uint16_t HALF_BLOCK = BLOCK / 2;

double update_time = 0.25;
double window_time = 0;
double delta_time = 0;
double run_time = 0;
clock_t last_time = 0;
clock_t current_time = 0;

float player_angle = 0.0f;

vector<vector<string>> player_map = {
	{ W1, W2, W2, W1, W2, W2, W1, W2, W2, W1, W2, W2, W1, W2, W2, W1 },
	{ W3, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, OO, W4, OO, OO, W3 },
	{ W1, OO, OO, W1, W2, W2, W1, W2, W2, W1, OO, OO, W1, OO, OO, W1 },
	{ W3, OO, OO, W3, OO, OO, OO, OO, OO, W3, OO, OO, OO, OO, OO, W4 },
	{ W1, OO, OO, W1, W2, W2, W1, OO, OO, W1, W2, W2, W1, OO, OO, W1 },
	{ W3, OO, OO, OO, OO, OO, W3, OO, OO, OO, OO, OO, W3, OO, OO, W4 },
	{ W1, W2, W2, W1, OO, OO, W1, W2, W2, W1, OO, OO, W1, OO, OO, W1 },
	{ W3, OO, OO, OO, OO, OO, W3, OO, OO, W3, OO, OO, OO, OO, OO, W4 },
	{ W1, OO, OO, W1, W2, W2, W1, OO, OO, W1, OO, OO, W1, W4, W4, W1 },
	{ W3, OO, OO, OO, OO, OO, OO, OO, OO, W3, OO, OO, OO, OO, OO, W5 },
	{ W1, W2, W2, W1, W2, W2, W1, W2, W2, W1, W2, W2, W1, W4, W4, W1 }
};
vec2  player_pos = vec2(BLOCK + HALF_BLOCK);
map<string, Texture> texture_map;

void init() {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Samuel | 60.00 FPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RESX + RESY, RESY, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void renderMinimap(const int& x, const int& y, const string& mapHit) {
	for (int cx = x; cx < x + BLOCK; cx++) {
		for (int cy = y; cy < y + BLOCK; cy++) {
			float tx = (float(cx - x) ) / float(BLOCK);
			float ty = (float(cy - y) ) / float(BLOCK);

			vec3 c = texture_map[mapHit].getColor(tx, ty);
			SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
			SDL_RenderDrawPoint(renderer, cx, cy);
		}
	}
}

void renderLine(const int& x, const float& h, const string& i_mapHit, const int& i_screen_x) {
	float start = HALF_RESY - h / 2.0f;
	float end = start + h;

	for (int y = start; y < end; y++) {
		const float x_coords = float(i_screen_x) / float(HALF_RESY * 0.5);
		const float y_coords = mapRange(y, start, end, 0.f, 1.f);
		uvec4 c = texture_map[i_mapHit].getColor(x_coords, y_coords);
		SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
		SDL_RenderDrawPoint(renderer, (RESX + RESY) - x, y);
	}
}


void movePlayer(const int& i_fwd, const int& i_side) {
	if (input[KEY_SHIFT]) {
		player_pos.x += i_fwd * SPRINT_SPEED * cos(player_angle * DEG_RAD) * delta_time;
		player_pos.y += i_fwd * SPRINT_SPEED * sin(player_angle * DEG_RAD) * delta_time;
		player_pos.x += i_side * SPRINT_SPEED * cos((player_angle + 90.f) * DEG_RAD) * delta_time;
		player_pos.y += i_side * SPRINT_SPEED * sin((player_angle + 90.f) * DEG_RAD) * delta_time;
	}
	else {
		player_pos.x += i_fwd * WALK_SPEED * cos(player_angle * DEG_RAD) * delta_time;
		player_pos.y += i_fwd * WALK_SPEED * sin(player_angle * DEG_RAD) * delta_time;
		player_pos.x += i_side * WALK_SPEED * cos((player_angle + 90.f) * DEG_RAD) * delta_time;
		player_pos.y += i_side * WALK_SPEED * sin((player_angle + 90.f) * DEG_RAD) * delta_time;
	}
}

tuple<float, string, int> rayCast(const float& i_angle) {
	float d = 0;
	string mapHit;
	int tx;

	while (true) {
		int x = static_cast<int>(player_pos.x + d * cos(i_angle * DEG_RAD));
		int y = static_cast<int>(player_pos.y + d * sin(i_angle * DEG_RAD));

		int i = x / BLOCK;
		int j = y / BLOCK;

		if (player_map[j][i] != " ") {
			mapHit = player_map[j][i];

			int hitx = x - i * BLOCK;
			int hity = y - j * BLOCK;
			int maxhit;

			if (hitx == 0 || hitx == BLOCK - 1) 
				maxhit = hity;
			else 
				maxhit = hitx;

			tx = maxhit * 128 / BLOCK;
			break;
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderDrawPoint(renderer, x, y);

		d += 1;
	}
	return make_tuple( d, mapHit, tx );
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
	if (input[KEY_A]) movePlayer(  0, -1 );
	if (input[KEY_D]) movePlayer(  0,  1 );
	if (input[KEY_L_ARROW]) player_angle -= TURN_SPEED * delta_time;
	if (input[KEY_R_ARROW]) player_angle += TURN_SPEED * delta_time;

	// render Minimap
	for (int x = 0; x < RESX; x += BLOCK) {
		for (int y = 0; y < RESY; y += BLOCK) {
			int i = static_cast<int>(x / BLOCK);
			int j = static_cast<int>(y / BLOCK);

			if (player_map[j][i] != " ") {
				string mapHit;
				mapHit = player_map[j][i];
				uvec4 c = uvec4(255, 0, 0, 255);
				renderMinimap(x, y, mapHit);
			}
		}
	}

	// render Walls
	for (int x = 0; x < RESY; x++) {
		double a = player_angle + 30.0 - 60.0 * x / RESY;
		auto[dist, mapHit, tx] = rayCast(a);
		uvec4 c = uvec4(255, 0, 0, 255);

		if (dist <= 0.01) {
			cout << "you lose";
			player_pos = vec2(BLOCK + HALF_BLOCK);
		}
		float h = static_cast<float>(RESY) / dist * 50.0f;
		renderLine(x, h, mapHit, tx);
	}

	SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
	init();
	bool running = true;

	// Load.png
	texture_map[W1] = Texture("./res/Wall.png");
	texture_map[W2] = Texture("./res/HH.png");
	texture_map[W3] = Texture("./res/SS.png");
	texture_map[W4] = Texture("./res/HH.png");
	texture_map[W5] = Texture("./res/HH.png");

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
		}
		render();
	}
	return 0;
}
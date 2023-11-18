#include "src/texture.hpp"
#include "src/include.hpp"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* frame_buffer = nullptr;
uint32_t* pixel_buffer = nullptr;

sf::Music music;
sf::SoundBuffer buffer;
sf::Sound sound;


int fire_animation = 0;

array<bool, 9> input = array<bool, 9>();

const float WALK_SPEED = 80.0f;
const float SPRINT_SPEED = WALK_SPEED * 2.4f;
const float TURN_SPEED = 120.0f;

const uint16_t WIDTH = 16;
const uint16_t HEIGHT = 11;
const uint16_t BLOCK = 60;
const uint16_t RESX = WIDTH * BLOCK;
const uint16_t RESY = HEIGHT * BLOCK;

const uint16_t QUART_RESX = RESX / 4;
const uint16_t QUART_RESY = RESY / 4;
const uint16_t QUART_BLOCK = BLOCK / 4;

const uint16_t HALF_RESX = RESX / 2;
const uint16_t HALF_RESY = RESY / 2;
const uint16_t HALF_BLOCK = BLOCK / 2;

double update_time = 0.25;
double window_time = 0;
double delta_time = 0;
double run_time = 0;
clock_t last_time = 0;
clock_t current_time = 0;
double animation_time = 0.0f;

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
vec2 last_player_pos = vec2(BLOCK + HALF_BLOCK);
vec2 player_pos = vec2(BLOCK + HALF_BLOCK);
map<string, Texture> texture_map;

void init() {
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Samuel | 60.00 FPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, RESX, RESY, SDL_WINDOW_SHOWN);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	frame_buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, RESX, RESY);

	music.openFromFile("./res/Deutsch.ogg");
	music.setLoop(true);
	music.play();
	buffer.loadFromFile("./res/Shot.wav");
	sound.setBuffer(buffer);
}

void renderPixel(const uint32_t& x, const uint32_t& y, const uint32_t& i_color) {
	if (y >= 0 && y < RESY && x >= 0 && x < RESX)
		pixel_buffer[y * RESX + x] = i_color;
}

void renderMinimap(const int& x, const int& y, const string& mapHit) {
	for (int cx = x; cx < x + QUART_BLOCK; cx++) {
		for (int cy = y; cy < y + QUART_BLOCK; cy++) {
			float tx = (float(cx - x) ) / float(QUART_BLOCK);
			float ty = (float(cy - y) ) / float(QUART_BLOCK);

			vec3 c = texture_map[mapHit].getColor(tx, ty);
			renderPixel(cx + RESX - QUART_RESX, cy + RESY - QUART_RESY, rgba(c.r, c.g, c.b));
		}
	}
}

void renderLine(const int& x, const float& h, const string& i_mapHit, const int& i_screen_x) {
	float start = HALF_RESY - h / 2.0f;
	float end = start + h;

	for (int y = start; y < end; y++) {
		const float x_coords = float(i_screen_x) / float(HALF_RESX) * 8.0;
		const float y_coords = mapRange(static_cast<float>(y), start, end, 0.f, 1.f);
		uvec4 c = texture_map[i_mapHit].getColor(x_coords, y_coords);
		renderPixel(RESX - x, y, rgba(c.r,c.g,c.b));
	}
}

void renderShotgun() {
	if (animation_time >= 0.0)
		animation_time -= delta_time;

	if (inRange(animation_time, 0.6, 1.20))
		fire_animation = 6;
	if (inRange(animation_time, 0.5, 0.6))
		fire_animation = 5;
	if (inRange(animation_time, 0.4, 0.5))
		fire_animation = 4;
	if (inRange(animation_time, 0.3, 0.4))
		fire_animation = 3;
	if (inRange(animation_time, 0.2, 0.3))
		fire_animation = 2;
	if (inRange(animation_time, 0.1, 0.2))
		fire_animation = 1;
	if (inRange(animation_time,-1.0, 0.1))
		fire_animation = 0;

	uvec4 c = uvec4(255, 0, 255, 255);
	for (int x = 0; x < 320; x++) {
		for (int y = 0; y < 320; y++) {
			float x_coord = static_cast<float>(x) / 320.f;
			float y_coord = static_cast<float>(y) / 320.f;

			if (fire_animation == 6)  c = texture_map["Shotgun Fire A"  ].getColor(x_coord, y_coord);
			if (fire_animation == 5)  c = texture_map["Shotgun Reload A"].getColor(x_coord, y_coord);
			if (fire_animation == 4)  c = texture_map["Shotgun Reload B"].getColor(x_coord, y_coord);
			if (fire_animation == 3)  c = texture_map["Shotgun Reload C"].getColor(x_coord, y_coord);
			if (fire_animation == 2)  c = texture_map["Shotgun Reload B"].getColor(x_coord, y_coord);
			if (fire_animation == 1)  c = texture_map["Shotgun Reload A"].getColor(x_coord, y_coord);
			if (fire_animation == 0)  c = texture_map["Shotgun Rest"    ].getColor(x_coord, y_coord);
			if (c.a != 0) renderPixel(x + QUART_RESX, y + RESY - 320, rgba(c.r, c.g, c.b));
		}
	}
}

void shoot() {
	if (fire_animation == 0) {
		fire_animation = 6;
		animation_time = 0.71;
		sound.play();
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

			tx = maxhit;
			break;
		}
		renderPixel(mapRange(x, 0, RESX, RESX - QUART_RESX, RESX), mapRange(y, 0, RESY, RESY - QUART_RESY, RESY), rgba(255,255,255));
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

	if (input[KEY_W]) movePlayer(  1,  0 );
	if (input[KEY_S]) movePlayer( -1,  0 );
	if (input[KEY_A]) movePlayer(  0, -1 );
	if (input[KEY_D]) movePlayer(  0,  1 );
	if (input[KEY_L_ARROW]) player_angle -= TURN_SPEED * delta_time;
	if (input[KEY_R_ARROW]) player_angle += TURN_SPEED * delta_time;

	int32_t pitch = 0;

	// Lock the memory in order to write our Back Buffer image to it
	if (!SDL_LockTexture(frame_buffer, NULL, (void**)&pixel_buffer, &pitch)) {
		pitch /= sizeof(uint32_t);

		// render Clear
		for (uint32_t i = 0; i < RESX * RESY; i++)
			pixel_buffer[i] = rgba(0,0,0);
		// render Walls
		for (int x = 0; x < RESX; x++) {
			float a = player_angle + 30.0f - 60.0f * x / RESX;
			auto [dist, mapHit, tx] = rayCast(a);
			uvec4 c = uvec4(255, 0, 0, 255);
			if (dist <= 2.5) {
				player_pos = last_player_pos;
			}
			float h = static_cast<float>(RESY) / dist * static_cast<float>(BLOCK);
			renderLine(x, h, mapHit, tx);
		}

		// render Minimap
		for (int x = 0; x < QUART_RESX; x += QUART_BLOCK) {
			for (int y = 0; y < QUART_RESY; y += QUART_BLOCK) {
				int i = static_cast<int>(x / QUART_BLOCK);
				int j = static_cast<int>(y / QUART_BLOCK);
				if (player_map[j][i] != " ") {
					string mapHit;
					mapHit = player_map[j][i];
					uvec4 c = uvec4(255, 0, 0, 255);
					renderMinimap(x, y, mapHit);
				}
			}
		}
		// render Shotty
		renderShotgun();

		last_player_pos = player_pos;

		SDL_UnlockTexture(frame_buffer);
		SDL_RenderCopy(renderer, frame_buffer, NULL, NULL);
		SDL_RenderPresent(renderer);
	}
}

int main(int argc, char* argv[]) {
	init();
	bool running = true;

	// Load.png
	texture_map[W1] = Texture("./res/Wall.png");
	texture_map[W2] = Texture("./res/HH.png");
	texture_map[W3] = Texture("./res/SS.png");
	texture_map[W4] = Texture("./res/Cacodemon.png");
	texture_map[W5] = Texture("./res/HH.png");

	texture_map["Shotgun Rest"]     = Texture("./res/Shotgun Rest.png");
	texture_map["Shotgun Reload A"] = Texture("./res/Shotgun Reload A.png");
	texture_map["Shotgun Reload B"] = Texture("./res/Shotgun Reload B.png");
	texture_map["Shotgun Reload C"] = Texture("./res/Shotgun Reload C.png");
	texture_map["Shotgun Fire A"]   = Texture("./res/Shotgun Fire A.png");
	texture_map["Shotgun Fire B"]   = Texture("./res/Shotgun Fire B.png");

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
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_SPACE) {
					shoot();
				}
			}
		}
		render();
	}
	return 0;
}
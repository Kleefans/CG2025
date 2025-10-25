#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>

#define WIDTH 320
#define HEIGHT 240
std::vector<float> interpolateSingleFloats(float, float, int);
std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfSteps);

void draw(DrawingWindow &window) {
	window.clearPixels();
	auto leftStartList = interpolateThreeElementValues(
		glm::vec3(255.0, 0.0, 0.0),
		glm::vec3(255.0, 255.0, 0.0),
		window.height);
	auto rightStartList = interpolateThreeElementValues(
		glm::vec3(0.0, 0.0, 255.0),
		glm::vec3(0.0, 255.0, 0.0),
		window.height);
	
	for (size_t y = 0; y < window.height; y++) {
		auto ColurList = interpolateThreeElementValues(
			leftStartList[y],
			rightStartList[y],
			window.width);
		for (size_t x = 0; x < window.width; x++) {
			float red = ColurList[x].r;
			float green = ColurList[x].g;
			float blue = ColurList[x].b;
			uint32_t colour = (255 << 24) + (int(red) << 16) + (int(green) << 8) + int(blue);
			window.setPixelColour(x, y, colour);
		}
	}
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

std::vector<float> interpolateSingleFloats(float from, float to, int numberOfSteps) {
	float step = (to - from) / (float(numberOfSteps) - 1);
	std::vector<float> result;
	for (int i = 0; i < numberOfSteps; i++) {
		result.push_back(from + step * float(i));
	}
	return result;
}	

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfSteps) {
	glm::vec3 step = (to - from) / (float(numberOfSteps) - 1);
	std::vector<glm::vec3> result;
	for (int i = 0; i < numberOfSteps; i++) {
		result.push_back(from + step * float(i));
	}
	return result;
}

void testInterpolateSingleFloats() {
	std::vector<float> result;
	result = interpolateSingleFloats(2.2, 8.5, 7);
	for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
	std::cout << std::endl;
}

void testInterpolateThreeElementValues() {
	glm::vec3 from(1.0, 4.0, 9.2);
	glm::vec3 to(4.0, 1.0, 9.8);
	auto result = interpolateThreeElementValues(from, to, 4);
	for (size_t i = 0; i < result.size(); i++) {
		std::cout << (float)result[i].x << " " << (float)result[i].y << " " << (float)result[i].z << std::endl;
	}
}

void test() {
	testInterpolateSingleFloats();
	testInterpolateThreeElementValues();
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;
	test();
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		draw(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <imgui.h>
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_sdlrenderer.h"
#include <SDL2/SDL.h>

#include <array>
#include <cstdint>

struct Point
{
	int x = 0;
	int y = 0;

	[[nodiscard]]
	constexpr Point midpoint(const Point rhs) const
	{
		return Point{(x + rhs.x) / 2, (y + rhs.y) / 2};
	}
};

struct Pixel
{
	std::uint8_t red = 0;
	std::uint8_t green = 0;
	std::uint8_t blue = 0;
};

// note that signature matters
auto main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) -> int
{
	fmt::print("Primary output has been initialized successfully\n");

	spdlog::info("Logger has been initialized successfully");
	// Setup SDL
	// (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
	// depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// Setup window
	constexpr std::size_t window_dimension_y = 720, window_dimension_x = 1280;
	auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	SDL_Window* window = SDL_CreateWindow("I am severely depressed and i need help", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_dimension_x, window_dimension_y, window_flags);

	// Setup SDL_Renderer instance
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr)
	{
		SDL_Log("Error creating SDL_Renderer!");
		return false;
	}
	//SDL_RendererInfo info;
	//SDL_GetRendererInfo(renderer, &info);
	//SDL_Log("Current SDL_Renderer: %s", info.name);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer_Init(renderer);

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	// Our state

	ImVec4 clear_color = ImVec4(0x1C/255.0, 0x0D/255.0, 0x0D/255.0, 1.00f);

	std::int64_t iterations = 100'000'000;

	Point edges[] = {{0,799}, {799, 799}, {400, 0}};
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	Point current_point = {rand() % 800, rand() % 800};

	using Canvas = std::array<std::array<Pixel, 800>, 800>;
	std::unique_ptr<Canvas> canvas_ptr = std::make_unique<Canvas>();

	auto* texture = SDL_CreateTexture(renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGB24, SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING, 800, 800);
	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
				done = true;
		}

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::Begin("Rendering Demo");

			ImGui::Text("This should directly render to background");

			ImGui::Image(texture, {800, 800});

			ImGui::End();
		}


		if(iterations > 0)
		{
			Canvas& canvas = *canvas_ptr;

			for (int i = 0; i < 100; ++i)
			{
				current_point = edges[rand() % 3].midpoint(current_point);
				canvas[current_point.y][current_point.x] = {255, 0, 0};
			}

			iterations -= 100;
		}


		SDL_UpdateTexture(texture, nullptr, canvas_ptr.get(), 800 * 3);

		// Rendering
		ImGui::Render();
		SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer);
	}

	// Cleanup
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;

}
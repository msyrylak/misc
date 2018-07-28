#include "SOC.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl2.h"
#include "imgui_memory_editor.h"
#include <stdio.h>
#include <iostream>

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Error %d: %s\n", error, description);
}

int main()
{
	SOC test;
	test.Reset();
	test.Run(6);

	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if(!glfwInit())
		return 1;
	GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui GLFW+OpenGL2 example", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

						 // Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui_ImplGlfwGL2_Init(window, true);

	// Setup style
	ImGui::StyleColorsDark();

	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		ImGui_ImplGlfwGL2_NewFrame();

			static MemoryEditor mem_edit_2;
			ImGui::SetWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
			ImGui::Begin("Memory");
			mem_edit_2.DrawContents(test.myMem, 65536, 0x0000);
			ImGui::End();

		// Rendering
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound, but prefer using the GL3+ code.
		ImGui::Render();
		ImGui_ImplGlfwGL2_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);

	}

	// Cleanup
	ImGui_ImplGlfwGL2_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
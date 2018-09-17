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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main()
{
	SOC test;
	uint8_t* mem_copy = new uint8_t[65536];


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

			//ImGui::SetWindowPos(ImVec2(400, 400), ImGuiCond_FirstUseEver);
			ImGui::Begin("Menu");
			ImGui::Text("Welcome!");
			if(ImGui::Button("Load"))
			{
				test.LoadMemory(test.myMem, 65536);
				memcpy(mem_copy, test.myMem, 65536);
			}
			if(ImGui::Button("Save"))
			{
				test.SaveMemory(test.myMem, 65536);
			}
			if(ImGui::Button("Reset"))
			{
				test.Reset();
			}
			if(ImGui::Button("Run"))
			{
				test.changes.clear();
				memcpy(mem_copy, test.myMem, 65536);
				test.Run(10);
			}
			if(ImGui::Button("Step"))
			{
				test.changes.clear();
				memcpy(mem_copy, test.myMem, 65536);
				test.Run(1);
			}
			if(ImGui::Button("Exit"))
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			ImGui::End();

	

			static MemoryEditor mem_edit_2;
			ImGui::SetWindowPos(ImVec2(200, 100), ImGuiCond_FirstUseEver);
			ImGui::Begin("Memory");
			mem_edit_2.DrawContents(test.myMem, 65536, &test.changes, mem_copy, 0x0000);
			ImGui::End();

			ImGui::Begin("Registers");
			ImGui::Columns(4, "mycolumns"); 
			ImGui::Separator();
			ImGui::Text("R1"); ImGui::NextColumn();
			ImGui::Text("R2"); ImGui::NextColumn();
			ImGui::Text("R3"); ImGui::NextColumn();
			ImGui::Text("PC"); ImGui::NextColumn();
			ImGui::Separator();
			char temp[32];
			char temp2[32];
			char temp3[32];
			char temp4[32];
			sprintf_s(temp, "%02x", test.R1);
			sprintf_s(temp2, "%02x", test.R2);
			sprintf_s(temp3, "%02x", test.R3);
			sprintf_s(temp4, "%02x", test.PC);
			ImGui::Text(temp); ImGui::NextColumn(); 
			//ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "lol");
			ImGui::Text(temp2); ImGui::NextColumn();
			ImGui::Text(temp3); ImGui::NextColumn();
			ImGui::Text(temp4); ImGui::NextColumn();
			ImGui::Separator();
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
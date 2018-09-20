#include "imgui_memory_editor.h"


// Memory Editor contents only
void MemoryEditor::DrawContents(u8* mem_data, size_t mem_size, std::vector<SOC::changedByte> changesVector, u8* mem_copy, size_t base_display_addr)
{
	Sizes s;
	CalcSizes(s, mem_size, base_display_addr);
	ImGuiStyle& style = ImGui::GetStyle();

	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
	ImGui::BeginChild("##scrolling", ImVec2(0, -footer_height_to_reserve));
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	const int line_total_count = (int)((mem_size + Rows - 1) / Rows);
	ImGuiListClipper clipper(line_total_count, s.LineHeight);
	const size_t visible_start_addr = clipper.DisplayStart * Rows;
	const size_t visible_end_addr = clipper.DisplayEnd * Rows;

	bool data_next = false;

	if(ReadOnly || DataEditingAddr >= mem_size)
		DataEditingAddr = (size_t)-1;

	size_t data_editing_addr_backup = DataEditingAddr;
	size_t data_editing_addr_next = (size_t)-1;
	if(DataEditingAddr != (size_t)-1)
	{
		// Move cursor but only apply on next frame so scrolling with be synchronized (because currently we can't change the scrolling while the window is being rendered)
		if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) && DataEditingAddr >= (size_t)Rows)
		{
			data_editing_addr_next = DataEditingAddr - Rows; DataEditingTakeFocus = true;
		}
		else if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)) && DataEditingAddr < mem_size - Rows)
		{
			data_editing_addr_next = DataEditingAddr + Rows; DataEditingTakeFocus = true;
		}
		else if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)) && DataEditingAddr > 0)
		{
			data_editing_addr_next = DataEditingAddr - 1; DataEditingTakeFocus = true;
		}
		else if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)) && DataEditingAddr < mem_size - 1)
		{
			data_editing_addr_next = DataEditingAddr + 1; DataEditingTakeFocus = true;
		}
	}
	if(data_editing_addr_next != (size_t)-1 && (data_editing_addr_next / Rows) != (data_editing_addr_backup / Rows))
	{
		// Track cursor movements
		const int scroll_offset = ((int)(data_editing_addr_next / Rows) - (int)(data_editing_addr_backup / Rows));
		const bool scroll_desired = (scroll_offset < 0 && data_editing_addr_next < visible_start_addr + Rows * 2) || (scroll_offset > 0 && data_editing_addr_next > visible_end_addr - Rows * 2);
		if(scroll_desired)
			ImGui::SetScrollY(ImGui::GetScrollY() + scroll_offset * s.LineHeight);
	}

	// Draw vertical separator
	ImVec2 window_pos = ImGui::GetWindowPos();
	if(OptShowAscii)
		draw_list->AddLine(ImVec2(window_pos.x + s.PosAsciiStart - s.GlyphWidth, window_pos.y), ImVec2(window_pos.x + s.PosAsciiStart - s.GlyphWidth, window_pos.y + 9999), ImGui::GetColorU32(ImGuiCol_Border));

	const ImU32 color_text = ImGui::GetColorU32(ImGuiCol_Text);
	const ImU32 color_disabled = OptGreyOutZeroes ? ImGui::GetColorU32(ImGuiCol_TextDisabled) : color_text;

	for(int line_i = clipper.DisplayStart; line_i < clipper.DisplayEnd; line_i++) // display only visible lines
	{
		size_t addr = (size_t)(line_i * Rows);
		ImGui::Text("%0*" _PRISizeT ": ", s.AddrDigitsCount, base_display_addr + addr);

		// Draw Hexadecimal
		for(int n = 0; n < Rows && addr < mem_size; n++, addr++)
		{
			float byte_pos_x = s.PosHexStart + s.HexCellWidth * n;
			if(OptMidRowsCount > 0)
				byte_pos_x += (n / OptMidRowsCount) * s.SpacingBetweenMidRows;
			ImGui::SameLine(byte_pos_x);


			// Draw highlight
			if((addr >= HighlightMin && addr < HighlightMax) || (HighlightFn && HighlightFn(mem_data, addr)))
			{
				ImVec2 pos = ImGui::GetCursorScreenPos();
				float highlight_width = s.GlyphWidth * 2;
				bool is_next_byte_highlighted = (addr + 1 < mem_size) && ((HighlightMax != (size_t)-1 && addr + 1 < HighlightMax) || (HighlightFn && HighlightFn(mem_data, addr + 1)));
				if(is_next_byte_highlighted || (n + 1 == Rows))
				{
					highlight_width = s.HexCellWidth;
					if(OptMidRowsCount > 0 && n > 0 && (n + 1) < Rows && ((n + 1) % OptMidRowsCount) == 0)
						highlight_width += s.SpacingBetweenMidRows;
				}
				draw_list->AddRectFilled(pos, ImVec2(pos.x + highlight_width, pos.y + s.LineHeight), HighlightColor);
			}


			if(DataEditingAddr == addr)
			{
				// Display text input on current byte
				bool data_write = false;
				ImGui::PushID((void*)addr);
				if(DataEditingTakeFocus)
				{
					ImGui::SetKeyboardFocusHere();
					ImGui::CaptureKeyboardFromApp(true);
					sprintf_s(AddrInputBuf, "%0*" _PRISizeT, s.AddrDigitsCount, base_display_addr + addr);
					sprintf_s(DataInputBuf, "%02X", ReadFn ? ReadFn(mem_data, addr) : mem_data[addr]);
				}
				ImGui::PushItemWidth(s.GlyphWidth * 2);
				struct UserData
				{
					// FIXME: We should have a way to retrieve the text edit cursor position more easily in the API, this is rather tedious. This is such a ugly mess we may be better off not using InputText() at all here.
					static int Callback(ImGuiTextEditCallbackData* data)
					{
						UserData* user_data = (UserData*)data->UserData;
						if(!data->HasSelection())
							user_data->CursorPos = data->CursorPos;
						if(data->SelectionStart == 0 && data->SelectionEnd == data->BufTextLen)
						{
							// When not editing a byte, always rewrite its content (this is a bit tricky, since InputText technically "owns" the master copy of the buffer we edit it in there)
							data->DeleteChars(0, data->BufTextLen);
							data->InsertChars(0, user_data->CurrentBufOverwrite);
							data->SelectionStart = 0;
							data->SelectionEnd = data->CursorPos = 2; //highlight
						}
						return 0;
					}
					char   CurrentBufOverwrite[3];  // Input
					int    CursorPos;               // Output
				};
				UserData user_data;
				user_data.CursorPos = -1;
				sprintf_s(user_data.CurrentBufOverwrite, "%02X", ReadFn ? ReadFn(mem_data, addr) : mem_data[addr]);
				ImGuiInputTextFlags flags = ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_NoHorizontalScroll | ImGuiInputTextFlags_AlwaysInsertMode | ImGuiInputTextFlags_CallbackAlways;
				if(ImGui::InputText("##data", DataInputBuf, 32, flags, UserData::Callback, &user_data))
					data_write = data_next = true;
				else if(!DataEditingTakeFocus && !ImGui::IsItemActive())
					DataEditingAddr = data_editing_addr_next = (size_t)-1;
				DataEditingTakeFocus = false;
				ImGui::PopItemWidth();
				if(user_data.CursorPos >= 2)
					data_write = data_next = true;
				if(data_editing_addr_next != (size_t)-1)
					data_write = data_next = false;
				int data_input_value;
				if(data_write && sscanf_s(DataInputBuf, "%X", &data_input_value) == 1)
				{
					if(WriteFn)
						WriteFn(mem_data, addr, (u8)data_input_value);
					else
						mem_data[addr] = (u8)data_input_value;
				}
				ImGui::PopID();
			}
			else
			{
				// NB: The trailing space is not visible but ensure there's no gap that the mouse cannot click on.
				u8 b = ReadFn ? ReadFn(mem_data, addr) : mem_data[addr];


				if(OptShowHexII)
				{
					if((b >= 32 && b < 128))
						ImGui::Text(".%c ", b);
					else if(b == 0xFF && OptGreyOutZeroes)
						ImGui::TextDisabled("## ");
					else if(b == 0x00)
						ImGui::Text("   ");
					else
					{
						if(!changesVector.empty())
						{
							unsigned i;
							for(i = 0; i < changesVector.size(); i++)
							{
								if(addr == changesVector[i].address)
								{
									if(mem_data[changesVector[i].address] != mem_copy[changesVector[i].address])
									{
										ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%02X ", b);
									}
									else
									{
										ImGui::Text("%02X ", b);
									}
									break;
								}
							}
							if(i == changesVector.size())
							{
								ImGui::Text("%02X ", b);
							}
						}
						else
						{
							ImGui::Text("%02X ", b);
						}
					}
				}
				else
				{
					if(b == 0 && OptGreyOutZeroes)
						ImGui::TextDisabled("00 ");
					else
					{
						if(!changesVector.empty())
						{
							unsigned i;
							for(i = 0; i < changesVector.size(); i++)
							{
								if(addr == changesVector[i].address)
								{
									if(mem_data[changesVector[i].address] != mem_copy[changesVector[i].address])
									{
										ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%02X ", b);
									}
									else
									{
										ImGui::Text("%02X ", b);
									}
									break;
								}
							}
							if(i == changesVector.size())
							{
								ImGui::Text("%02X ", b);
							}
						}
						else
						{
							ImGui::Text("%02X ", b);
						}
					}
				}

				if(!ReadOnly && ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
				{
					DataEditingTakeFocus = true;
					data_editing_addr_next = addr;
				}
			}
		}

		if(OptShowAscii)
		{
			// Draw ASCII values
			ImGui::SameLine(s.PosAsciiStart);
			ImVec2 pos = ImGui::GetCursorScreenPos();
			addr = line_i * Rows;
			ImGui::PushID(line_i);
			if(ImGui::InvisibleButton("ascii", ImVec2(s.PosAsciiEnd - s.PosAsciiStart, s.LineHeight)))
			{
				DataEditingAddr = addr + (size_t)((ImGui::GetIO().MousePos.x - pos.x) / s.GlyphWidth);
				DataEditingTakeFocus = true;
			}
			ImGui::PopID();
			for(int n = 0; n < Rows && addr < mem_size; n++, addr++)
			{
				if(addr == DataEditingAddr)
				{
					draw_list->AddRectFilled(pos, ImVec2(pos.x + s.GlyphWidth, pos.y + s.LineHeight), ImGui::GetColorU32(ImGuiCol_FrameBg));
					draw_list->AddRectFilled(pos, ImVec2(pos.x + s.GlyphWidth, pos.y + s.LineHeight), ImGui::GetColorU32(ImGuiCol_TextSelectedBg));
				}
				unsigned char c = ReadFn ? ReadFn(mem_data, addr) : mem_data[addr];
				char display_c = (c < 32 || c >= 128) ? '.' : c;
				draw_list->AddText(pos, (display_c == '.') ? color_disabled : color_text, &display_c, &display_c + 1);
				pos.x += s.GlyphWidth;
			}
		}
	}
	clipper.End();
	ImGui::PopStyleVar(2);
	ImGui::EndChild();

	if(data_next && DataEditingAddr < mem_size)
	{
		DataEditingAddr = DataEditingAddr + 1;
		DataEditingTakeFocus = true;
	}
	else if(data_editing_addr_next != (size_t)-1)
	{
		DataEditingAddr = data_editing_addr_next;
	}

	ImGui::Separator();

	// Options menu
	if(ImGui::Button("Options"))
		ImGui::OpenPopup("context");
	if(ImGui::BeginPopup("context"))
	{
		ImGui::PushItemWidth(56);
		if(ImGui::DragInt("##rows", &Rows, 0.2f, 4, 32, "%.0f rows")) ContentsWidthChanged = true;
		ImGui::PopItemWidth();
		ImGui::Checkbox("Show HexII", &OptShowHexII);
		if(ImGui::Checkbox("Show Ascii", &OptShowAscii)) ContentsWidthChanged = true;
		ImGui::Checkbox("Grey out zeroes", &OptGreyOutZeroes);
		ImGui::EndPopup();
	}

	ImGui::SameLine();
	ImGui::Text("Range %0*" _PRISizeT "..%0*" _PRISizeT, s.AddrDigitsCount, base_display_addr, s.AddrDigitsCount, base_display_addr + mem_size - 1);
	ImGui::SameLine();
	ImGui::PushItemWidth((s.AddrDigitsCount + 1) * s.GlyphWidth + style.FramePadding.x * 2.0f);
	if(ImGui::InputText("##addr", AddrInputBuf, 32, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
	{
		size_t goto_addr;
		if(sscanf_s(AddrInputBuf, "%" _PRISizeT, &goto_addr) == 1)
		{
			GotoAddr = goto_addr - base_display_addr;
			HighlightMin = HighlightMax = (size_t)-1;
		}
	}
	ImGui::PopItemWidth();

	if(GotoAddr != (size_t)-1)
	{
		if(GotoAddr < mem_size)
		{
			ImGui::BeginChild("##scrolling");
			ImGui::SetScrollFromPosY(ImGui::GetCursorStartPos().y + (GotoAddr / Rows) * ImGui::GetTextLineHeight());
			ImGui::EndChild();
			DataEditingAddr = GotoAddr;
			DataEditingTakeFocus = true;
		}
		GotoAddr = (size_t)-1;
	}

	// Notify the main window of our ideal child content size (FIXME: we are missing an API to get the contents size from the child)
	ImGui::SetCursorPosX(s.WindowWidth);
}
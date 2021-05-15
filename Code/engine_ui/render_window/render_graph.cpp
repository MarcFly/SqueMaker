#include "render_graph.h"

void SQUE_RenderGraph_Messager::RegisterSubscriber()
{
	memcpy(name, "Render_Graph", strlen("Render_Graph") + 1);
	sub_ref = SQUE_MSG_RegisterSubscriber(this);
}

void SQUE_RenderGraph_Messager::DeclareSubjects()
{
	subjects.push_back(SQUE_MSG_DeclareSubject("RenderGraphUpdated"));
}

void SQUE_RenderGraph_Messager::SubscribeToSubjects()
{
	SQUE_MSG_SubscribeToSubject(sub_ref, subjects[0]);
}

void SQUE_RenderGraph::Init()
{
	active = false;
	EngineUI_RegisterItem(this);
	name = "Render_Graph";

	rm_menu.container_name = name;
	rm_menu.flags = ImGuiHoveredFlags_RootAndChildWindows;
	EngineUI_RegisterMessager(&msgr);

	// Set all colors.... have an editor and a way to save them too
	title_color[RENDER_VALUE_VERTEX] = ImVec4(0,.5,0,1);
	title_color[RENDER_VALUE_FRAGMENT] = ImVec4(.5,0,0,1);

	title_hovered_color[RENDER_VALUE_VERTEX] = ImVec4(0,.9, 0,1);
	title_hovered_color[RENDER_VALUE_FRAGMENT] = ImVec4(.9,0,0,1);
	
	title_selected_color[RENDER_VALUE_VERTEX] = ImVec4(0, .7,0,1);
	title_selected_color[RENDER_VALUE_FRAGMENT] = ImVec4(.7,0,0,1);
}

static bool was_opened_node = false;
static bool was_opened = false;
void SQUE_RenderGraph::UpdateRMMenu()
{
	rm_menu.CheckToOpen();
	bool to_open = ImGui::BeginPopup(rm_menu.container_name);
	if (!was_opened_node && to_open)
	{
		if (!was_opened)
		{
			was_opened = true;
			// Check if Clicked while hovering a node
			int node_hovered;
			uint16_t num_nodes = ImNodes::NumSelectedNodes();
			if (num_nodes > 0)
			{
				int* selected = new int[num_nodes];
				ImNodes::GetSelectedNodes(selected);

				if (!was_opened_node)
					was_opened_node = (num_nodes == 1 && ImNodes::IsNodeHovered(&node_hovered) && (*selected) == node_hovered);
				delete selected;
			}
		}
		if (!was_opened_node)
		{
			// Canvas Right Mouse Menu
			if (ImGui::MenuItem("Add Vertex Step"))
			{
				RenderStep* new_step = new RenderStep();
				sprintf(new_step->name, "NewRenderStep");
				new_step->type = RENDER_VALUE_VERTEX;
				new_step->shader_out = Render_GenOutputValue();
				new_step->shader_out.type = RENDER_VALUE_VERTEX;
				Render_AddStep(new_step);
			}
			if (ImGui::MenuItem("Add Fragment Step"))
			{
				RenderStep* new_step = new RenderStep();
				sprintf(new_step->name, "NewRenderStep");
				new_step->type = RENDER_VALUE_FRAGMENT;
				new_step->shader_in = Render_GenInputValue();
				new_step->shader_in.type = RENDER_VALUE_VERTEX;
				Render_AddStep(new_step);
			}
		}
	}

	// Node Right Mouse Menu
	if (was_opened_node)
	{
		uint16_t num_nodes = ImNodes::NumSelectedNodes();
		int* selected = new int[num_nodes];
		ImNodes::GetSelectedNodes(selected);

		if (ImGui::MenuItem("Delete Node"))
		{
			bool test = false;
		}
		
		ImGui::Separator();

		if (ImGui::MenuItem("Add Input"))
		{
			Render_GetStep(selected[0])->input_data.push_back(Render_GenInputValue());
		}
		if (ImGui::MenuItem("Add Output"))
		{
			Render_GetStep(selected[0])->output_data.push_back(Render_GenOutputValue());
		}
		delete selected;
	}
	
	if (to_open) 
		ImGui::EndPopup();
	else
	{
		// It is not slated to open, force close
		was_opened = false;
		was_opened_node = false;
	}

}


void SQUE_RenderGraph::UpdateNodeTitleBar(RenderStep* step)
{
	ImNodes::BeginNodeTitleBar();
	static char id[26];

	if (ImGui::Button(ICON_FK_PENCIL, ImVec2(20, 20))) editing_name = !editing_name;
	ImGui::SameLine();
	if (editing_name)
	{
		sprintf(id, "##NodeTitleID%d", step->id);
		ImGui::PushItemWidth(64 * 3 + 10);
		ImGui::InputText(id, step->name, sizeof(step->name));
		ImGui::PopItemWidth();
	}
	else
		ImGui::Text(step->name);

	if (step->shader_in.type != -1)
	{
		ImNodes::BeginInputAttribute(step->shader_in.id, ImNodesPinShape_QuadFilled);
		ImNodes::EndInputAttribute();
	}
	if (step->shader_out.type != -1)
	{
		ImNodes::BeginOutputAttribute(step->shader_out.id, ImNodesPinShape_QuadFilled);
		ImNodes::EndOutputAttribute();
	}
	ImNodes::EndNodeTitleBar();	
}


void SQUE_RenderGraph::UpdateNodeOptions(RenderStep* step)
{
	// TODO: Change from buttons to another right mouse menu for each node specific
	// Node settings outside of the canvas
	// Smae with name changes, try on a per item basis somehow
}

void SQUE_RenderGraph::UpdateNodeInputs(RenderStep* step)
{
	sque_vec<RenderValue>& inputs = step->input_data;

	static char options[32];
	for (uint16_t i = 0; i < inputs.size(); ++i)
	{
		// Push Color Style

		ImNodes::BeginInputAttribute(inputs[i].id);
		if (editing_name)
		{
			static char id[32];
			sprintf(id, "##%s%d", inputs[i].name, inputs[i].id);
			ImGui::InputText("", inputs[i].name, sizeof(inputs[i].name));
		}
		else
			ImGui::Text(inputs[i].name);
		
		ImGui::SameLine();

		sprintf(options, "##OutValueOption%d", i);
		if(ImGui::BeginCombo(options, RenderValueString[inputs[i].type]))
		{
			bool is_selected;
			for (uint16_t j = RENDER_VALUE_FLOAT; j < RENDER_VALUE_TABLE_NUM_STATES; ++j)
			{
				is_selected = ((j + RENDER_VALUE_FRAGMENT) == inputs[i].type);
				if (ImGui::Selectable(RenderValueString[j], &is_selected))
				{
					inputs[i].type = j;
					for (auto it = links.begin(); it != NULL && it != links.end(); ++it)
					{
						if (it->_data->in_id == step->input_data[i].id)
						{
							links.pop(it);
							break;
						}
					}
					break;
				}
			}
			ImGui::EndCombo();
		}

		ImNodes::EndInputAttribute();

		// TODO: Pop Color Styles
	}	
}

void SQUE_RenderGraph::UpdateNodeOutputs(RenderStep* step)
{
	// Outputs
	static char options[32];
	for (uint16_t i = 0; i < step->output_data.size(); ++i)
	{
		// TODO: Push Color Styles
		ImNodes::BeginOutputAttribute(step->output_data[i].id);

		sprintf(options, "##OutValueOption%d", i);
		if (ImGui::BeginCombo(options, RenderValueString[step->output_data[i].type]))
		{
			bool is_selected;
			for (uint16_t j = RENDER_VALUE_FLOAT; j < RENDER_VALUE_TABLE_NUM_STATES; ++j)
			{
				is_selected = ((j) == step->output_data[i].type);
				if (ImGui::Selectable(RenderValueString[j], &is_selected))
				{
					step->output_data[i].type = j;
					for (auto it = links.begin(); it != NULL && it != links.end(); ++it)
					{
						if (it->_data->in_id == step->output_data[i].id)
						{
							links.pop(it);
							break;
						}
					}
					break;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		if (editing_name)
		{
			static char id[32];
			sprintf(id, "##%s%d", step->output_data[i].name, step->output_data[i].id);
			ImGui::InputText(id, step->output_data[i].name, sizeof(step->output_data[i].name));
		}
		else
			ImGui::Text(step->output_data[i].name);
		ImNodes::EndInputAttribute();
		// TODO: Pop Color styles
	}
}

void SQUE_RenderGraph::Update(float dt)
{
	for (uint16_t i = 0; i < msgr.inbox.size(); ++i)
	{
		// Take Care of messages
	}

	uint16_t num_nodes = ImNodes::NumSelectedNodes();
	int* selected = new int[num_nodes];
	ImNodes::GetSelectedNodes(selected);

	if (ImGui::Begin(name, &active))
	{
		ImNodes::BeginNodeEditor();

		sque_vec<RenderStep*>& steps = Render_GetSteps();
		int item_id = 1;
		for (uint16_t i = 0; i < steps.size(); ++i)
		{
			RenderStep* step = steps[i];
			int node_id = item_id;
			
			ImNodes::PushColorStyle(ImNodesCol_TitleBar, title_color[step->type]);
			ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, title_hovered_color[step->type]);
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, title_selected_color[step->type]);
			
			ImNodes::BeginNode(step->id);
			
			UpdateNodeTitleBar(step);
			UpdateNodeOptions(step);

			ImGui::PushItemWidth(64 * 3 - 20);

			UpdateNodeInputs(step);
			
			ImVec2 node_dimentions = ImNodes::GetNodeDimensions(step->id);
			ImVec2 sep_pos = ImGui::GetCursorScreenPos();
			sep_pos.y += 5;
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddLine(sep_pos, ImVec2(sep_pos.x + node_dimentions.x-20, sep_pos.y), title_color[step->type], 5);
			ImGui::SetCursorPosY(sep_pos.y - 55);
			UpdateNodeOutputs(step);

			//ImGui::Unindent(node_dimentions.x / 3 - 30);

			ImGui::PopItemWidth();
			
			ImNodes::EndNode();

			ImNodes::PopColorStyle();
			ImNodes::PopColorStyle();
			ImNodes::PopColorStyle();
		}
		uint32_t link_num = 0;
		list_node<AttributeLink>* t = links.begin();
		for (list_node<AttributeLink>* iter = links.begin(); iter != links.end(); iter = iter->next, ++link_num)
		{
			ImNodes::Link(link_num, iter->_data->in_id, iter->_data->out_id);
		}

		ImNodes::EndNodeEditor();

		static AttributeLink new_link;
		if (ImNodes::IsLinkCreated(&new_link.in_id, &new_link.out_id))
		{
			
			if (new_link.in_id != new_link.out_id)
			{
				if (CHK_FLAG(new_link.in_id, 1))
				{
					int temp = new_link.in_id;
					new_link.in_id = new_link.out_id;
					new_link.out_id = temp;
				}

				RenderValue* in = Render_GetValue(new_link.in_id);
				RenderValue* out = Render_GetValue(new_link.out_id);

				if (in != NULL && out != NULL && in->type == out->type)
				{
					for (list_node<AttributeLink>* it = links.begin(); it != links.end(); it = it->next)
					{
						if (it->_data->in_id == in->id)
						{
							it->_data->out_id = out->id;
							out = NULL;
							break;
						}
					}
					if(out != NULL) links.push_back(new_link);
				}
			}				
		}
		
		static int detached_link;
		if (ImNodes::IsLinkDestroyed(&detached_link))
		{
			for (auto it = links.begin(); it != NULL && it != links.end(); ++it)
			{
				if (it->_data->out_id == detached_link)
				{
					links.pop(it);
					break;
				}
			}
		}
		

		UpdateRMMenu();
	}
	ImGui::End();
}

void SQUE_RenderGraph::CleanUp()
{

}
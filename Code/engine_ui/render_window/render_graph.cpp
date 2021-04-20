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
	title_color[RENDER_STEP_VERTEX] = ImVec4(0,.5,0,1);
	title_color[RENDER_STEP_FRAGMENT] = ImVec4(.5,0,0,1);

	title_hovered_color[RENDER_STEP_VERTEX] = ImVec4(0,.9, 0,1);
	title_hovered_color[RENDER_STEP_FRAGMENT] = ImVec4(.0,0,0,1);
	
	title_selected_color[RENDER_STEP_VERTEX] = ImVec4(0, .7,0,1);
	title_selected_color[RENDER_STEP_FRAGMENT] = ImVec4(.7,0,0,1);
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
				new_step->type = RENDER_STEP_VERTEX;
				Render_AddStep(new_step);
			}
			if (ImGui::MenuItem("Add Fragment Step"))
			{
				RenderStep* new_step = new RenderStep();
				sprintf(new_step->name, "NewRenderStep");
				new_step->type = RENDER_STEP_FRAGMENT;
				Render_AddStep(new_step);
			}
		}
	}

	// Node Right Mouse Menu
	if (was_opened_node)
	{
		if (ImGui::MenuItem("Delete Node"))
		{
			bool test = false;
		}
		ImGui::Separator();
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


void SQUE_RenderGraph::UpdateNodeTitleBar(RenderStep* step, int& item_id, const int node_id)
{
	

	ImNodes::BeginNodeTitleBar();
	static char id[26];

	if (ImGui::Button(ICON_FK_PENCIL, ImVec2(20, 20))) editing_name = !editing_name;
	ImGui::SameLine();
	if (editing_name)
	{
		sprintf(id, "##NodeTitleID%d", item_id);
		ImGui::PushItemWidth(64 * 3 + 10);
		ImGui::InputText(id, step->name, sizeof(step->name));
		ImGui::PopItemWidth();
	}
	else
		ImGui::Text(step->name);

	ImNodes::BeginInputAttribute(item_id++, ImNodesPinShape_QuadFilled);
	ImNodes::EndInputAttribute();
	ImNodes::BeginOutputAttribute(item_id++, ImNodesPinShape_QuadFilled);
	ImNodes::EndOutputAttribute();

	ImNodes::EndNodeTitleBar();

	
}


void SQUE_RenderGraph::UpdateNodeOptions(RenderStep* step, int& item_id, const int node_id)
{
	// TODO: Change from buttons to another right mouse menu for each node specific
	// Node settings outside of the canvas
	// Smae with name changes, try on a per item basis somehow

	static RenderValue input_def;
	sprintf(input_def.name, "Input Value %d", step->input_data.size());
	if (ImGui::Button(ICON_FK_PLUS"##In", ImVec2(20, 20)))
	{
		step->input_data.push_back(input_def);
	}
	
	ImVec2 node_dimentions = ImNodes::GetNodeDimensions(node_id);
	ImGui::SameLine(node_dimentions.x - 40);

	static RenderValue output_def;
	sprintf(output_def.name, "Output Value %d", step->output_data.size());
	if (ImGui::Button(ICON_FK_PLUS"##Out", ImVec2(20, 20)))
	{
		step->output_data.push_back(output_def);
	}
}

void SQUE_RenderGraph::UpdateNodeInputs(RenderStep* step, int& item_id, const int node_id)
{
	sque_vec<RenderValue>& inputs = step->input_data;

	static char options[32];
	for (uint16_t i = 0; i < inputs.size(); ++i)
	{
		// Push Color Style

		ImNodes::BeginInputAttribute(item_id++);
		if (editing_name)
		{
			static char id[26];
			sprintf(id, "##%s%d", inputs[i].name, item_id);
			ImGui::InputText("", inputs[i].name, sizeof(inputs[i].name));
		}
		else
			ImGui::Text(inputs[i].name);
		
		ImGui::SameLine();

		sprintf(options, "%s##OutValueOption%d", ICON_FK_WRENCH, i);
		if (ImGui::Button(options, ImVec2(20, 20)))
		{

		}

		ImNodes::EndInputAttribute();

		// TODO: Pop Color Styles
	}	
}

void SQUE_RenderGraph::UpdateNodeOutputs(RenderStep* step, int& item_id, const int node_id)
{
	sque_vec<RenderValue>& outputs = step->output_data;
	// Outputs
	static char options[32];
	for (uint16_t i = 0; i < outputs.size(); ++i)
	{
		// TODO: Push Color Styles
		ImNodes::BeginOutputAttribute(item_id++);

		sprintf(options, "%s##OutValueOption%d", ICON_FK_WRENCH, i);
		if (ImGui::Button(options, ImVec2(20, 20)))
		{

		}
		ImGui::SameLine();
		
		if (editing_name)
		{
			static char id[26];
			sprintf(id, "##%s%d", outputs[i].name, item_id);
			ImGui::InputText(id, outputs[i].name, sizeof(outputs[i].name));
		}
		else
			ImGui::Text(outputs[i].name);
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
			ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, title_color[step->type]);
			ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, title_selected_color[step->type]);
			
			ImNodes::BeginNode(item_id++);
			
			UpdateNodeTitleBar(step, item_id, node_id);
			UpdateNodeOptions(step, item_id, node_id);

			ImGui::PushItemWidth(64 * 3 - 20);

			UpdateNodeInputs(step, item_id, node_id);
			
			ImVec2 node_dimentions = ImNodes::GetNodeDimensions(node_id);
			ImGui::Indent(node_dimentions.x / 3 - 30);

			UpdateNodeOutputs(step, item_id, node_id);

			ImGui::Unindent(node_dimentions.x / 3 - 30);

			ImGui::PopItemWidth();
			
			ImNodes::EndNode();

			ImNodes::PopColorStyle();
			ImNodes::PopColorStyle();
			ImNodes::PopColorStyle();
		}

		ImNodes::EndNodeEditor();

		UpdateRMMenu();
	}
	ImGui::End();
}

void SQUE_RenderGraph::CleanUp()
{

}
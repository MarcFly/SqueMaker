#include "render_graph.h"
#include "../assets_window/asset_window.h"
#include "../engine_ui.h"

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
			if (ImGui::MenuItem("Add Render Step"))
			{
				RenderStep* new_step = new RenderStep();
				sprintf(new_step->name, "NewRenderStep");
				new_step->frag_source_id = UINT32_MAX;
				new_step->vert_source_id = UINT32_MAX;
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

		if (ImGui::MenuItem("Add Texture Output"))
		{
			RenderStep* step = Render_GetStep(selected[0]);
			step->texture_names.push_back("OutputTex");
			step->framebuffer.textures.push_back(SQUE_Texture());
			step->internal_texture_ids.push_back(SQUE_RNG());

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

	ImNodes::EndNodeTitleBar();
}


void SQUE_RenderGraph::UpdateNodeOptions(RenderStep* step)
{
	// TODO: Change from buttons to another right mouse menu for each node specific
	// Node settings outside of the canvas
	// Smae with name changes, try on a per item basis somehow
	ImVec2 min;
	ImVec2 max;

	// Vertex Shader
	const SQUE_CtrlAsset* vert_s = AssetManager_GetConstAsset(step->vert_source_id);
	if (vert_s != NULL)
		ImGui::Text("VS: %s", vert_s->name);
	else
		ImGui::Text("VS: NotSet...");
	min = ImGui::GetItemRectMin();
	if (ImGui::Button(ICON_FK_LINK))
	{
		// popup to search all .sq_verts
	}
	max = ImGui::GetItemRectMax();
	FileDraggable* shader_drag = (FileDraggable*)EngineUI_CheckDroppedDraggable(min, max);
	if (shader_drag != NULL && (AssetManager_GetConstAsset(shader_drag->file_id)->type == FT_VERT_SHADER))
	{
		step->vert_source_id = shader_drag->file_id;
	}

	// Fragment Shader
	const SQUE_CtrlAsset* frag_s = AssetManager_GetConstAsset(step->frag_source_id);
	if (vert_s != NULL)
		ImGui::Text("FS: %s", frag_s->name);
	else
		ImGui::Text("FS: NotSet...");
	min = ImGui::GetItemRectMin();
	if (ImGui::Button(ICON_FK_LINK))
	{
		// popup to search all .sq_verts
	}
	max = ImGui::GetItemRectMax();

	if (shader_drag != NULL && (AssetManager_GetConstAsset(shader_drag->file_id)->type == FT_FRAG_SHADER))
	{
		step->vert_source_id = shader_drag->file_id;
	}
}	

void SQUE_RenderGraph::UpdateNodeInputs(RenderStep* step)
{
	sque_vec<SQUE_Uniform>& inputs = step->program.uniforms;

	static char options[32];
	for (uint16_t i = 0; i < inputs.size(); ++i)
	{
		// Push Color Style

		ImNodes::BeginInputAttribute(step->internal_uniform_ids[i]);
		if (editing_name)
		{
			static char id[32];
			sprintf(id, "##%s%d", inputs[i].name, inputs[i].id);
			ImGui::InputText("", inputs[i].name, sizeof(inputs[i].name));
		}
		else
			ImGui::Text(inputs[i].name);
		
		ImGui::SameLine();

		const char* type_string = RenderType_String(inputs[i].type);
		ImGui::Text(" - %s##InValueOption%d", type_string, i);

		ImNodes::EndInputAttribute();

		// TODO: Pop Color Styles
	}	
}

void SQUE_RenderGraph::UpdateNodeOutputs(RenderStep* step)
{
	// Outputs
	static char options[32];
	sque_vec<SQUE_Texture>& outputs = step->framebuffer.textures;
	sque_vec<std::string> tex_names = step->texture_names;
	for (uint16_t i = 0; i < outputs.size(); ++i)
	{
		// TODO: Push Color Styles
		ImNodes::BeginOutputAttribute(step->internal_texture_ids[i]);

		if (editing_name)
		{
			static char id[32];
			sprintf(id, "##%s%d", tex_names[i], outputs[i].id);
			//ImGui::InputText(id, tex_names[i]., sizeof(tex_names[i]));
			ImGui::Text("NameNotChangeable,WIP...");
		}
		else
			ImGui::Text(tex_names[i].c_str());
		
		ImGui::SameLine();

		const char* type_string = TextureType_String(outputs[i].use_format);
		ImGui::Text("(%s)", type_string, i);

		
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
				
			// Specific RenderStepColor?
			
			ImNodes::BeginNode(step->id);
			
			UpdateNodeTitleBar(step);
			UpdateNodeOptions(step);

			ImGui::PushItemWidth(64 * 3 - 20);

			UpdateNodeInputs(step);
			
			ImVec2 node_dimentions = ImNodes::GetNodeDimensions(step->id);
			ImVec2 sep_pos = ImGui::GetCursorScreenPos();
			sep_pos.y += 5;
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddLine(sep_pos, ImVec2(sep_pos.x + node_dimentions.x-20, sep_pos.y), ImGui::GetColorU32(ImVec4()), 5);
			ImGui::SetCursorPosY(sep_pos.y - 55);
			UpdateNodeOutputs(step);

			//ImGui::Unindent(node_dimentions.x / 3 - 30);

			ImGui::PopItemWidth();
			
			ImNodes::EndNode();

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

				// - render_steps[i]->id id has to be managed with render id... to make them a bit more unique for imnodes
				RenderValue_Link in = Render_GetValue(new_link.in_id);
				RenderValue_Link out = Render_GetValue(new_link.out_id);

				if (in.id != UINT32_MAX && out.id != UINT32_MAX && in.type == out.type)
				{
					for (list_node<AttributeLink>* it = links.begin(); it != links.end(); it = it->next)
					{
						if (it->_data->in_id == in.id)
						{
							it->_data->out_id = out.id;
							out.id = UINT32_MAX;
							break;
						}
					}
					if(out.id != UINT32_MAX) links.push_back(new_link);
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
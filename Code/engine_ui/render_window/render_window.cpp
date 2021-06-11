#include "./render_window.h"

void SQUE_RenderWindow::Init()
{
    active = true;
    EngineUI_RegisterItem(this);

    name = "Render_Window";
    render_step_ref = Render_GetSteps().size()-1; // Get Last by default
    output_value_ref = -1;
    // This will break unless there is a default render step...
}

void SQUE_RenderWindow::Update(float dt)
{
    if(ImGui::Begin(name, &active))
    {
        sque_vec<RenderStep*>& steps = Render_GetSteps();
        if (steps.size() < 1)
        {
            ImGui::End(); return;
        }
        ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2 - 20);
        bool step_selected;
        if (render_step_ref > steps.size())
            step_selected = ImGui::BeginCombo("##RenderSteps", steps[0]->name);
        else
            step_selected = ImGui::BeginCombo("##RenderSteps", steps[render_step_ref]->name);
        if(step_selected)
        {
            bool is_selected;
            static char selectable_name[32];
            for (uint16_t i = 0; i < steps.size(); ++i)
            {
                is_selected = (i == render_step_ref);
                sprintf(selectable_name, "%s##%u", steps[i]->name, i);
                if (ImGui::Selectable(selectable_name, &is_selected))
                {
                    render_step_ref = i;
                    break;
                }
            }
            
            ImGui::EndCombo();
        }
        if (render_step_ref > steps.size())
        {
            ImGui::End();
            return;
        }
        else
        {
            ImGui::SameLine();
            RenderStep* step = steps[render_step_ref];
            sque_vec<SQUE_Texture>& output = step->framebuffer.textures;
            if (output.size() > 0)
            {
                bool combo_open = false;

                if (output_value_ref > output.size())
                    combo_open = ImGui::BeginCombo("##OutValues", step->texture_names[0].c_str());
                else
                    combo_open = ImGui::BeginCombo("##OutValues", step->texture_names[output_value_ref].c_str());
                if (combo_open)
                {
                    bool is_selected;
                    for (uint16_t i = 0; i < output.size(); ++i)
                    {
                        is_selected = (i == output_value_ref);
                        char name[64];
                        sprintf(name, "%s%u##%u", step->texture_names[i].c_str(), i,i);
                        if (ImGui::Selectable(name, &is_selected))
                        {
                            output_value_ref = i;
                            break;
                        }

                    }
                    ImGui::EndCombo();
                }
            }
        }
        ImGui::PopItemWidth();

        ImGui::BeginChild("RenderOutput");
        if(render_step_ref != -1)
        {    
            RenderStep* r_step = steps[render_step_ref];
            ImGui::GetWindowSize();
            if (output_value_ref != -1)
            {
                ImVec2 wsize = ImGui::GetWindowSize();
                // Data should not be held in values, but in the compiled steps
                // Compiled 
                uint32_t id = r_step->framebuffer.textures[output_value_ref].id;
                ImGui::Image((void*)r_step->framebuffer.textures[output_value_ref].id, wsize, ImVec2(0,1), ImVec2(1,0));
            }
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

void SQUE_RenderWindow::CleanUp()
{

}
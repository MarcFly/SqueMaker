#include "./render_window.h"

void SQUE_RenderWindow::Init()
{
    active = true;
    EngineUI_RegisterItem(this);

    name = "Render_Window";
    render_step_ref = Render_GetSteps().size()-1; // Get Last by default
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
        else 
        {
            rs_names.clear();
            for (uint16_t i = 0; i < steps.size(); ++i)
                rs_names.push_back(steps[i]->name);
        }
        if(ImGui::BeginCombo("##RenderSteps", rs_names[render_step_ref]))
        {
            bool is_selected;
            for (uint16_t i = 0; i < rs_names.size(); ++i)
            {
                is_selected = (i == render_step_ref);
                if (ImGui::Selectable(rs_names[i], &is_selected))
                {
                    render_step_ref = i;
                    break;
                }
            }

            if (steps[render_step_ref]->output_data.size() > 0)
            {
                t_names.clear();
                //for (uint16_t i = 0; i < steps[render_step_ref]->output_data.size(); ++i)

                if (ImGui::BeginCombo("##OutValues", t_names[output_value_ref]))
                {

                }
            }
            ImGui::EndCombo();
        }
        
        if(render_step_ref != -1)
        {    
            RenderStep* r_step = Render_GetStep(render_step_ref);


        }
        ImGui::End();
    }
}

void SQUE_RenderWindow::CleanUp()
{

}
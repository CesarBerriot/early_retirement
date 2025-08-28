#include "form.hpp"
#include <vector>
#include <string>
#include <imgui.h>

static form instance;

void form::draw()
{	draw_inputs();
	draw_results_table();
	draw_action_buttons();
}

void form::draw_inputs()
{	category
	(	"Inputs",
		[this]
		{	ImGui::BeginDisabled(is_running_asynchronous_task());
			ImGui::InputFloat("Initial Share Value", &inputs.share_value.value, 0, 0, "%.2f");
			ImGui::InputFloat("Monthly Yield", &inputs.monthly_yield.value, 0, 0, "%.2f%%");
			ImGui::InputFloat("Target Monthly Value Yield", &inputs.target_monthly_value_yield.value, 0, 0, "%.2f");
			ImGui::InputInt("Owned Shares", &inputs.owned_shares.value);
			ImGui::InputInt("Monthly Budget", &inputs.monthly_budget.value);
			ImGui::EndDisabled();
			inputs.share_value.clamp();
			inputs.monthly_yield.clamp();
			inputs.target_monthly_value_yield.clamp();
			inputs.owned_shares.clamp();
			inputs.monthly_budget.clamp();
		}
	);
}

void form::draw_results_table()
{	results_mutex.lock();
	std::vector<result> results = this->results;
	results_mutex.unlock();

	std::vector<std::string> columns = { "Month", "Owned Shares", "Leftover Budget", "Share Value", "Total Value", "Monthly Value Yield" };
	if(ImGui::BeginTable("Results", columns.size(), ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit))
	{	for(std::string column : columns)
		ImGui::TableSetupColumn(column.c_str());
		ImGui::TableHeadersRow();
		for(result result : results)
		{	ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("%i", result.month);
			ImGui::TableNextColumn();
			ImGui::Text("%i", result.owned_shares);
			ImGui::TableNextColumn();
			ImGui::Text("%i", result.leftover_budget);
			ImGui::TableNextColumn();
			ImGui::Text("%.2f", result.share_value);
			ImGui::TableNextColumn();
			ImGui::Text("%.2f", result.total_value);
			ImGui::TableNextColumn();
			ImGui::Text("%.2f", result.monthly_value_yield);
		}
		ImGui::EndTable();
	}

	results_mutex.lock();
	int total = results.empty() ? 0 : results.back().month;
	results_mutex.unlock();
	int years, months;
	if(total > 11)
	{	months = total % 12;
		years = (total - months) / 12;
	}
	else
	{	months = total;
		years = 0;
	}
	ImGui::Text("Total : %i Years, %i Months", years, months);

	spacer();
}

void form::draw_action_buttons()
{	ImGui::BeginDisabled(is_running_asynchronous_task());
	if(ImGui::Button("Compute"))
		run_asynchronous_task
		(	[this]
			{	results_mutex.lock();
				results.clear();
				results_mutex.unlock();

				result result = {};
				auto &
					[	month,
						owned_shares,
						leftover_budget,
						share_value,
						total_value,
						monthly_value_yield
					] = result;

				owned_shares = inputs.owned_shares.value;
				share_value = inputs.share_value.value;
				monthly_value_yield = share_value * inputs.monthly_yield.value / 100;

				while(monthly_value_yield < inputs.target_monthly_value_yield.value)
				{	++month;
					leftover_budget += inputs.monthly_budget.value;
					while(leftover_budget >= share_value)
					{	++owned_shares;
						leftover_budget -= share_value;
					}
					total_value = share_value * owned_shares;
					monthly_value_yield = total_value * inputs.monthly_yield.value / 100 - total_value;
					results_mutex.lock();
					results.push_back(result);
					results_mutex.unlock();
					share_value *= inputs.monthly_yield.value / 100;
				}
			}
		);
	ImGui::SameLine();
	if(ImGui::Button("Reset"))
		run_asynchronous_task
		(	[this]
			{	results_mutex.lock();
				results.clear();
				results_mutex.unlock();
			}
		);
	ImGui::EndDisabled();
}

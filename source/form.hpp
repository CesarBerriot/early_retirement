#include <dial/form_base.hpp>

class form final : dial::form_base
{	private:
		struct result
		{	int
				month,
				owned_shares,
				leftover_budget;
			float
				share_value,
				total_value,
				monthly_value_yield;
		};
	private:
		struct
		{	template<typename type>
			struct clamped_value
			{	public:
					type value;
				private:
					type const minimum_value;
				public:
					clamped_value(type minimum_value = 1)
					: value(minimum_value), minimum_value(minimum_value)
					{}
					void clamp() { value = std::max(value, minimum_value); }
			};
			clamped_value<float>
				share_value,
				monthly_yield = { 100.1 },
				target_monthly_value_yield;
			clamped_value<int>
				owned_shares = { 0 },
				monthly_budget = { 0 };
		} inputs;
		std::vector<result> results;
		std::mutex results_mutex;
	private:
		std::string get_name() override { return "form"; }
		void draw() override;
		void draw_inputs();
		void draw_results_table();
		void draw_action_buttons();
};
